#!/usr/bin/env python3
import os
import sys
import time
import json
import socket
import threading
import signal
from pathlib import Path

# Setup SDK Path
SDK_ROOT = os.environ.get("AXELERA_FRAMEWORK", "/data/voyager-sdk")
if SDK_ROOT not in sys.path:
    sys.path.insert(0, SDK_ROOT)
if not os.environ.get('AXELERA_FRAMEWORK'):
    sys.exit("Please set AXELERA_FRAMEWORK and LD_LIBRARY_PATH before running.")

# Force device path for Aetina
os.environ['AXELERA_DEVICE'] = '/dev/metis-0:1:0'


from axelera.app import config, logging_utils, yaml_parser
from axelera.app.stream import create_inference_stream

LOG = logging_utils.getLogger("InferenceBridge")

class MetadataServer:
    def __init__(self, socket_path="/tmp/ax_bridge/meta.sock"):

        self.socket_path = socket_path
        self.clients = []
        self.running = True
        if os.path.exists(self.socket_path):
            try:
                os.remove(self.socket_path)
            except:
                pass

            
    def start(self):
        self.thread = threading.Thread(target=self._run, daemon=True)
        self.thread.start()
        
    def _run(self):
        with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
            s.bind(self.socket_path)
            s.listen(5)
            s.settimeout(1.0)
            while self.running:
                try:
                    conn, addr = s.accept()
                    self.clients.append(conn)
                except socket.timeout:
                    continue
                except Exception as e:
                    LOG.error(f"Metadata server error: {e}")
                    
    def broadcast(self, data):
        message = json.dumps(data) + "\n"
        disconnected = []
        for client in self.clients:
            try:
                client.sendall(message.encode())
            except:
                disconnected.append(client)
        for d in disconnected:
            self.clients.remove(d)
            
    def stop(self):
        self.running = False
        if os.path.exists(self.socket_path):
            os.remove(self.socket_path)

import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst

class SHMWriter:
    def __init__(self, socket_path="/tmp/ax_bridge/shm.sock"):

        Gst.init(None)
        pipeline_str = f"appsrc name=src ! videoconvert ! video/x-raw,format=I420 ! shmsink socket-path={socket_path} sync=false wait-for-connection=false shm-size=100000000"
        self.pipeline = Gst.parse_launch(pipeline_str)
        self.src = self.pipeline.get_by_name("src")
        self.pipeline.set_state(Gst.State.PLAYING)
        
    def write_frame(self, frame):
        # frame is axelera.app.utils.Image object
        data = frame.asarray("BGR")
        h, w, _ = data.shape
        caps = Gst.Caps.from_string(f"video/x-raw,format=BGR,width={w},height={h},framerate=0/1")
        self.src.set_property("caps", caps)
        
        buf = Gst.Buffer.new_allocate(None, len(data.tobytes()), None)
        buf.fill(0, data.tobytes())
        self.src.emit("push-buffer", buf)

def run_bridge(model_yaml, source, shm_socket="/tmp/ax_bridge/shm.sock", cores="4"):

    meta_server = MetadataServer()
    meta_server.start()
    
    shm_writer = SHMWriter(shm_socket)

    args_list = [
        model_yaml, 
        source, 
        "--display", "none", 
        "--pipe", "gst",
        "--metis", "m2",
        "--aipu-cores", str(cores)
    ]
    network_yaml_info = yaml_parser.get_network_yaml_info()
    parser = config.create_inference_argparser(network_yaml_info)
    args = parser.parse_args(args_list)

    sys_cfg = config.SystemConfig.from_parsed_args(args)
    inf_cfg = config.InferenceStreamConfig.from_parsed_args(args)
    pipe_cfg = config.PipelineConfig.from_parsed_args(args)
    log_cfg = config.LoggingConfig.from_parsed_args(args)
    dep_cfg = config.DeployConfig.from_parsed_args(args)

    stream = create_inference_stream(sys_cfg, inf_cfg, pipe_cfg, log_cfg, dep_cfg)

    LOG.info(f"Bridge started. Source: {source}, Model: {model_yaml}, SHM: {shm_socket}, Cores: {cores}")
    
    try:
        for frame_result in stream:
            # Broadcast metadata
            detections = []
            if hasattr(frame_result, 'detections'):
                for d in frame_result.detections:
                    label = "object"
                    if hasattr(d, 'label') and hasattr(d.label, 'name'):
                        label = d.label.name
                    elif hasattr(d, 'class_id'):
                        label = f"class_{d.class_id}"
                        
                    detections.append({
                        "label": label,
                        "confidence": float(d.confidence) if hasattr(d, 'confidence') else 0.0,
                        "bounding_box": [int(x) for x in d.box] if hasattr(d, 'box') else []
                    })
            
            meta_server.broadcast({
                "stream_id": frame_result.stream_id,
                "detections": detections
            })
            
            # Write frame to SHM
            if frame_result.image:
                shm_writer.write_frame(frame_result.image)
            
    except KeyboardInterrupt:
        pass
    finally:
        stream.stop()
        meta_server.stop()
        shm_writer.pipeline.set_state(Gst.State.NULL)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: inference_bridge.py <model.yaml> <source> [cores]")
        sys.exit(1)
    
    cores = sys.argv[3] if len(sys.argv) > 3 else "4"
    run_bridge(sys.argv[1], sys.argv[2], cores=cores)
