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
import yaml
from pathlib import Path

# --- Constants & Globals ---
DEFAULT_CONFIG = "config_multi_camera.json"
PORT_VIDEO     = 5568
PORT_METADATA  = 5566
PORT_COMMAND   = 5567
_SKIP_FRAMES   = 1 # 1=none, 2=half, etc.

_stream        = None
_stop_event    = threading.Event()
_inference_done = threading.Event()
_latest_jpeg   = None
_running       = True
_video_enabled = True # Runtime toggle for JPEG encoding
_gui_clients    = []
_gui_clients_lock = threading.Lock()

_latest_jpeg_lock = threading.Lock()

# Configure logging
def setup_logging(log_file=None):
    handlers = [logging.StreamHandler(sys.stdout)]
    if log_file:
        log_dir = os.path.dirname(log_file)
        if log_dir and not os.path.exists(log_dir):
            os.makedirs(log_dir, exist_ok=True)
        handlers.append(logging.FileHandler(log_file))
    
    logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s - [%(levelname)s] - %(message)s',
        handlers=handlers
    )
    return logging.getLogger("MASTER")

logger = None # Will be initialized in main

# SDK Root
_sdk_root = os.environ.get("AXELERA_FRAMEWORK", "/data/voyager-sdk")
if _sdk_root not in sys.path:
    sys.path.append(_sdk_root)

# --- SDK Monkey Patch for Multi-Process Core Partitioning ---
# The SDK's DeviceManager hardcodes core_index to 0 for each process.
# We patch it to use AXELERA_CORE_OFFSET as the starting core.
try:
    import axelera.app.device_manager as device_manager
    import time
    
    _original_configure_boards = device_manager._AipuDeviceManager._configure_boards

    def _patched_configure_boards(self, nn):
        # Read the global core offset and limit for this process
        core_offset = int(os.environ.get("AXELERA_CORE_OFFSET", "0"))
        core_limit  = int(os.environ.get("AXELERA_CORE_LIMIT", "0"))

        # Global process bounds
        proc_start = core_offset
        proc_end   = proc_start + (core_limit if core_limit > 0 else 4) # Default to 4 cores if no limit
        current_offset = proc_start
        
        configures = {}
        last = proc_start # Initialize 'last' to the process start for tracking max core used
        patch_logger = logging.getLogger("SDK-PATCH")
        
        for task in nn.tasks:
            if not getattr(task, 'is_dl_task', True):
                continue
            
            # Use original cores as requested (just for logging)
            requested_cores = task.aipu_cores
            
            # Constrain task to the process allocated cores
            task_offset = current_offset
            task.aipu_cores = min(requested_cores, proc_end - task_offset)
            if task.aipu_cores < 1: task.aipu_cores = 1
            # For multitask pipelines, we stack tasks while staying within proc_end
            task_offset = current_offset
            task_end    = task_offset + task.aipu_cores
            
            patch_logger.info(f"Task '{getattr(task, 'name', 'unknown')}' "
                              f"(Model Req: {requested_cores}, Process Limit: {task.aipu_cores}) "
                              f"at Hardware Offset: {task_offset}")
            
            # Use SDK's internal logic to get clock and mvm
            clock = nn.model_infos.clock_profile(task.model_info.name, self.metis)
            mvm = nn.model_infos.mvm_limitation(task.model_info.name, self.metis)

            # Call original _get_configures with fixed process bounds
            configures.update(device_manager._get_configures(task_offset, task_end, clock, mvm))
            
            # Track the maximum core used across all tasks for the final clock report
            if task_end > last:
                last = task_end
            # Update current_offset to stack the next task
            current_offset = task_end

        if configures:
            conf_str = ', '.join(f"{k}={v}" for k, v in configures.items())
            patch_logger.info(f"--- CORE PARTITIONING ACTIVE ---")
            patch_logger.info(f"Process Base Offset: {proc_start}")
            patch_logger.info(f"Allocated Core Range: {list(range(proc_start, last))}")
            patch_logger.info(f"Board Configuration: {conf_str}")
            
            # Run the actual configuration on the device
            ready = [self.context.configure_device(d, **configures) for d in self.devices]
            if not all(ready):
                patch_logger.info("Waiting for AIPU cores to initialize...")
                while not all(self.context.device_ready(d) for d in self.devices):
                    time.sleep(0.3)
                patch_logger.info("AIPU cores READY.")
                    
        # Return the clock freqs for tracers (expects {core_id: freq})
        return device_manager._get_core_clocks(self.context, self.devices[0], proc_start, last)

    # Apply the patch
    device_manager._AipuDeviceManager._configure_boards = _patched_configure_boards
    logging.getLogger("SDK-PATCH").info("Successfully applied core offset monkey-patch to Axelera SDK")
