import os
import sys
import time
import json
import socket
import threading
import struct
import logging
import argparse
import datetime
import requests
import signal
from pathlib import Path

# --- Global Exception Handling ---
def global_exception_handler(exctype, value, traceback):
    logging.getLogger("MASTER").critical("Unhandled Exception:", exc_info=(exctype, value, traceback))
    # Don't call sys.exit() here to allow threads to potentially stay alive or for the port to remain bound
    # However, if it's the main thread, we might be in trouble.

sys.excepthook = global_exception_handler

# --- Constants & Globals ---
DEFAULT_CONFIG = "config_multi_camera.json"
TELEMETRY_PORT = 5566
COMMAND_PORT = 5567
VIDEO_PORT = 5568

_running = True
_latest_jpeg = None
_latest_jpeg_lock = threading.Lock()
_gui_clients = []
_gui_clients_lock = threading.Lock()
_stream = None          # The actual SDK stream object
_stream_lock = threading.Lock()
_stop_event = threading.Event()  # Signals the inference loop to stop
_inference_done = threading.Event()  # Set when thread fully exits

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - [%(levelname)s] - %(message)s')
logger = logging.getLogger("MASTER")

# SDK Root
_sdk_root = os.environ.get("AXELERA_FRAMEWORK", "/data/voyager-sdk")
if _sdk_root not in sys.path:
    sys.path.append(_sdk_root)


# ------------------------------------------------------------------------------
# VIDEO SERVER
# ------------------------------------------------------------------------------
def handle_video_client(conn, addr):
    global _latest_jpeg, _running
    c_logger = logging.getLogger(f"VideoClient-{addr[0]}")
    c_logger.info(f"Video client connected: {addr}")
    conn.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    last_sent_frame = None
    try:
        while _running:
            with _latest_jpeg_lock:
                frame = _latest_jpeg
            if frame and frame is not last_sent_frame:
                header = struct.pack(">I", len(frame))
                conn.sendall(header + frame)
                last_sent_frame = frame
            else:
                time.sleep(0.01)
    except Exception as e:
        c_logger.debug(f"Client disconnected: {e}")
    finally:
        conn.close()

def run_video_server(port):
    global _running
    srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("0.0.0.0", port))
    srv.listen(10)
    srv.settimeout(1.0)
    logger.info(f"Video server on port {port}")
    while _running:
        try:
            conn, addr = srv.accept()
            threading.Thread(target=handle_video_client, args=(conn, addr), daemon=True).start()
        except socket.timeout:
            continue
    srv.close()

# ------------------------------------------------------------------------------
# TELEMETRY SERVER
# ------------------------------------------------------------------------------
def run_telemetry_server(port):
    global _running, _gui_clients
    srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("0.0.0.0", port))
    srv.listen(10)
    srv.settimeout(1.0)
    logger.info(f"Telemetry server on port {port}")

    while _running:
        try:
            conn, addr = srv.accept()
            logger.info(f"Telemetry client: {addr}")
            with _gui_clients_lock:
                _gui_clients.append(conn)
        except socket.timeout:
            continue
    srv.close()

def broadcast_telemetry(data):
    global _gui_clients
    if not _gui_clients:
        return
    try:
        msg = json.dumps(data).encode('utf-8')
        # Packet: [total_len(4)][json_len(4)][flags(4)][json] (Little Endian)
        packet = struct.pack("<III", 12 + len(msg), len(msg), 0) + msg
        with _gui_clients_lock:
            dead = []
            for client in _gui_clients:
                try:
                    client.sendall(packet)
                except Exception:
                    dead.append(client)
            for d in dead:
                _gui_clients.remove(d)
    except Exception:
        pass

# ------------------------------------------------------------------------------
# EdgeX Coordination
# ------------------------------------------------------------------------------
_last_sent_to_edgex = {}  # Format: {name: timestamp}
_edgex_lock = threading.Lock()
_profile_cache = {}

def fetch_device_profile(profile_name):
    """
    Fetch device profile from EdgeX metadata service
    """
    if profile_name in _profile_cache:
        return _profile_cache[profile_name]

    try:
        url = f"http://localhost:59881/api/v3/deviceprofile/name/{profile_name}"
        r = requests.get(url, timeout=3)

        if r.status_code != 200:
            return None

        data = r.json()
        profile = data.get("profile") or data.get("deviceProfile") or {}
        resources = profile.get("deviceResources", [])

        profile_map = {}

        for res in resources:
            name = res["name"]
            value_type = res["properties"]["valueType"]
            profile_map[name] = value_type

        _profile_cache[profile_name] = profile_map
        return profile_map

    except Exception as e:
        logger.warning(f"Device profile fetch failed: {e}")
        return None

