import urllib.request
import urllib.parse
import json
import base64
import struct
import subprocess
import os
from http.server import BaseHTTPRequestHandler, HTTPServer

KERNEL_ORCH_URL  = "http://127.0.0.1:8108/api/orchestrate?query="
KERNEL_LEARN_URL = "http://127.0.0.1:8108/api/learn?"

class BridgeHandler(BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        return
    def do_OPTIONS(self):
        self.send_response(200, "ok")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

    def do_POST(self):
        if self.path == "/api/drishti":
            try:
                length = int(self.headers.get("Content-Length", 0))
                body = json.loads(self.rfile.read(length).decode("utf-8"))
                w = int(body["width"])
                h = int(body["height"])
                mode = int(body.get("mode", 2))
                rgb_bytes = base64.b64decode(body["rgb_b64"])
                mask_bytes = base64.b64decode(body["mask_b64"])

                with open("drishti_in.bin", "wb") as f:
                    f.write(struct.pack("<iii", w, h, mode))
                    f.write(rgb_bytes)
                    f.write(mask_bytes)

                proc = subprocess.run(["./drishti-engine", "drishti_in.bin", "drishti_out.bin"], capture_output=True, text=True, timeout=15)
                telemetry = proc.stdout.strip() or "✅ Native C-Engine Complete!"

                with open("drishti_out.bin", "rb") as f:
                    out_rgb = f.read(w * h * 3)
                    out_mask = f.read(w * h)

                resp = {
                    "status": "ok",
                    "telemetry": telemetry,
                    "rgb_b64": base64.b64encode(out_rgb).decode("ascii"),
                    "mask_b64": base64.b64encode(out_mask).decode("ascii")
                }
                self.send_response(200)
                self.send_header("Content-type", "application/json")
                self.send_header("Access-Control-Allow-Origin", "*")
                self.end_headers()
                self.wfile.write(json.dumps(resp).encode("utf-8"))
            except Exception as e:
                self.send_response(500)
                self.send_header("Access-Control-Allow-Origin", "*")
                self.end_headers()
                self.wfile.write(json.dumps({"error": str(e)}).encode("utf-8"))
            return

    def do_GET(self):
        if self.path == "/" or self.path.startswith("/index.html"):
            with open("index.html", "rb") as f:
                content = f.read()
            self.send_response(200)
            self.send_header("Content-type", "text/html; charset=utf-8")
            self.end_headers()
            self.wfile.write(content)
            return

        if self.path.startswith("/api/chat?query="):
            query = urllib.parse.unquote(self.path.split("?query=")[1]).strip()
            if query.lower().startswith("teach:") and "|" in query:
                parts = query[6:].split("|", 1)
                k, a = parts[0].strip(), parts[1].strip()
                params = urllib.parse.urlencode({"key": k, "ans": a})
                lr = json.loads(urllib.request.urlopen(KERNEL_LEARN_URL + params, timeout=3).read().decode("utf-8"))
                out_data = {
                    "status": "success", "task": "GURUKUL", "hash": 999, "spike": 0, "overlap": 40,
                    "llm_response": f"🧬 Gurukul Sutra Locked in Smriti-Kosh! (Total Sutras: {lr.get('total_sutras')})"
                }
                self.send_response(200)
                self.send_header("Content-type", "application/json")
                self.send_header("Access-Control-Allow-Origin", "*")
                self.end_headers()
                self.wfile.write(json.dumps(out_data).encode("utf-8"))
                return

            safe_q = urllib.parse.quote(query)
            try:
                req = urllib.request.urlopen(KERNEL_ORCH_URL + safe_q, timeout=3)
                k_res = json.loads(req.read().decode("utf-8"))
                if k_res.get("cached_hit") == 1 and k_res.get("smriti_reply"):
                    k_res["llm_response"] = k_res["smriti_reply"]
                else:
                    k_res["llm_response"] = ""
                self.send_response(200)
                self.send_header("Content-type", "application/json")
                self.send_header("Access-Control-Allow-Origin", "*")
                self.end_headers()
                self.wfile.write(json.dumps(k_res).encode("utf-8"))
            except Exception as e:
                self.send_response(500)
                self.end_headers()
                self.wfile.write(json.dumps({"error": str(e)}).encode("utf-8"))

HTTPServer(("127.0.0.1", 8110), BridgeHandler).serve_forever()