except Exception as e:
    logging.getLogger("SDK-PATCH").error(f"CRITICAL: Failed to patch SDK: {e}")


# ==============================================================================
# VIDEO SERVER
# ==============================================================================

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


# ==============================================================================
# TELEMETRY SERVER
# ==============================================================================

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
        msg    = json.dumps(data).encode('utf-8')
        # Packet: [total_len(4)][json_len(4)][flags(4)][json]  (Little Endian)
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


# ==============================================================================
# EDGEX - PROFILE CACHE + AUTO RESOLVER
# ==============================================================================

# Cache structure:
#   _profile_cache = {
#       "resources":     list[dict],          # raw resource descriptors
#       "resolver_map":  dict[name -> fn],    # pre-built at fetch time
#   }
_profile_cache    = {}
_profile_lock     = threading.Lock()
_last_sent_to_edgex = {}
_edgex_lock         = threading.Lock()


def parse_device_profile(profile_json):
    """
    Extract deviceResources from any EdgeX v2/v3 profile JSON response.
    Returns list of {"name", "description", "valueType", "units"}.
    """
    profile = (
        profile_json.get("profile")
        or profile_json.get("deviceProfile")
        or profile_json
    )
    resources = []
    for res in profile.get("deviceResources", []):
        props = res.get("properties", {})
        resources.append({
            "name":        res.get("name", ""),
            "description": res.get("description", ""),
            "valueType":   props.get("valueType", "String"),
            "units":       props.get("units", ""),
        })
    return resources


def _build_resolver_map(resources):
    """
    Called ONCE when a profile is fetched.
    Analyses every resource name and returns a resolver_map:

        { resource_name: fn(inference_output) -> value }

    inference_output is the live dict produced each frame - it always
    contains the same fixed keys regardless of which profile is loaded.
    """
    import re

    # - Inference output keys (fixed, produced every frame) -------
    EXTRACTORS = {
        "device_name":        lambda o: o.get("device_name", ""),
        "face_count":         lambda o: o.get("face_count", 0),
        "person_count":       lambda o: o.get("person_count", 0),
        "animal_count":       lambda o: o.get("animal_count", 0),
        "total_count":        lambda o: o.get("total_count", 0),
        "known_count":        lambda o: o.get("known_count", 0),
        "confidence":         lambda o: o.get("confidence", 0.0),
        "recognized_names":   lambda o: o.get("recognized_names", "unknown"),
        "all_labels":         lambda o: o.get("all_labels", ""),
        "bbox_coordinates":   lambda o: o.get("bbox_coordinates", "[]"),
        "embeddings":         lambda o: o.get("embeddings", "[]"),
        "frame_timestamp":    lambda o: o.get("frame_timestamp", ""),
        "status":             lambda o: o.get("status", "active"),
        "cam_temp":           lambda o: o.get("cam_temp", 0.0),
    }

    # - Token -> extractor key table -------------------
    TOKEN_TABLE = [
        ({"device", "camera", "sensor", "bn"}, "device_name"),
        ({"face", "head"},                     "face_count"),
        ({"person", "human", "people"},        "person_count"),
        ({"animal", "pet", "beast"},           "animal_count"),
        ({"total", "all", "sum", "count"},     "total_count"),
        ({"known", "identified", "recog"},     "known_count"),
        ({"confidence", "score", "prob",
          "accuracy", "certainty"},            "confidence"),
        ({"recognized", "name", "identity",
          "who", "match", "names"},            "recognized_names"),
        ({"label", "class", "tag", "category",
          "object", "type", "all"},            "all_labels"),
        ({"bbox", "box", "coordinate", "rect",
          "region", "location", "coords"},     "bbox_coordinates"),
        ({"embed", "vector", "feature",
          "latent", "descriptor"},             "embeddings"),
        ({"time", "date", "stamp", "ts",
          "when", "epoch"},                    "frame_timestamp"),
        ({"status", "state", "active",
          "alive", "health"},                  "status"),
        ({"temp", "temperature", "heat",
          "thermal", "celsius"},               "cam_temp"),
    ]

    def _tokens(name):
        # 1. Split by non-alphanumeric
        s1 = re.split(r'[^a-zA-Z0-9]+', name.strip('_'))
        # 2. Split CamelCase (e.g., PersonCount -> Person, Count)
        tokens = []
        for part in s1:
            if not part: continue
            # re.sub finds transition from lower/digit to upper
            s2 = re.sub('([a-z0-9])([A-Z])', r'\1 \2', part)
            for t in s2.split():
                tokens.append(t.lower())
        return set(tokens)

    resolver_map = {}
    for res in resources:
        name       = res["name"]
        name_tokens = _tokens(name)

        matched_extractor = None

        # Pass 1 - exact extractor key match
        norm = name.lstrip("_").lower().replace(" ", "_").replace("-", "_")
        if norm in EXTRACTORS:
            matched_extractor = norm

        # Pass 2 - token table match
        if not matched_extractor:
            for token_set, extractor_key in TOKEN_TABLE:
                if name_tokens & token_set:
                    matched_extractor = extractor_key
                    break

        if matched_extractor:
            resolver_map[name] = EXTRACTORS[matched_extractor]
        else:
            vtype = res["valueType"]
            if vtype in {"Float32", "Float64"}:
                resolver_map[name] = lambda o: 0.0
            elif vtype in {"Int8","Int16","Int32","Int64","Uint8","Uint16","Uint32","Uint64"}:
                resolver_map[name] = lambda o: 0
            elif vtype == "Bool":
                resolver_map[name] = lambda o: False
            else:
                resolver_map[name] = lambda o: "unknown"

    logger.info(f"[ResolverBuild] Built resolver map for {len(resolver_map)} resources: {list(resolver_map.keys())}")
    return resolver_map


