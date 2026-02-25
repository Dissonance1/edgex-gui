#!/bin/bash

# Set the base directory for all services (change as per your actual source/build location)
BASE_DIR="$HOME/src/edgex"
KUIPER_DIR="$BASE_DIR/ekuiper/_build/kuiper--linux-arm64/bin"

# Disable security
export SECURITY_SERVICE=false
export EDGEX_SECURITY_SECRET_STORE=false

# Helper to run services with labeled output
run_service() {
  local name=$1
  local dir=$2
  local cmd=$3

  echo "Starting $name..."
  (
    cd "$dir" || { echo "Failed to cd into $dir"; return 1; }
    $cmd 2>&1 | sed "s/^/[$name] /" &
  )
}

echo "Cleaning up old Redis/Consul processes..."
# Kill Redis on port 6379
REDIS_PID=$(sudo lsof -t -i:6379)
if [ -n "$REDIS_PID" ]; then
  sudo kill $REDIS_PID
  sleep 1
fi

# Kill Consul on port 8300
CONSUL_PID=$(sudo lsof -t -i:8300)
if [ -n "$CONSUL_PID" ]; then
  sudo kill $CONSUL_PID
  sleep 1
fi

# Start Redis in background
echo "Starting Redis..."
redis-server --loglevel verbose 2>&1 | sed 's/^/[REDIS] /' &

# Start Consul in background
echo "Starting Consul..."
consul agent -dev 2>&1 | sed 's/^/[CONSUL] /' &

# Start core-common-config-bootstrapper
run_service "config-bootstrapper" \
  "$BASE_DIR/edgex-go/cmd/core-common-config-bootstrapper" \
  "./core-common-config-bootstrapper -cp=consul://localhost:8500 -r"

# Start core services
run_service "core-metadata" \
  "$BASE_DIR/edgex-go/cmd/core-metadata" \
  "./core-metadata -cp=consul://localhost:8500 -r"
run_service "core-data" \
  "$BASE_DIR/edgex-go/cmd/core-data" \
  "./core-data -cp=consul://localhost:8500 -r"
run_service "core-command" \
  "$BASE_DIR/edgex-go/cmd/core-command" \
  "./core-command -cp=consul://localhost:8500 -r"

# Start support services
run_service "support-notifications" \
  "$BASE_DIR/edgex-go/cmd/support-notifications" \
  "./support-notifications -cp=consul://localhost:8500 -r"
run_service "support-scheduler" \
  "$BASE_DIR/edgex-go/cmd/support-scheduler" \
  "./support-scheduler -cp=consul://localhost:8500 -r"

# Start UI service
run_service "edgex-ui" \
  "$BASE_DIR/edgex-ui-go/cmd/edgex-ui-server" \
  "./edgex-ui-server"

run_service "device-rest" \
  "$BASE_DIR/device-rest-go/cmd" \
  "./device-rest -cp=consul://localhost:8500 -r"

# Uncomment to start Kuiper daemon
run_service "kuiperd" "$KUIPER_DIR" "./kuiperd"

# Start app-service-configurable
# run_service "app-service-configurable" \
#   "$BASE_DIR/app-service-configurable" \
#   "./app-service-configurable \
#   --configFile=config.yaml \
#   --configDir=res \
#   --profile=SenML \
#   --configProvider=consul://localhost:8500 \
#   --registry"

echo ""
echo "✅ All services started. Logs streaming below. Press Ctrl+C to stop."
wait
