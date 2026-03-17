import os
import sys
from pathlib import Path

# Setup SDK Path
SDK_ROOT = "/data/voyager-sdk"
if SDK_ROOT not in sys.path:
    sys.path.insert(0, SDK_ROOT)

from axelera.app import config as ax_config, yaml_parser
from axelera.app.stream import create_inference_stream

def run():
    model_name = "/data/voyager-sdk/ax_models/zoo/yolo/object_detection/yolov8n-coco-onnx.yaml"
    source = "usb:20"
    
    # Load SDK Arguments
    network_yaml_info = yaml_parser.get_network_yaml_info()
    parser = ax_config.create_inference_argparser(network_yaml_info)
    
    # Simulate parser logic from axelera_server.py
    args = parser.parse_args([
        model_name, 
        source, 
        "--display", "none",
        "--metis", "m2",
        "--aipu-cores", "4"
    ])
    args.data_root = Path(os.path.join(SDK_ROOT, "ax_models"))
    args.build_root = Path(os.path.join(SDK_ROOT, "build"))
    
    print(f"Starting test: Model={model_name}, Source={source}")
    
    stream = None
    try:
        sys_cfg = ax_config.SystemConfig.from_parsed_args(args)
        inf_cfg = ax_config.InferenceStreamConfig.from_parsed_args(args)
        pipe_cfg = ax_config.PipelineConfig.from_parsed_args(args)
        log_cfg = ax_config.LoggingConfig.from_parsed_args(args)
        dep_cfg = ax_config.DeployConfig.from_parsed_args(args)
        
        stream = create_inference_stream(sys_cfg, inf_cfg, pipe_cfg, log_cfg, dep_cfg)
        
        print("Stream created successfully. Pulling 5 frames...")
        count = 0
        for res in stream:
            det_count = len(res.detections) if hasattr(res, 'detections') else 'N/A'
            print(f"Frame {count} received. Detections: {det_count}")
            count += 1
            if count >= 5:
                break
        print("Success!")
    except Exception as e:
        print(f"Error during test: {e}")
        import traceback
        traceback.print_exc()
    finally:
        if stream:
            stream.stop()

if __name__ == "__main__":
    run()
