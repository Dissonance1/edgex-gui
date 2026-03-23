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
from pathlib import Path

# --- Constants & Globals ---
DEFAULT_CONFIG = "config_multi_camera.json"
TELEMETRY_PORT = 5566
COMMAND_PORT   = 5567
VIDEO_PORT     = 5568

_running            = True
_latest_jpeg        = None
_latest_jpeg_lock   = threading.Lock()
_gui_clients        = []
_gui_clients_lock   = threading.Lock()
_stream             = None          # The actual SDK stream object
_stream_lock        = threading.Lock()
_stop_event         = threading.Event()   # Signals the inference loop to stop
_inference_done     = threading.Event()   # Set when thread fully exits

# Configure logging
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - [%(levelname)s] - %(message)s'
)
logger = logging.getLogger("MASTER")

# SDK Root
_sdk_root = os.environ.get("AXELERA_FRAMEWORK", "/data/voyager-sdk")
if _sdk_root not in sys.path:
    sys.path.append(_sdk_root)


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

    # - Parse model name / path -----------------------
    model = str(cfg.get("modelPath", "yolov8n-coco-onnx")).strip()

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
                    mdata = yaml.safe_load(f)
                if mdata and "datasets" in mdata:
                    for dname, dinfo in mdata["datasets"].items():
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
                                    ydata = yaml.safe_load(fy)
                                names_found = ydata.get("names")
                        
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

    log.info(
        f"START: model={model} source={src} cores={cores} "
        f"conf={conf} edgex={edgex_device}/{edgex_profile}"
    )

    # - Build SDK args ----------------------------
    original_argv = sys.argv[:]
    sys.argv      = [sys.argv[0]]
    parser        = ax_config.create_inference_argparser(yaml_parser.get_network_yaml_info())

    pipe    = str(cfg.get("pipelineType", "gst")).strip().lower()
    metis   = "m2"  # Hardcoded as requested
    display_raw = str(cfg.get("displayMode", "none")).strip().lower()

    display = "none"
    if "window" in display_raw:
        display = "window"
    elif "headless" in display_raw:
        display = "none"
    else:
        display = display_raw

    args_list = [
        model, src,
        "--display", display,
        "--pipe", pipe,
        "--metis", metis,
        "--aipu-cores", str(cores)
    ]
    log.info(f"SDK Args: {args_list}")
    try:
        args = parser.parse_args(args_list)
    except SystemExit:
        log.error("SDK Parser help:")
        parser.print_help()
        raise
    sys.argv = original_argv

    args.data_root  = Path(_sdk_root) / "ax_models"
    args.build_root = Path(_sdk_root) / "build"

    is_file = not src.startswith("usb:") and not src.startswith("rtsp:") and os.path.exists(src)

    while not _stop_event.is_set():
        sdk_stream = None
        try:
            sc  = ax_config.SystemConfig.from_parsed_args(args)
            stc = ax_config.InferenceStreamConfig.from_parsed_args(args)
            pc  = ax_config.PipelineConfig.from_parsed_args(args)

            if edgex_profile:
                with _profile_lock:
                    _profile_cache = {}
                async_fetch_profile(edgex_profile)

            lc  = ax_config.LoggingConfig.from_parsed_args(args)
            dc  = ax_config.DeployConfig.from_parsed_args(args)

            sdk_stream = create_inference_stream(sc, stc, pc, lc, dc)
            with _stream_lock:
                if _stop_event.is_set():
                    sdk_stream.stop()
                    break
                _stream = sdk_stream
            log.info("Stream online.")

            # - Per-frame loop --------------------------
            for res in sdk_stream:
                if _stop_event.is_set():
                    break

                # - Encode JPEG frame ----------------------
                try:
                    img      = res.image.asarray("BGR")
                    ret, buf = cv2.imencode('.jpg', img, [cv2.IMWRITE_JPEG_QUALITY, 50])
                    if ret:
                        with _latest_jpeg_lock:
                            _latest_jpeg = buf.tobytes()
                except Exception as e:
                    log.debug(f"Frame encode error: {e}")

                # - Run detections ------------------------
                dets  = []
                m_map = (
                    res.meta._meta_map
                    if hasattr(res, 'meta') and hasattr(res.meta, '_meta_map')
                    else {}
                )
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
            sdk_stream.stop()

        except Exception as e:
            log.error(f"Inference error: {e}")
            break
        finally:
            if sdk_stream is not None:
                try:
                    log.info("Closing SDK stream...")
                    sdk_stream.stop()
                except Exception:
                    pass
            with _stream_lock:
                _stream = None

    _inference_done.set()
    log.info("Inference thread done.")


# ==============================================================================
# COMMAND SERVER
# ==============================================================================

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
                threading.Thread(
                    target=_run_inference, args=(cfg,), daemon=True
                ).start()
                conn.sendall(b"OK\n")

            elif cmd == "stop":
                with _stream_lock:
                    existing = _stream
                if existing is not None:
                    logger.info("Stop requested - releasing AIPU cores...")
                    _stop_event.set()
                    with _profile_lock:
                        _profile_cache = {}
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

    srv.close()


# ==============================================================================
# MAIN
# ==============================================================================

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--config", default=DEFAULT_CONFIG)
    args = parser.parse_args()

    try:
        with open(args.config, 'r') as f:
            cfg = json.load(f)["streams"][0]
    except Exception:
        cfg = {}

    threading.Thread(target=run_video_server,     args=(VIDEO_PORT,),        daemon=True).start()
    threading.Thread(target=run_telemetry_server,  args=(TELEMETRY_PORT,),   daemon=True).start()
    threading.Thread(target=run_command_server,    args=(COMMAND_PORT, cfg), daemon=True).start()

    logger.info("Axelera Server Online.")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        logger.info("Shutting down...")
        _stop_event.set()
        _running = False
        time.sleep(1)