import urllib.request
import urllib.parse
import json
import sys
from http.server import BaseHTTPRequestHandler, HTTPServer

# Android Patch
if sys.platform == "android":
    sys.platform = "linux"

print("=====================================================")
print(" ⏳ INITIATING LOCAL OFFLINE AI ENGINE (Llama.cpp)...")
try:
    from llama_cpp import Llama
    llm = Llama(model_path="./model.gguf", n_ctx=2048, verbose=False)
    print("✅ ENGINE LOADED SUCCESSFULLY!\n")
except Exception as e:
    print(f"❌ ERROR: {e}")
    sys.exit(1)

KERNEL_URL = "http://127.0.0.1:8108/api/orchestrate?query="

class BridgeHandler(BaseHTTPRequestHandler):
    # Handle CORS for browser
    def do_OPTIONS(self):
        self.send_response(200, "ok")
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, OPTIONS')
        self.end_headers()
        
    def do_GET(self):
        if self.path.startswith("/api/chat?query="):
            query = urllib.parse.unquote(self.path.split("?query=")[1])
            safe_prompt = urllib.parse.quote(query)
            
            try:
                # 1. Ask CEO (C-Kernel)
                req = urllib.request.urlopen(KERNEL_URL + safe_prompt)
                kernel_reply = json.loads(req.read().decode('utf-8'))
                task_type = kernel_reply.get("task", "UNKNOWN")
                
                # 2. If CODE, generate from LLM
                if task_type == "CODE":
                    print(f"\n⚡ GENERATING CODE FOR: {query}")
                    prompt = f"<|im_start|>user\n{query}<|im_end|>\n<|im_start|>assistant\n"
                    output = llm(prompt, max_tokens=300, stop=["<|im_end|>"], echo=False)
                    kernel_reply["llm_response"] = output['choices'][0]['text'].strip()
                
                # 3. Send final data back to UI
                self.send_response(200)
                self.send_header('Content-type', 'application/json')
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()
                self.wfile.write(json.dumps(kernel_reply).encode('utf-8'))
                
            except Exception as e:
                self.send_response(500)
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()
                self.wfile.write(json.dumps({"error": str(e)}).encode('utf-8'))
        else:
            self.send_response(404)
            self.end_headers()

# Start Bridge Server
server = HTTPServer(('127.0.0.1', 8110), BridgeHandler)
print(" 🧠 BRIDGE API ACTIVE ON PORT 8110 🧠 ")
server.serve_forever()