def fetch_and_cache_profile(profile_name):
    """
    Fetch profile from EdgeX API, parse resources, build resolver map,
    store everything in _profile_cache.
    """
    global _profile_cache
    try:
        url = f"http://localhost:59881/api/v3/deviceprofile/name/{profile_name}"
        r   = requests.get(url, timeout=5)
        if r.status_code != 200:
            logger.warning(f"Profile '{profile_name}' not found (HTTP {r.status_code})")
            return False

        resources    = parse_device_profile(r.json())
        resolver_map = _build_resolver_map(resources)

        with _profile_lock:
            _profile_cache = {
                "profile_name": profile_name,
                "resources":    resources,
                "resolver_map": resolver_map,
            }

        # Reset debug gate
        if hasattr(send_edgex_payload, "_debugged"):
            del send_edgex_payload._debugged

        logger.info(f"Profile '{profile_name}' cached - {len(resources)} resources.")
        return True
    except Exception as e:
        logger.warning(f"Profile fetch failed: {e}")
        return False


def async_fetch_profile(profile_name):
    """Fetch and cache profile in a background thread."""
    threading.Thread(
        target=fetch_and_cache_profile,
        args=(profile_name,),
        daemon=True
    ).start()

def build_payload(device_name, inference_output):
    """
    Build SenML payload for whatever profile is currently cached.
    """
    NUMERIC_TYPES = {
        "Float32", "Float64",
        "Int8",  "Int16",  "Int32",  "Int64",
        "Uint8", "Uint16", "Uint32", "Uint64",
    }
    BOOL_TYPES = {"Bool"}

    with _profile_lock:
        cache = _profile_cache.copy()

    if not cache:
        logger.warning("[PayloadBuilder] No profile cached - skipping.")
        return []

    resources    = cache["resources"]
    resolver_map = cache["resolver_map"]
    timestamp    = datetime.datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S UTC")
    payload      = []

    for res in resources:
        name       = res["name"]
        value_type = res["valueType"]
        unit       = res["description"] if res["description"] else res["units"]

        resolver = resolver_map.get(name)
        value    = resolver(inference_output) if resolver else None

        if value is None:
            value = 0 if value_type in NUMERIC_TYPES else (False if value_type in BOOL_TYPES else "unknown")

        record = {"bn": device_name, "n": name, "u": unit, "timestamp": timestamp}

        if value_type in NUMERIC_TYPES:
            try:
                record["v"]  = float(value) if "Float" in value_type else int(float(value))
            except (ValueError, TypeError):
                record["v"] = 0
        elif value_type in BOOL_TYPES:
            record["vb"] = bool(value)
        else:
            record["vs"] = str(value)

        payload.append(record)
    
    return payload


