import http.server
import urllib.request
import logging
import json
import sys

# Configure logging to stdout so it's captured by start-edgex.sh
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(message)s',
    handlers=[logging.StreamHandler(sys.stdout)]
)

class ProxyHandler(http.server.BaseHTTPRequestHandler):
    def do_POST(self):
        content_length = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(content_length)
        
        # Mapping: /core-data/api/v3/... -> http://localhost:59880/api/v3/...
        target_path = self.path
        if target_path.startswith("/core-data"):
            target_path = target_path[10:]
            
        target_url = f"http://localhost:59880{target_path}"
        logging.info(f"Forwarding POST {self.path} -> {target_url}")
        
        req = urllib.request.Request(target_url, data=body, method="POST")
        for key, value in self.headers.items():
            if key.lower() not in ["host", "content-length"]:
                req.add_header(key, value)
        
        try:
            with urllib.request.urlopen(req) as response:
                self.send_response(response.status)
                for key, value in response.getheaders():
                    self.send_header(key, value)
                self.end_headers()
                self.wfile.write(response.read())
        except Exception as e:
            logging.error(f"Proxy error: {e}")
            self.send_response(500)
            self.end_headers()
            self.wfile.write(str(e).encode())

    def log_message(self, format, *args):
        # Suppress default server logging to avoid double logs
        return

if __name__ == "__main__":
    PORT = 4000
    logging.info(f"Starting EdgeX Ingestion Proxy on port {PORT} (Target: 59880)")
    httpd = http.server.HTTPServer(("0.0.0.0", PORT), ProxyHandler)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        logging.info("Shutting down proxy...")
        httpd.server_close()
