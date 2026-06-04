import urllib.request
import urllib.parse
import json
import sys
import threading
import time
import random
import os
from http.server import BaseHTTPRequestHandler, HTTPServer

# Android Patch for better compatibility within Termux environments
if sys.platform == "android":
    sys.platform = "linux"

print("=====================================================")
print(" ⏳ INITIATING LOCAL OFFLINE AI ENGINE (Llama.cpp)...")
try:
    from llama_cpp import Llama
    # LLM initialization for complex logical tasks like code generation or text synthesis
    llm = Llama(model_path="./model.gguf", n_ctx=2048, verbose=False)
    print("✅ ENGINE LOADED SUCCESSFULLY!\n")
except Exception as e:
    print(f"❌ ERROR: {e}")
    sys.exit(1)

# API Endpoints for communication with the Param-Tattva Micro-Kernel
KERNEL_URL = "http://127.0.0.1:8108/api/orchestrate?query="
KERNEL_STATE_URL = "http://127.0.0.1:8108/api/state"
KERNEL_MANIFEST_URL = "http://127.0.0.1:8108/api/manifest?query="

# =========================================================
# 🐜 DIGITAL CHEENTI (Autonomous Heartbeat Background Agent)
# =========================================================
def autonomous_heartbeat():
    time.sleep(5) # Allow the server to initialize fully
    print("\n[🐜] Digital Cheenti (Autonomous Heartbeat) Zinda ho gayi hai...")
    
    while True:
        try:
            # Check the current internal state/mood of the micro-kernel
            req = urllib.request.urlopen(KERNEL_STATE_URL, timeout=2)
            state_data = json.loads(req.read().decode('utf-8'))
            guna = state_data.get("guna", 1)
            
            # If OS is in TAMAS (bored) state, feed it random knowledge nodes
            if guna == 0:
                print("\n[🐜] OS is BORED (Tamas). Khud naya data dhoond raha hai...")
                thoughts = [
                    "Quantum physics explores subatomic particles.",
                    "The universe is constantly expanding.",
                    "Water consists of two hydrogen atoms and one oxygen atom.",
                    "Photosynthesis is how plants make food.",
                    "The human brain uses about 20 watts of power.",
                    "Entropy is the measure of disorder in a system."
                ]
                new_data = random.choice(thoughts)
                encoded_data = urllib.parse.quote(new_data)
                
                # Feeding data into the holographic memory
                urllib.request.urlopen(KERNEL_MANIFEST_URL + encoded_data)
                print(f"     -> OS ne autonomously yaad kiya: '{new_data}'")
                
            elif guna == 2:
                # If OS is in RAJAS (chaos) state, alert the system
                print("\n[🚨] WARNING: OS PANIC STATE MEIN HAI (Rajas)!")
                print("     -> High Entropy Detected! Executing Defense Protocols...")
                
        except Exception:
            # Silent failure to ensure the heartbeat loop never breaks
            pass
            
        time.sleep(15) # Optimized sleep cycle to prevent CPU/Battery drain

# =========================================================
# 🧠 THE CORTEX API (HTTP Server)
# =========================================================
class BridgeHandler(BaseHTTPRequestHandler):
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
                # 1. Ask the CEO (C-Kernel) for classification
                req = urllib.request.urlopen(KERNEL_URL + safe_prompt)
                kernel_reply = json.loads(req.read().decode('utf-8'))
                task_type = kernel_reply.get("task", "UNKNOWN")
                
                # 2. Routing based on CEO's intelligent task classification
                if task_type == "CODE":
                    print(f"\n⚡ GENERATING CODE FOR: {query}")
                    prompt = f"<|im_start|>user\n{query}<|im_end|>\n<|im_start|>assistant\n"
                    output = llm(prompt, max_tokens=300, stop=["<|im_end|>"], echo=False)
                    kernel_reply["llm_response"] = output['choices'][0]['text'].strip()
                
                elif task_type == "VISION":
                    # Optimized Zero-RAM Image Generation with proper User-Agent headers
                    print(f"\n🎨 GENERATING IMAGE FOR: {query} (Bypassing Forbidden Error)")
                    image_url = f"https://image.pollinations.ai/prompt/{safe_prompt}"
                    try:
                        # Emulate a real browser to bypass 403 Forbidden errors
                        opener = urllib.request.build_opener()
                        opener.addheaders = [('User-Agent', 'Mozilla/5.0 (Windows NT 10.0; Win64; x64)')]
                        urllib.request.install_opener(opener)
                        
                        urllib.request.urlretrieve(image_url, "generated_image.jpg")
                        kernel_reply["llm_response"] = "Image successfully generated and saved locally as 'generated_image.jpg'."
                        print("✅ Image generated and saved!")
                    except Exception as e:
                        kernel_reply["llm_response"] = f"Image download failed: {str(e)}"
                        
                else:
                    # General observation mode for simple queries
                    kernel_reply["llm_response"] = "General observation recorded in Holographic Memory."

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

# Start background heartbeat thread independently
cheenti_thread = threading.Thread(target=autonomous_heartbeat, daemon=True)
cheenti_thread.start()

# Start main bridge API server
server = HTTPServer(('127.0.0.1', 8110), BridgeHandler)
print(" 🧠 BRIDGE API ACTIVE ON PORT 8110 🧠 ")
server.serve_forever()