def debug_profile_vs_rawdata(resources, resolver_map, inference_output):
    """
    Log exactly how each resource was resolved.
    """
    logger.info("=" * 60)
    logger.info("[ProfileDebug] Resource Resolution:")
    for res in resources:
        name = res["name"]
        resolver = resolver_map.get(name)
        val = resolver(inference_output) if resolver else "N/A"
        logger.info(f"  {name} -> {val!r}")
    logger.info("=" * 60)


# ==============================================================================
# EDGEX - SEND PAYLOAD
# ==============================================================================

def send_edgex_payload(device_name, profile_name, inference_output=None, template=None):
    if not device_name:
        return

    if template:
        payload = template
    else:
        with _profile_lock:
            cache = _profile_cache.copy()

        if not cache:
            logger.warning(f"Profile '{profile_name}' not yet cached - skipping frame.")
            return

        # - Debug: log once per profile -----------------
        if not hasattr(send_edgex_payload, "_debugged"):
            debug_profile_vs_rawdata(cache["resources"], cache["resolver_map"], inference_output or {})
            send_edgex_payload._debugged = True

        payload = build_payload(device_name, inference_output or {})
        logger.info(f"Built {len(payload)}-record payload for '{device_name}'")

    url = f"http://localhost:4000/core-data/api/v3/event/device-rest/{device_name}/{profile_name}/all"

    def post_task():
        try:
            res = requests.post(url, json=payload, timeout=2)
            if res.status_code == 200:
                logger.info(f"EdgeX accepted payload for '{device_name}'")
            else:
                logger.warning(f"EdgeX HTTP {res.status_code}")
        except Exception as e:
            logger.warning(f"EdgeX upload error: {e}")

    threading.Thread(target=post_task, daemon=True).start()


# ==============================================================================
# INFERENCE ENGINE
# ==============================================================================

def _get_inference_configs(cfg, base_args):
    """Safely build Axelera SDK config objects without using from_parsed_args to avoid sys.exit triggers."""
    from axelera.app import config as ax_config
    
    # GUI config mapping (Robust against key casing)
    model_path = str(cfg.get("modelPath", cfg.get("model_path", getattr(base_args, "network", "")))).strip()
    cam_source = str(cfg.get("cameraSource", cfg.get("camera_source", getattr(base_args, "sources", ["usb:0"])[0]))).strip()
    aipu_cores = str(cfg.get("aipuCores", cfg.get("aipu_cores", getattr(base_args, "devices", "0")))).strip()
    pipe_type  = str(cfg.get("pipelineType", cfg.get("pipeline_type", getattr(base_args, "pipe", "gst")))).strip()

    # 1. SystemConfig
    sc = ax_config.SystemConfig(
        data_root=Path(base_args.data_root) if hasattr(base_args, "data_root") and base_args.data_root else None,
        build_root=Path(base_args.build_root) if hasattr(base_args, "build_root") and base_args.build_root else None,
        hardware_caps=ax_config.HardwareCaps.DEFAULT,
        allow_hardware_codec=getattr(base_args, 'enable_hardware_codec', False)
    )

    # 2. InferenceStreamConfig
    stc = ax_config.InferenceStreamConfig(
        timeout=int(getattr(base_args, 'timeout', 5)),
        frames=int(getattr(base_args, 'frames', 0))
    )

    # CORE PARTITIONING LOGIC:
    # We use AXELERA_CORE_OFFSET environment variable to pin the process to a core.
    # The device_selector identifies the Metis chip (always "0").
    device_id = "0"
    core_offset = os.environ.get("AXELERA_CORE_OFFSET", "0")
    
    # Calculate core COUNT from the string (e.g., "0,1" -> 2 cores, "2" -> 1 core)
    core_list = [c.strip() for c in aipu_cores.split(",") if c.strip()]
    core_count = len(core_list) if core_list else 1
    
    logger.info(f"Process partitioning: Metis Device {device_id}, Core Offset {core_offset}, Core Count {core_count}")

    # 3. PipelineConfig
    # Manual configuration mapping to bypass SDK argparse limitations 
    pc = ax_config.PipelineConfig(
        network=model_path,
        sources=[ax_config.Source(cam_source)],
        pipe_type=pipe_type,
        device_selector=device_id, # Always use chip index 0
        aipu_cores=core_count,     # Use actual core count requested
        rtsp_latency=int(getattr(base_args, 'rtsp_latency', 500)),
        low_latency=bool(getattr(base_args, 'low_latency', False)),
        save_output=str(getattr(base_args, 'save_output', ""))
    )

    lc = ax_config.LoggingConfig()
    dc = ax_config.DeployConfig()

    return sc, stc, pc, lc, dc

