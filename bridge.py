
import urllib.request

import urllib.parse

import json

import threading

import time

import os

from http.server import BaseHTTPRequestHandler, HTTPServer



llm = None

if os.path.exists("./model.gguf"):

    try:

        from llama_cpp import Llama

        llm = Llama(model_path="./model.gguf", n_ctx=2048, verbose=False)

    except Exception:

        pass



KERNEL_ORCH_URL  = "http://127.0.0.1:8108/api/orchestrate?query="

KERNEL_LEARN_URL = "http://127.0.0.1:8108/api/learn?"

KERNEL_STATE_URL = "http://127.0.0.1:8108/api/state"



class BridgeHandler(BaseHTTPRequestHandler):

    def log_message(self, format, *args):

        return

    def do_OPTIONS(self):

        self.send_response(200, "ok")

        self.send_header("Access-Control-Allow-Origin", "*")

        self.send_header("Access-Control-Allow-Methods", "GET, OPTIONS")

        self.end_headers()



    def do_GET(self):

        # Serve index.html directly when opening http://127.0.0.1:8110 in browser!

        if self.path == "/" or self.path.startswith("/index.html"):

            try:

                with open("index.html", "rb") as f:

                    content = f.read()

                self.send_response(200)

                self.send_header("Content-type", "text/html; charset=utf-8")

                self.end_headers()

                self.wfile.write(content)

                return

            except Exception as e:

                self.send_response(404)

                self.end_headers()

                return



        if self.path.startswith("/api/chat?query="):

            query = urllib.parse.unquote(self.path.split("?query=")[1]).strip()



            # Direct Gurukul Teach Command from Chat: "teach: question | answer"

            if query.lower().startswith("teach:") and "|" in query:

                parts = query[6:].split("|", 1)

                k, a = parts[0].strip(), parts[1].strip()

                params = urllib.parse.urlencode({"key": k, "ans": a})

                lr = json.loads(urllib.request.urlopen(KERNEL_LEARN_URL + params, timeout=3).read().decode("utf-8"))

                out_data = {

                    "status": "success", "task": "GURUKUL", "hash": 999, "spike": 0, "overlap": 40,

                    "llm_response": f"🧬 Gurukul Sutra Locked in Smriti-Kosh! (Total Sutras: {lr.get('total_sutras')})",

                    "engine": "⚡ Gurukul Persistent ROM"

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

                    k_res["engine"] = "⚡ C-Kernel Smriti-Kosh (0.002ms • Zero Heat)"

                else:

                    task = k_res.get("task", "GENERAL")

                    if task == "LEDGER":

                        k_res["llm_response"] = f"✅ Zyvera Ledger Action Extracted for: '{query}'"

                        k_res["engine"] = "⚡ Zyvera Deterministic Core"

                    elif task == "CODE" and llm is not None:

                        prompt = f"<|im_start|>user\n{query}<|im_end|>\n<|im_start|>assistant\n"

                        out = llm(prompt, max_tokens=250, stop=["<|im_end|>"], echo=False)

                        k_res["llm_response"] = out["choices"][0]["text"].strip()

                        k_res["engine"] = "🧠 Local Qwen-0.5B GGUF"

                    elif task == "VISION":

                        k_res["llm_response"] = f"🎨 Vision Intent Routed for: '{query}'"

                        k_res["engine"] = "🎨 Drishti Vision Router"

                    else:

                        k_res["llm_response"] = ""

                        k_res["engine"] = "🔥 Spanda Novelty Spike (Auto-Fetching Knowledge...)"



                self.send_response(200)

                self.send_header("Content-type", "application/json")

                self.send_header("Access-Control-Allow-Origin", "*")

                self.end_headers()

                self.wfile.write(json.dumps(k_res).encode("utf-8"))

            except Exception as e:

                self.send_response(500)

                self.send_header("Access-Control-Allow-Origin", "*")

                self.end_headers()

                self.wfile.write(json.dumps({"error": str(e)}).encode("utf-8"))

        else:

            self.send_response(404)

            self.end_headers()



HTTPServer(("127.0.0.1", 8110), BridgeHandler).serve_forever()

