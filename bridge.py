import urllib.request
import urllib.parse
import json
import sys
import threading
import time
import random
import os
from http.server import BaseHTTPRequestHandler, HTTPServer

# Android Patch
if sys.platform == "android":
    sys.platform = "linux"

print("=====================================================")
print(" ⏳ INITIATING LOCAL OFFLINE AI ENGINE (Llama.cpp)...")
try:
    from llama_cpp import Llama
    # LLM load ho raha hai (Only for CODE/TEXT tasks)
    llm = Llama(model_path="./model.gguf", n_ctx=2048, verbose=False)
    print("✅ ENGINE LOADED SUCCESSFULLY!\n")
except Exception as e:
    print(f"❌ ERROR: {e}")
    sys.exit(1)

KERNEL_URL = "http://127.0.0.1:8108/api/orchestrate?query="
KERNEL_STATE_URL = "http://127.0.0.1:8108/api/state"
KERNEL_MANIFEST_URL = "http://127.0.0.1:8108/api/manifest?query="

# =========================================================
# 🐜 DIGITAL CHEENTI (Autonomous Heartbeat)
# =========================================================
def autonomous_heartbeat():
    time.sleep(5) # Server start hone ka wait karo
    print("\n[🐜] Digital Cheenti (Autonomous Heartbeat) Zinda ho gayi hai...")
    
    while True:
        try:
            # OS ka current mood (Guna) check karo
            req = urllib.request.urlopen(KERNEL_STATE_URL, timeout=2)
            state_data = json.loads(req.read().decode('utf-8'))
            guna = state_data.get("guna", 1)
            
            if guna == 0:
                # TAMAS (Bored/Sleep) - OS ko jagane ke liye data do
                print("\n[🐜] OS is BORED (Tamas). Khud naya data dhoond raha hai...")
                thoughts = [
                    "Quantum physics explores subatomic particles.",
                    "The universe is constantly expanding.",
                    "Water consists of two hydrogen atoms and one oxygen atom.",
                    "Photosynthesis is how plants make food."
                ]
                new_data = random.choice(thoughts)
                encoded_data = urllib.parse.quote(new_data)
                
                # OS ko feed karo
                urllib.request.urlopen(KERNEL_MANIFEST_URL + encoded_data)
                print(f"     -> OS ne autonomously yaad kiya: '{new_data}'")
                
            elif guna == 2:
                # RAJAS (Chaos/Panic)
                print("\n[🚨] WARNING: OS PANIC STATE MEIN HAI (Rajas)!")
                print("     -> High Entropy Detected! Executing Defense Protocols...")
                
        except Exception:
            # Agar C-Kernel band hai, toh crash mat ho, shanti se wait karo
            pass
            
        time.sleep(15) # Har 15 second mein check karega taaki battery bache

# =========================================================
# 🧠 THE CORTEX API (HTTP Server)
# =========================================================
class BridgeHandler(BaseHTTPRequestHandler):
    # Handle CORS for browser UI
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
                
                # 2. Decide action based on CEO's Task Classification
                if task_type == "CODE":
                    print(f"\n⚡ GENERATING CODE FOR: {query}")
                    prompt = f"<|im_start|>user\n{query}<|im_end|>\n<|im_start|>assistant\n"
                    output = llm(prompt, max_tokens=300, stop=["<|im_end|>"], echo=False)
                    kernel_reply["llm_response"] = output['choices'][0]['text'].strip()
                
                elif task_type == "VISION":
                    # ZERO-RAM Cloud Image Generation to prevent Termux Crash
                    print(f"\n🎨 GENERATING IMAGE FOR: {query} (Zero-RAM Cloud Mode)")
                    image_url = f"https://image.pollinations.ai/prompt/{safe_prompt}"
                    try:
                        urllib.request.urlretrieve(image_url, "generated_image.jpg")
                        kernel_reply["llm_response"] = "Image successfully saved as 'generated_image.jpg' in Termux folder."
                        print("✅ Image generated and saved!")
                    except Exception as e:
                        kernel_reply["llm_response"] = f"Image download failed: {e}"
                        
                else:
                    # GENERAL Task (No heavy processing needed)
                    kernel_reply["llm_response"] = "General observation recorded in Holographic Memory."

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

# Start Background Thread
cheenti_thread = threading.Thread(target=autonomous_heartbeat, daemon=True)
cheenti_thread.start()

# Start Bridge Server
server = HTTPServer(('127.0.0.1', 8110), BridgeHandler)
print(" 🧠 BRIDGE API ACTIVE ON PORT 8110 🧠 ")
server.serve_forever()