def _run_inference(cfg, args):
    global _stream, _latest_jpeg, _running, _inference_done, _stop_event, _SKIP_FRAMES
    log = logging.getLogger("Inference")
    _inference_done.clear()

    try:
        from axelera.app import config as ax_config, yaml_parser
        from axelera.app.stream import create_inference_stream
        import cv2
    except Exception as e:
        log.error(f"Axelera SDK import error: {e}")
        _inference_done.set()
        return

    # - Parse model name / path -----------------------
    model = str(cfg.get("modelPath", "")).strip()

    if not model:
        log.error("CRITICAL: No modelPath provided in configuration! Aborting launch to prevent on-the-fly compilation.")
        _inference_done.set()
        return

    if not os.path.isabs(model) and not os.path.exists(model):
        search_dirs = [
            os.path.join(_sdk_root, "ax_models"),
            os.path.join(_sdk_root, "ax_models/zoo/yolo/object_detection"),
            os.path.join(_sdk_root, "ax_models/zoo/yolo/segmentation"),
            os.path.join(_sdk_root, "ax_models/zoo/yolo/pose_estimation"),
            "/data/voyager-sdk/customers/xray",
        ]
        for d in search_dirs:
            p = os.path.join(d, f"{model}.yaml")
            if os.path.exists(p):
                model = p
                break
        else:
            log.warning(f"Model '{model}' not found in search paths - passing as-is.")

    if not model:
        model = "yolov8n-coco-onnx"

    # - Parse camera source -------------------------
    src = str(cfg.get("cameraSource", "usb:20")).strip()
    if src.isdigit():
        src = f"usb:{src}"
    elif src.startswith("/dev/video"):
        num = src.split("/dev/video")[1].split("/")[0]
        src = f"usb:{num}"

    # - Parse AIPU cores ---------------------------
    cores_raw = str(cfg.get("aipuCores", "4")).strip()
    if "," in cores_raw:
        cores = len([c for c in cores_raw.split(",") if c.strip()])
    else:
        try:
            cores = int(cores_raw)
        except Exception:
            cores = 4

    conf = float(cfg.get("confidenceThreshold", 0.4))

    # --- Load class map ---
    class_map = {}
    class_map_path = cfg.get("classMapPath", "")
    if class_map_path and os.path.exists(str(class_map_path)):
        try:
            with open(class_map_path, 'r') as f:
                raw_map = json.load(f)
            if raw_map:
                first_key, first_val = next(iter(raw_map.items()))
                if isinstance(first_val, str):
                    class_map = {str(k): v for k, v in raw_map.items()}
                elif isinstance(first_val, int) and isinstance(first_key, str):
                    class_map = {str(v): k for k, v in raw_map.items()}
            log.info(f"Loaded class map: {len(class_map)} classes from {class_map_path}")
        except Exception as e:
            log.warning(f"Class map load error: {e}")

    # --- Extract labels from model metadata (preferred by user) ---
    try:
        model_name = Path(model).stem
        build_dir = Path(_sdk_root) / "build" / model_name
        possible_json = [
            build_dir / "model_info.json",
            build_dir / model_name / "model_info.json",
        ]
        
        found_labels = False
        for pjson in possible_json:
            if pjson.exists():
                with open(pjson, 'r') as f:
                    mdata = json.load(f)
                if "labels" in mdata and mdata["labels"]:
                    class_map.update({str(i): n for i, n in enumerate(mdata["labels"])})
                    log.info(f"Loaded {len(mdata['labels'])} labels from {pjson}")
                    found_labels = True
                    break
        
        if not found_labels:
            import yaml
            if os.path.exists(model):
                with open(model, 'r') as f:
                    ydata = yaml.safe_load(f)
                if ydata and "datasets" in ydata:
                    for dname, dinfo in ydata["datasets"].items():
                        names_found = None
                        if "names" in dinfo:
                            names_found = dinfo["names"]
                        elif "ultralytics_data_yaml" in dinfo:
                            ddir = dinfo.get("data_dir_name", ".").replace("$AXELERA_FRAMEWORK", _sdk_root)
                            if not ddir.startswith("/"):
                                ddir = os.path.join(_sdk_root, "ax_models", ddir)
                            dyaml = os.path.join(ddir, dinfo["ultralytics_data_yaml"])
                            if os.path.exists(dyaml):
                                with open(dyaml, 'r') as fy:
                                    ydata_inner = yaml.safe_load(fy)
                                names_found = ydata_inner.get("names")
                        
                        if names_found:
                            if isinstance(names_found, list):
                                class_map.update({str(i): n for i, n in enumerate(names_found)})
                            elif isinstance(names_found, dict):
                                class_map.update({str(k): v for k, v in names_found.items()})
                            log.info(f"Updated class map from YAML dataset '{dname}'")
                            break
    except Exception as ey:
        log.warning(f"Could not extract labels from model metadata: {ey}")

    # --- EdgeX context ---
    edgex_device   = cfg.get("edgexDeviceName", "")
    edgex_profile  = cfg.get("edgexProfileName", "")
    senml_template = cfg.get("edgexPayloadTemplate", None)

    log.info(f"START: model={model} source={src}")

    is_file = not src.startswith("usb:") and not src.startswith("rtsp:") and os.path.exists(src)

    while not _stop_event.is_set():
        _stream = None
        try:
            # - Build SDK configs safely -------------------
            model = cfg.get("modelPath", "")
            if model.endswith(".yaml") and os.path.exists(model):
                # YAML-AWARE RESOLVER: We parse the YAML to see exactly what weights it needs.
                # If weights are relative and missing, we link them from the global cache.
                try:
                    m_dir = os.path.dirname(model)
                    with open(model, 'r') as f:
                        ydata = yaml.safe_load(f)
                    
                    models_info = ydata.get("models", {})
                    for m_name, m_cfg in models_info.items():
                        w_path = m_cfg.get("weight_path")
                        if w_path and not os.path.isabs(w_path):
                            # Target location the SDK expects
                            target = os.path.abspath(os.path.join(m_dir, w_path))
                            if not os.path.exists(target):
                                # Search in global /data cache
                                fname = os.path.basename(w_path)
                                # Possible cache locations
                                cache_opts = [
                                    f"/data/os_data_move/home/aetina/.cache/axelera/weights/{m_name}/{fname}",
                                    f"/data/voyager-sdk/weights/{fname}"
                                ]
                                for opt in cache_opts:
                                    if os.path.exists(opt):
                                        os.makedirs(os.path.dirname(target), exist_ok=True)
                                        os.symlink(opt, target)
                                        log.info(f"Resolved weight: {fname} -> {opt}")
                                        break

                    # Also ensure 'build' link exists for binaries
                    b_link = os.path.join(m_dir, "build")
                    if not os.path.exists(b_link):
                        os.symlink("/data/voyager-sdk/build", b_link)
                        log.info(f"Auto-linked build cache to {m_dir}")

                except Exception as e:
                    log.warning(f"YAML Resolver failed: {e}")

            sc, stc, pc, lc, dc = _get_inference_configs(cfg, args)
            pc.network = model
            _stream = create_inference_stream(sc, stc, pc, lc, dc)
            
            log.info("Stream online.")

            # - Per-frame loop --------------------------
            frame_idx = 0
            for res in _stream:
                if _stop_event.is_set():
                    break
                
                frame_idx += 1
                if frame_idx % _SKIP_FRAMES != 0:
                    continue

                # - Encode JPEG frame ----------------------
                try:
                    if _video_enabled:
                        img      = res.image.asarray("BGR")
                        ret, buf = cv2.imencode('.jpg', img, [cv2.IMWRITE_JPEG_QUALITY, 50])
                        if ret:
                            with _latest_jpeg_lock:
                                _latest_jpeg = buf.tobytes()
                    else:
                        with _latest_jpeg_lock:
                            _latest_jpeg = None
                except Exception as e:
                    import traceback
                    log.error(f"Inference error: {e}\n{traceback.format_exc()}")
                    break

                # - Run detections ------------------------
                dets  = []
                m_map = {}
                if hasattr(res, 'meta') and hasattr(res.meta, '_meta_map'):
                    m_map = res.meta._meta_map
                    if isinstance(m_map, str):
                        try: m_map = json.loads(m_map)
                        except Exception: m_map = {}

                # Ensure m_map is a dict
                if not isinstance(m_map, dict):
                    m_map = {}

                base = m_map.get("detections") or m_map.get("objects")
                if base:
                    h, w    = res.image.height, res.image.width
                    indices = range(len(base)) if hasattr(base, '__len__') else []

                    for i in indices:
                        try:
                            obj         = base.Object(base, i) if hasattr(base, 'Object') else base[i]
                            score       = float(obj.score)
                            if score < conf:
                                continue

                            box         = obj.box
                            label       = "Object"
                            final_score = score

                            try:
                                detections_obj = m_map.get("detections")
                                sec_list       = getattr(
                                    detections_obj, "_secondary_metas", {}
                                ).get("recognitions")

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
                                            prev = base.Object(base, j) if hasattr(base, 'Object') else base[j]
                                            if int(prev.class_id) == 0:
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
                                            log.info(f"Face recognised: {label} (id:{face_id})")
                                        if hasattr(recog, "_scores") and recog._scores:
                                            final_score = float(recog._scores[0][0])
                                else:
                                    cls_id_num = int(obj.class_id) if hasattr(obj, 'class_id') else -1
                                    label      = class_map.get(str(cls_id_num), "Person" if cls_id_num == 0 else "Object")

                            except Exception as sec_e:
                                log.warning(f"Secondary metadata error at index {i}: {sec_e}")
                                label = "Object"

                            dets.append({
                                "label": label,
                                "score": final_score,
                                "box_norm": [
                                    float(box[0]) / w,
                                    float(box[1]) / h,
                                    float(box[2] - box[0]) / w,
                                    float(box[3] - box[1]) / h,
                                ],
                            })
                        except Exception:
                            continue

                    # - Build raw_data and send to EdgeX -------------
                    if dets and edgex_device:
                        labels = [d["label"] for d in dets]
                        log.info(f"Detections: {len(dets)} - {', '.join(labels)}")

                        now_ts         = time.time()
                        unique_current = set(labels)
                        should_send    = False

                        with _edgex_lock:
                            for lbl in unique_current:
                                if now_ts - _last_sent_to_edgex.get(lbl, 0.0) > 5.0:
                                    should_send              = True
                                    _last_sent_to_edgex[lbl] = now_ts

                            if should_send:
                                all_scores  = [d["score"] for d in dets]
                                avg_conf    = sum(all_scores) / len(all_scores) if all_scores else 0.0
                                bboxes      = [d["box_norm"] for d in dets]
                                animal_labels = {
                                    "cat","dog","bird","horse","sheep","cow",
                                    "elephant","bear","zebra","giraffe",
                                }
                                person_dets = [d for d in dets if d["label"].lower() in ("person","object")]
                                animal_dets = [d for d in dets if d["label"].lower() in animal_labels]
                                known_dets  = [d for d in dets if d["label"] not in ("Person","Object","object")]

                                inference_output = {
                                    "device_name":      str(edgex_device),
                                    "face_count":       len(dets),
                                    "person_count":     len(person_dets),
                                    "animal_count":     len(animal_dets),
                                    "total_count":      len(dets),
                                    "known_count":      len(known_dets),
                                    "confidence":       round(avg_conf * 100.0, 2),
                                    "recognized_names": ",".join(d["label"] for d in known_dets) or "unknown",
                                    "all_labels":       ",".join(d["label"] for d in dets),
                                    "bbox_coordinates": json.dumps(bboxes),
                                    "embeddings":       "[]",
                                    "frame_timestamp":  datetime.datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S UTC"),
                                    "status":           "active",
                                    "cam_temp":         41.5,
                                }

                                send_edgex_payload(
                                    device_name      = str(edgex_device),
                                    profile_name     = str(edgex_profile or "DefaultProfile"),
                                    inference_output = inference_output,
                                    template         = senml_template,
                                )

                broadcast_telemetry({
                    "type":         "detections",
                    "detections":   dets,
                    "edgexDevice":  edgex_device,
                    "edgexProfile": edgex_profile,
                })

            if not is_file or _stop_event.is_set():
                break
            log.info("Looping video file...")
            _stream.stop()

        except Exception as e:
            import traceback
            log.error(f"STREAM CRASH: {e}\n{traceback.format_exc()}")
        finally:
            if _stream is not None:
                try:
                    log.info("Closing SDK stream...")
                    _stream.stop()
                except Exception:
                    pass
            _stream = None

    _inference_done.set()
    log.info("Inference thread done.")


