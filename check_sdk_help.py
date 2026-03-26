import sys
import os
sdk_root = os.environ.get("AXELERA_FRAMEWORK", "/data/voyager-sdk")
if sdk_root not in sys.path:
    sys.path.append(sdk_root)
try:
    from axelera.app import config as ax_config, yaml_parser
    # Create the parser exactly like axelera_server.py does
    parser = ax_config.create_inference_argparser(yaml_parser.get_network_yaml_info())
    parser.print_help()
except Exception as e:
    import traceback
    traceback.print_exc()
