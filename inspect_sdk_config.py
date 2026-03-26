import sys
import os
sdk_root = os.environ.get("AXELERA_FRAMEWORK", "/data/voyager-sdk")
if sdk_root not in sys.path:
    sys.path.append(sdk_root)
try:
    from axelera.app import config as ax_config, yaml_parser
    model_path = os.path.join(sdk_root, "ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml")
    
    parser = ax_config.create_inference_argparser(yaml_parser.get_network_yaml_info())
    # Try the syntax -d 0:2 (Device 0, Core 2)
    try:
        args = parser.parse_args([model_path, "usb:0", "-d", "0:2"])
        print("SUCCESS with -d 0:2")
        pc  = ax_config.PipelineConfig.from_parsed_args(args)
        print(f"  aipu_cores: {pc.aipu_cores}")
        print(f"  device_selector: {pc.device_selector}")
    except Exception as e:
        print(f"FAILED with -d 0:2: {e}")

except Exception as e:
    import traceback
    traceback.print_exc()