# ==============================================================================
# COMMAND SERVER
# ==============================================================================


def run_command_server(port, fallback_cfg, args):
    global _running, _stream, _stop_event, _inference_done
    srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("0.0.0.0", port))
    srv.listen(5)
    srv.settimeout(1.0)
    logger.info(f"Command server on port {port}")

    while _running:
        try:
            conn, addr = srv.accept()
            # Read until newline
            buffer = b""
            while True:
                chunk = conn.recv(4096)
                if not chunk:
                    break
                buffer += chunk
                if b"\n" in chunk:
                    break
            
            # Decode and take only the first complete line
            decoded = buffer.decode("utf-8")
            lines = [l.strip() for l in decoded.split("\n") if l.strip()]
            if not lines:
                conn.close()
                continue
                
            raw = lines[0]

            cmd  = raw.split(":", 1)[0]
            body = raw.split(":", 1)[1] if ":" in raw else ""

            if cmd == "start":
                if _stream is not None:
                    conn.sendall(b"ALREADY_RUNNING\n")
                else:
                    try:
                        cfg = json.loads(body) if body else fallback_cfg
                        logger.info(f"RECEIVED CONFIG: {json.dumps(cfg)}")
                    except Exception as e:
                        logger.error(f"Config parse error: {e}")
                        conn.sendall(b"ERROR:PARSE_FAILED\n")
                        conn.close()
                        continue
                    _stop_event.clear()
                    threading.Thread(target=_run_inference, args=(cfg, args), daemon=True).start()
                    conn.sendall(b"OK\n")

            elif cmd == "stop":
                _stop_event.set()
                if _stream:
                    try: _stream.stop()
                    except Exception: pass
                _inference_done.wait(timeout=5.0)
                conn.sendall(b"STOPPED\n")
            
            elif cmd == "toggle_video":
                _video_enabled = (body.lower() == "on")
                logger.info(f"Video encoding: {'ENABLED' if _video_enabled else 'DISABLED'}")
                conn.sendall(b"OK\n")

            elif cmd == "status":
                info = {
                    "running": _stream is not None,
                    "ports": {
                        "video": PORT_VIDEO,
                        "meta": PORT_METADATA,
                        "cmd": PORT_COMMAND
                    }
                }
                conn.sendall((json.dumps(info) + "\n").encode())

            conn.close()
        except socket.timeout:
            continue

    srv.close()


