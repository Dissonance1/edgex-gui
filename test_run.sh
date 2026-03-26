#!/bin/bash
ssh -o StrictHostKeyChecking=no aetina@10.12.83.250 << 'EOF'
  cd /data/edgex-gui
  source /data/voyager-sdk/venv/bin/activate
  export PATH=/usr/sbin:/usr/bin:/sbin:/bin:$PATH
  export AXELERA_FRAMEWORK=/data/voyager-sdk
  export PYTHONPATH=/data/edgex-gui:/data/voyager-sdk:/opt/axelera/runtime-1.4.0-1/tvm/tvm-src:$PYTHONPATH
  export GST_PLUGIN_PATH=/data/voyager-sdk/operators/lib:/data/voyager-sdk/operators/Release:/data/voyager-sdk/operators/Release/axstreamer:$GST_PLUGIN_PATH
  /data/voyager-sdk/venv/bin/python3 backend/launcher.py --config '{"edgex_enabled":false,"pipelines":[{"camera_id":"USB Camera 20","confidence":0.6,"model":"/data/voyager-sdk/ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml","source":"20"}]}'
EOF
