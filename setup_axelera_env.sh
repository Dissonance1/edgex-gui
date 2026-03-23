#!/bin/bash

# Configuration
SDK_ROOT="/data/voyager-sdk"
RUNTIME_VERSION="1.5.2-1"
DEVICE_VERSION="1.5.2-1"
VIRTUAL_ENV="${SDK_ROOT}/venv"

RUNTIME_DIR="/opt/axelera/runtime-${RUNTIME_VERSION}"
DEVICE_DIR="/opt/axelera/device-${DEVICE_VERSION}/omega"

# Export Environment Variables
export VIRTUAL_ENV="${VIRTUAL_ENV}"
export AXELERA_RUNTIME_DIR="${RUNTIME_DIR}"
export AXELERA_DEVICE_DIR="${DEVICE_DIR}"
export AXELERA_FRAMEWORK="${SDK_ROOT}"
export AXELERA_DEVICE="/dev/metis-0:1:0"

export AIPU_RUNTIME_STAGE0_OMEGA="${DEVICE_DIR}/bin/start_axelera_runtime_stage0.bin"
export AIPU_FIRMWARE_OMEGA="${DEVICE_DIR}/bin/start_axelera_runtime.elf"
export AXELERA_RISCV_TOOLCHAIN_DIR="/opt/axelera/riscv-gnu-newlib-toolchain-409b951ba662-7"
export TVM_HOME="${RUNTIME_DIR}/tvm/tvm-src"
export PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python

# Library and Plugin Paths
# Crucial: LD_LIBRARY_PATH and GST_PLUGIN_PATH must be correct for axinplace to load correctly
export LD_LIBRARY_PATH="${RUNTIME_DIR}/lib:${SDK_ROOT}/operators/lib:${LD_LIBRARY_PATH}"
export GST_PLUGIN_PATH="${RUNTIME_DIR}/lib/gstreamer-1.0:${SDK_ROOT}/operators/lib:${GST_PLUGIN_PATH}"
export PYTHONPATH="${SDK_ROOT}:${RUNTIME_DIR}/tvm/tvm-src:${PYTHONPATH}"
export PKG_CONFIG_PATH="${RUNTIME_DIR}/lib/pkgconfig:${SDK_ROOT}/operators/lib/pkgconfig"

# Binary Paths
export PATH="${VIRTUAL_ENV}/bin:${RUNTIME_DIR}/bin:${AXELERA_RISCV_TOOLCHAIN_DIR}/bin:${PATH}"

# Additional SDK Variables
export TVM_HOME="${RUNTIME_DIR}/tvm/tvm-src"
export PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python

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