# ==============================================================================
# MAIN
# ==============================================================================

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--config",     default=DEFAULT_CONFIG)
    parser.add_argument("--cmd-port",   type=int, default=5567)
    parser.add_argument("--video-port", type=int, default=5568)
    parser.add_argument("--meta-port",  type=int, default=5566)
    parser.add_argument("--skip-frames", type=int, default=1)
    parser.add_argument("--log-file",    type=str, default=None)
    parser.add_argument("--no-display",  action="store_true", help="Disable local visualization window")
    
    # Capture SDK args for from_parsed_args fallback if needed
    known_args, sdk_args_raw = parser.parse_known_args()
    
    # Initialize logger
    logger = setup_logging(known_args.log_file)
    
    # Initialize globals with CLI args
    PORT_COMMAND  = known_args.cmd_port
    PORT_VIDEO    = known_args.video_port
    PORT_METADATA = known_args.meta_port
    _SKIP_FRAMES  = known_args.skip_frames

    if known_args.no_display:
        os.environ["AXELERA_DISABLE_DISPLAY"] = "1"
        logger.info("Local display DISABLED")

    # Re-parse to get the full args namespace that SDK helpers expect
    args = parser.parse_args()

    try:
        with open(args.config, 'r') as f:
            cfg = json.load(f)["streams"][0]
    except Exception:
        cfg = {}

    threading.Thread(target=run_video_server,     args=(PORT_VIDEO,),        daemon=True).start()
    threading.Thread(target=run_telemetry_server,  args=(PORT_METADATA,),     daemon=True).start()
    threading.Thread(target=run_command_server,    args=(PORT_COMMAND, cfg, args), daemon=True).start()

    logger.info(f"Axelera Server Online. Ports: CMD={PORT_COMMAND}, VIDEO={PORT_VIDEO}, META={PORT_METADATA}")
    try:
        while _running:
            time.sleep(1)
    except KeyboardInterrupt:
        logger.info("Shutting down...")
        _stop_event.set()
        if _stream: _stream.stop()
        _running = False
        time.sleep(1)