def generate_auto_payload(profile_map, ai_results):
    """
    Generate payload dynamically based on device profile
    """
    payload = {}

    for resource, dtype in profile_map.items():
        key = resource.lower()

        if key in ai_results:
            payload[resource] = ai_results[key]
            continue

        # Default auto values
        if dtype in ["Int32", "Int16", "Int64"]:
            payload[resource] = int(ai_results.get(key, 0))
        elif dtype in ["Float32", "Float64"]:
            payload[resource] = float(ai_results.get(key, 0.0))
        elif dtype == "String":
            payload[resource] = str(ai_results.get(key, "unknown"))
        else:
            payload[resource] = str(ai_results.get(key, ""))

    return payload

def send_edgex_payload(device_name, profile_name, readings=None, template=None):

    if not device_name:
        return

    # Mode 1: Manual payload with placeholder replacement
    if template:
        try:
            # Recursive placeholder replacement
            def replace_placeholders(obj):
                if isinstance(obj, str):
                    for k, v in (readings or {}).items():
                        placeholder = "${" + str(k) + "}"
                        if placeholder in obj:
                            # Handle numeric conversions if needed, but usually SenML 'v' is numeric
                            obj = obj.replace(placeholder, str(v))
                    return obj
                elif isinstance(obj, list):
                    return [replace_placeholders(i) for i in obj]
                elif isinstance(obj, dict):
                    return {k: replace_placeholders(v) for k, v in obj.items()}
                return obj

            payload = replace_placeholders(template)
        except Exception as e:
            logger.warning(f"Template placeholder replacement failed: {e}")
            payload = readings
    # Mode 2: Auto payload (simplified fallback)
    else:
        payload = readings

    if not payload:
        return

    url = f"http://localhost:4000/core-data/api/v3/event/device-rest/{device_name}/{profile_name}/all"

    try:
        def post_task():
            try:
                # Some EdgeX endpoints expect a single object, some expect an array (SenML)
                # If template was an array, send as-is.
                requests.post(url, json=payload, timeout=2)
                logger.info(f"EdgeX payload sent ({device_name})")
            except Exception as e:
                logger.warning(f"EdgeX send failed: {e}")
        threading.Thread(target=post_task, daemon=True).start()
    except Exception as e:
        logger.warning(f"Failed to start POST thread: {e}")


