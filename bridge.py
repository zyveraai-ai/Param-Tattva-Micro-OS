
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



class ReusableHTTPServer(HTTPServer):

    allow_reuse_address = True



# 🏹 EKLAVYA TEACHER DISTILLATION ENGINE:

# Reverse-engineers responses from large Teacher AI models (Pollinations OpenAI/Mistral or Custom API)

# and compiles them into compact Sutras inside our local C-Kernel smriti.bin!

def eklavya_distill_from_teacher(query):

    try:

        sys_prompt = (

            "You are a Teacher AI being distilled into the Param-Tattva Micro-OS C-Kernel. "

            "Give a direct, high-density factual or technical answer in 2 concise sentences (under 280 chars) "

            "in clear Hinglish or English so the Edge C-Kernel can lock it as a permanent Sutra."

        )

        full_p = urllib.parse.quote(f"{sys_prompt}\nUser Query: {query}")

        req = urllib.request.Request(

            f"https://text.pollinations.ai/{full_p}",

            headers={"User-Agent": "ParamTattva-Eklavya-Distiller/9.0"}

        )

        with urllib.request.urlopen(req, timeout=8) as resp:

            ans = resp.read().decode("utf-8").strip()

            if len(ans) > 10:

                clean_ans = ans.replace('"', "'").replace("\n", " ")[:340]

                params = urllib.parse.urlencode({"key": query, "ans": clean_ans})

                urllib.request.urlopen(KERNEL_LEARN_URL + params, timeout=3)

                return clean_ans

    except Exception:

        pass

    return None



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



                proc = subprocess.run(["./drishti-engine", "drishti_in.bin", "drishti_out.bin"], capture_output=True, text=True, timeout=30)

                telemetry = proc.stdout.strip() or "✅ Native C-Engine v9.0 Complete!"



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

                self.send_header("Cache-Control", "no-store, no-cache, must-revalidate")

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

        if self.path == "/" or self.path.startswith("/index.html") or self.path.startswith("/?"):

            with open("index.html", "rb") as f:

                content = f.read()

            self.send_response(200)

            self.send_header("Content-type", "text/html; charset=utf-8")

            self.send_header("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0")

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

                    k_res["distilled"] = 0

                else:

                    # SPIKE = 1 -> Trigger Eklavya Teacher Reverse-Engineering!

                    distilled_ans = eklavya_distill_from_teacher(query)

                    if distilled_ans:

                        k_res["llm_response"] = distilled_ans

                        k_res["distilled"] = 1

                    else:

                        k_res["llm_response"] = ""

                        k_res["distilled"] = 0

                self.send_response(200)

                self.send_header("Content-type", "application/json")

                self.send_header("Access-Control-Allow-Origin", "*")

                self.end_headers()

                self.wfile.write(json.dumps(k_res).encode("utf-8"))

            except Exception as e:

                self.send_response(500)

                self.end_headers()

                self.wfile.write(json.dumps({"error": str(e)}).encode("utf-8"))



print("🏹 Bridge v9.0 (Eklavya Distiller + Deep Drishti C-Engine) Live on 8110!")

ReusableHTTPServer(("127.0.0.1", 8110), BridgeHandler).serve_forever()

