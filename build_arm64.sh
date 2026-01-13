#!/bin/bash
set -e

# Build the docker image
# Using linux/arm64 architecture
docker build --platform linux/arm64 -f Dockerfile.jammy.arm64 -t edgex-gui-builder-arm64 .

# Create a container to extract the deb file
id=$(docker create --platform linux/arm64 edgex-gui-builder-arm64)

# Copy the deb file out
docker cp $id:/build/edgex-qt-ui_1.0.0_arm64.deb ./edgex-qt-ui_1.0.0_arm64.deb

# Remove the container
docker rm $id

echo "Done! Package saved to edgex-qt-ui_1.0.0_arm64.deb"