# ------------------------------------------------------------------------------
# INFERENCE ENGINE
# ------------------------------------------------------------------------------
def _run_inference(cfg):
    global _stream, _latest_jpeg, _running
    log = logging.getLogger("Inference")
    try:
        from axelera.app import config as ax_config, yaml_parser
        from axelera.app.stream import create_inference_stream
        import cv2
    except Exception as e:
        log.error(f"Axelera SDK import error: {e}")
        _inference_done.set()
        return

    # ---- Parse model name/path ----
    model_raw = str(cfg.get("modelPath", "yolov8n-coco-onnx")).strip()
    model = model_raw

    if not os.path.isabs(model) and not os.path.exists(model):
        zoo_info = yaml_parser.get_network_yaml_info()
        if model not in zoo_info:
            search_dirs = [
                os.path.join(_sdk_root, "ax_models"),
                os.path.join(_sdk_root, "ax_models/zoo/yolo/object_detection"),
                os.path.join(_sdk_root, "ax_models/zoo/yolo/segmentation"),
                os.path.join(_sdk_root, "ax_models/zoo/yolo/pose_estimation"),
            ]
            found = False
            for d in search_dirs:
                p = os.path.join(d, f"{model}.yaml")
                if os.path.exists(p):
                    model = p
                    found = True
                    break
            if not found:
                log.warning(f"Model '{model}' not found in zoo or search paths. Passing as-is.")

    if not model:
        model = "yolov8n-coco-onnx"

    # ---- Parse camera source ----
    src = str(cfg.get("cameraSource", "usb:20")).strip()
    if src.isdigit():
        src = f"usb:{src}"
    elif src.startswith("/dev/video"):
        num = src.split("/dev/video")[1].split("/")[0]
        src = f"usb:{num}"

    # ---- Parse AIPU cores ----
    cores_raw = str(cfg.get("aipuCores", "4")).strip()
    if "," in cores_raw:
        cores = len([c for c in cores_raw.split(",") if c.strip()])
    else:
        try:
            cores = int(cores_raw)
        except Exception:
            cores = 4

    conf = float(cfg.get("confidenceThreshold", 0.4))

    # ---- Load class map for detection labels ----
    # FIX 4: Explicit key/value type check to avoid silent inversion of class map.
    # Previously, any non-string first value would trigger inversion without validation.
    class_map = {}
    class_map_path = cfg.get("classMapPath", "")
    if class_map_path and os.path.exists(str(class_map_path)):
        try:
            with open(class_map_path, 'r') as f:
                raw_map = json.load(f)
                if raw_map:
                    first_key, first_val = next(iter(raw_map.items()))
                    if isinstance(first_val, str):
                        # Format: {"0": "person", "1": "car", ...} — use as-is
                        class_map = {str(k): v for k, v in raw_map.items()}
                    elif isinstance(first_val, int) and isinstance(first_key, str):
                        # Format: {"person": 0, "car": 1, ...} — invert intentionally
                        class_map = {str(v): k for k, v in raw_map.items()}
                    else:
                        log.warning(
                            f"Unexpected class map format (key={type(first_key).__name__}, "
                            f"val={type(first_val).__name__}). Skipping class map."
                        )
            log.info(f"Loaded class map: {len(class_map)} classes from {class_map_path}")
        except Exception as e:
            log.warning(f"Class map load error: {e}")

    # EdgeX context
    edgex_device  = cfg.get("edgexDeviceName", "")
    edgex_profile = cfg.get("edgexProfileName", "")
    senml_template = cfg.get("edgexPayloadTemplate", None)

    log.info(f"START: model={model} source={src} cores={cores} conf={conf} edgex={edgex_device}/{edgex_profile}")

    # FIX 3: Preserve original sys.argv and restore it after parsing to avoid
    # corrupting global state in a multi-threaded server environment.
    original_argv = sys.argv[:]
    sys.argv = [sys.argv[0]]
    
    # Use a custom ArgumentParser that doesn't call sys.exit()
    class NoExitParser(argparse.ArgumentParser):
        def exit(self, status=0, message=None):
            if message: logger.warning(f"Argparse exit suppression: {message}")
            raise Exception(f"Argparse failed: {message}")

    try:
        parser = NoExitParser(ax_config.create_inference_argparser(yaml_parser.get_network_yaml_info())._optionals)
        # Actually create_inference_argparser returns a full parser, we just need to ensure it doesn't exit.
        # Simple way: just catch SystemExit or override the exit method of the returned parser.
        parser = ax_config.create_inference_argparser(yaml_parser.get_network_yaml_info())
        def mock_exit(status=0, message=None):
            raise Exception(f"Argparse exit: {message}")
        parser.exit = mock_exit
        
        args = parser.parse_args([model, src, "--display", "none", "--metis", "m2", "--aipu-cores", str(cores)])
    except Exception as e:
        log.error(f"Failed to parse inference args: {e}")
        sys.argv = original_argv
        return

    sys.argv = original_argv

    args.data_root  = Path(_sdk_root) / "ax_models"
    args.build_root = Path(_sdk_root) / "build"

    sdk_stream = None
    try:
        sc  = ax_config.SystemConfig.from_parsed_args(args)
        stc = ax_config.InferenceStreamConfig.from_parsed_args(args)
        pc  = ax_config.PipelineConfig.from_parsed_args(args)
        lc  = ax_config.LoggingConfig.from_parsed_args(args)
        dc  = ax_config.DeployConfig.from_parsed_args(args)

        sdk_stream = create_inference_stream(sc, stc, pc, lc, dc)
        with _stream_lock:
            _stream = sdk_stream
        log.info("Stream online.")

        for res in sdk_stream:
            if _stop_event.is_set():
                log.info("Inference loop stop event detected.")
                break

            # Video frame - EXTREMELY DEFENSIVE
            try:
                # check if res has image and it's not empty
                if not hasattr(res, 'image') or res.image is None:
                    continue
                
                # Accessing height/width might trigger the crash if not careful
                h = getattr(res.image, 'height', 0)
                w = getattr(res.image, 'width', 0)
                if h == 0 or w == 0:
                    continue

                # The crash often happens here if the underlying buffer is invalid
                img = res.image.asarray("BGR")
                if img is None or img.size == 0 or img.shape[0] == 0 or img.shape[1] == 0:
                    continue

                ret, buf = cv2.imencode('.jpg', img, [cv2.IMWRITE_JPEG_QUALITY, 50])
                if ret:
                    with _latest_jpeg_lock:
                        _latest_jpeg = buf.tobytes()
            except BaseException as e:
                log.warning(f"Frame processing error (defended): {e}")
                continue

            # Detections
            dets = []
            try:
                m_map = res.meta._meta_map if hasattr(res, 'meta') and hasattr(res.meta, '_meta_map') else {}
            except Exception:
                m_map = {}

            base  = m_map.get("detections") or m_map.get("objects")

            if base:
                h = res.image.height
                w = res.image.width
                indices = range(len(base)) if hasattr(base, '__len__') else []
                for i in indices:
                    try:
                        obj   = base.Object(base, i) if hasattr(base, 'Object') else base[i]
                        score = float(obj.score)
                        if score >= conf:
                            box   = obj.box
                            label = "Object"
                            final_score = score

                            try:
                                detections_obj = m_map.get("detections")
                                sec_list = getattr(detections_obj, "_secondary_metas", {}).get("recognitions")

                                pass_filter = False
                                try:
                                    cls_id_num  = int(obj.class_id) if hasattr(obj, 'class_id') else -1
                                    pass_filter = (cls_id_num == 0)
                                except Exception:
                                    pass

                                filtered_idx = 0
                                if pass_filter:
                                    for j in range(i):
                                        try:
                                            prev_obj = base.Object(base, j) if hasattr(base, 'Object') else base[j]
                                            if int(prev_obj.class_id) == 0:
                                                filtered_idx += 1
                                        except Exception:
                                            pass

                                if pass_filter and sec_list and filtered_idx < len(sec_list):
                                    entry = sec_list[filtered_idx]
                                    if entry:
                                        recog = entry[0] if isinstance(entry, (list, tuple)) else entry
                                        if hasattr(recog, "_class_ids") and recog._class_ids:
                                            face_id = str(int(recog._class_ids[0][0]))
                                            label   = class_map.get(face_id, f"Person {face_id}")
                                            log.info(f"Face recognized: {label} (id:{face_id})")
                                        if hasattr(recog, "_scores") and recog._scores:
                                            final_score = float(recog._scores[0][0])
                                else:
                                    cls_id_num = int(obj.class_id) if hasattr(obj, 'class_id') else -1
                                    label = "Person" if cls_id_num == 0 else "Object"
                            except Exception as sec_e:
                                log.warning(f"Metadata extraction error at index {i}: {sec_e}")
                                label = "Object"

                            dets.append({
                                "label": label,
                                "score": final_score,
                                "box_norm": [
                                    float(box[0]) / w,
                                    float(box[1]) / h,
                                    float(box[2] - box[0]) / w,
                                    float(box[3] - box[1]) / h
                                ]
                            })
                    except Exception:
                        continue

                if dets:
                    labels = [d["label"] for d in dets]
                    log.info(f"Detections: {len(dets)} items - {', '.join(labels)}")

                    # --- EdgeX Integration (Dynamic Payload) ---
                    now_ts = time.time()
                    unique_current = set(labels)
                    should_send = False
                    
                    with _edgex_lock:
                        for lbl in unique_current:
                            if now_ts - _last_sent_to_edgex.get(lbl, 0.0) > 5.0:
                                should_send = True
                                _last_sent_to_edgex[lbl] = now_ts
                    
                    if should_send:
                        all_scores = [d["score"] for d in dets]
                        avg_conf = sum(all_scores) / len(all_scores) if all_scores else 0.0
                        bboxes = [d["box_norm"] for d in dets]
                        recognized = [l for l in labels if l != "Person" and l != "Object"]

                        readings = {
                            "bn": str(edgex_device or "Aetina-Face"),
                            "face_count": int(len(dets)),
                            "person_count": len([l for l in labels if l == "Person" or l in recognized]),
                            "recognized_names": ",".join(recognized) if recognized else "unknown",
                            "confidence": round(float(avg_conf) * 100.0, 2),
                            "bbox_coordinates": bboxes,
                            "frame_timestamp": datetime.datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ"),
                            "cam_temp": 42.0
                        }

                        send_edgex_payload(
                            str(edgex_device or "Aetina-Face"),
                            str(edgex_profile or "FaceRecog"),
                            readings,
                            template=senml_template
                        )

            # FIX 1: broadcast_telemetry moved to correct indentation level — inside
            # the `for res in sdk_stream` loop but OUTSIDE `if base:`, so it always
            # broadcasts per frame (with empty dets when nothing is detected), rather
            # than being accidentally outside the loop entirely.
            broadcast_telemetry({
                "type":         "detections",
                "detections":   dets,
                "edgexDevice":  edgex_device,
                "edgexProfile": edgex_profile
            })

    except Exception as e:
        log.error(f"Inference error: {e}")
    finally:
        if sdk_stream is not None:
            try:
                log.info("Stopping SDK stream and releasing hardware...")
                sdk_stream.stop()
                log.info("SDK stream stopped.")
            except Exception as e:
                log.warning(f"Stream stop error (may be OK): {e}")
        with _stream_lock:
            _stream = None
        _inference_done.set()
        log.info("Inference thread done.")


