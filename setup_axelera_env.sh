#!/bin/bash

# Configuration
SDK_ROOT="/data/voyager-sdk"
RUNTIME_15="/opt/axelera/runtime-1.5.2-1"
DEVICE_15="/opt/axelera/device-1.5.2-1/omega"

# Plugin and Library Paths
OP_LIB="${SDK_ROOT}/operators/lib"
RT_LIB_14="${RUNTIME_14}/lib"
RT_LIB_12="${RUNTIME_12}/lib"

# Export Environment Variables
export AXELERA_FRAMEWORK="${SDK_ROOT}"
export AXELERA_RUNTIME_DIR="${RUNTIME_15}"
export AXELERA_DEVICE_DIR="${DEVICE_15}"
export AXELERA_DEVICE="/dev/metis-0:1:0"

# Missing Firmware Variables
export AIPU_RUNTIME_STAGE0_OMEGA="${DEVICE_15}/bin/start_axelera_runtime_stage0.bin"
export AIPU_FIRMWARE_OMEGA="${DEVICE_15}/firmware/board_controller_fw_antelao_rev1_dvt_v7.1.bin"

export PATH="/opt/axelera/riscv-gnu-newlib-toolchain-409b951ba662-7/bin:${PATH}"
export LD_LIBRARY_PATH="${OP_LIB}:${RUNTIME_15}/lib:${LD_LIBRARY_PATH}"
export GST_PLUGIN_PATH="${OP_LIB}:${RUNTIME_15}/lib/gstreamer-1.0:${GST_PLUGIN_PATH}"
export PYTHONPATH="${RUNTIME_15}/tvm/tvm-src:${PYTHONPATH}"

# Clear GStreamer Registry Cache (Forces Rescan)
REGISTRY_FILE="${HOME}/.cache/gstreamer-1.0/registry.aarch64.bin"
if [ -f "$REGISTRY_FILE" ]; then
    echo "Clearing GStreamer registry cache: $REGISTRY_FILE"
    rm -f "$REGISTRY_FILE"
fi

# Verification
echo "Verifying axinplace discovery..."
if gst-inspect-1.0 axinplace > /dev/null 2>&1; then
    echo "OK: axinplace element found."
else
    echo "WARNING: axinplace element NOT found. Environment may be incomplete."
fi

echo "Axelera GStreamer environment configured successfully."