# ------------------------------------------------------------------------------
# COMMAND SERVER
# ------------------------------------------------------------------------------
def run_command_server(port, fallback):
    global _running, _stream
    srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("0.0.0.0", port))
    srv.listen(5)
    srv.settimeout(1.0)
    logger.info(f"Command server on port {port}")
    while _running:
        try:
            try:
                conn, addr = srv.accept()
                raw = conn.recv(10240).decode("utf-8").strip()
                if not raw:
                    conn.close()
                    continue
                cmd  = raw.split(":", 1)[0]
                body = raw.split(":", 1)[1] if ":" in raw else ""

                if cmd == "start":
                    try:
                        cfg = json.loads(body) if body else fallback
                    except Exception:
                        cfg = fallback

                    # Ensure we have something
                    if not cfg: cfg = fallback

                    with _stream_lock:
                        existing_stream = _stream

                    if existing_stream is not None:
                        logger.info("Stopping existing inference before switching model...")
                        _stop_event.set()
                        try:
                            existing_stream.stop()
                        except Exception:
                            pass
                        _inference_done.wait(timeout=10.0)
                        logger.info("Old stream released. Starting new model...")
                        time.sleep(1.0)

                    _stop_event.clear()
                    _inference_done.clear()
                    threading.Thread(target=_run_inference, args=(cfg,), daemon=True).start()
                    conn.sendall(b"OK\n")

                elif cmd == "stop":
                    with _stream_lock:
                        existing = _stream
                    if existing is not None:
                        logger.info("Stop requested — releasing AIPU cores...")
                        _stop_event.set()
                        try:
                            existing.stop()
                        except Exception:
                            pass
                        with _stream_lock:
                            _stream = None
                        conn.sendall(b"STOPPING\n")
                    else:
                        conn.sendall(b"IDLE\n")

                conn.close()
            except socket.timeout:
                continue
            except Exception as loop_e:
                logger.error(f"Command server loop error: {loop_e}")
                if 'conn' in locals() and conn:
                    try: conn.close()
                    except: pass
        except BaseException as main_loop_e:
            logger.error(f"Fatal Command Server error: {main_loop_e}")
            time.sleep(1) # prevent tight loop crash
    srv.close()


# ------------------------------------------------------------------------------
# MAIN
# ------------------------------------------------------------------------------
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--config", default=DEFAULT_CONFIG)
    args = parser.parse_args()
    try:
        with open(args.config, 'r') as f:
            cfg = json.load(f)["streams"][0]
    except Exception:
        cfg = {}

    threading.Thread(target=run_video_server,     args=(VIDEO_PORT,),       daemon=True).start()
    threading.Thread(target=run_telemetry_server,  args=(TELEMETRY_PORT,),   daemon=True).start()
    threading.Thread(target=run_command_server,    args=(COMMAND_PORT, cfg), daemon=True).start()

    logger.info("Axelera Server Online.")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        # FIX 2 (cont.): Signal inference thread to stop cleanly on shutdown,
        # not just the server loop. Previously _stop_event was never set here.
        logger.info("Shutting down...")
        _stop_event.set()
        _running = False
        time.sleep(1)