import urllib.request
import urllib.parse
import json
import sys

# =========================================================
# THE MAGIC PATCH: TRICKING THE AI INTO THINKING IT'S LINUX
# =========================================================
if sys.platform == "android":
    sys.platform = "linux"
# =========================================================

print("=====================================================")
print(" ⏳ INITIATING LOCAL OFFLINE AI ENGINE (Llama.cpp)...")
print("=====================================================")
try:
    from llama_cpp import Llama
    # Load the 400MB model downloaded in RAM
    llm = Llama(model_path="./model.gguf", n_ctx=2048, verbose=False)
    print("✅ ENGINE LOADED SUCCESSFULLY!\n")
except Exception as e:
    print(f"❌ ERROR LOADING AI: {e}")
    sys.exit(1)

KERNEL_URL = "http://127.0.0.1:8108/api/orchestrate?query="

def ask_kernel(user_prompt):
    safe_prompt = urllib.parse.quote(user_prompt)
    try:
        response = urllib.request.urlopen(KERNEL_URL + safe_prompt)
        data = json.loads(response.read().decode('utf-8'))
        return data
    except Exception as e:
        return {"error": str(e)}

print(" 🧠 PARAM-TATTVA MASTERMIND BRIDGE | v2.0 ACTIVE 🧠 ")
print("Type 'exit' to quit.\n")

while True:
    user_input = input("👤 [USER]: ")
    
    if user_input.lower() == 'exit':
        print("Shutting down bridge...")
        break
        
    print("⚙️  [OS]: Scanning Request...")
    kernel_reply = ask_kernel(user_input)
    
    if "error" in kernel_reply:
        print(f"❌ [FATAL]: Kernel is offline! Run ./param-tattva in the other tab.\n")
        continue

    task_type = kernel_reply.get("task", "UNKNOWN")
    
    # ---------------------------------------------------------
    # THE MAGIC: ROUTING TO LOCAL OFFLINE AI
    # ---------------------------------------------------------
    if task_type == "CODE":
        print(f"✅ [DECISION]: CODING TASK. Routing to Local Code-LLM...\n")
        print("⚡ [GENERATING CODE OFFLINE - Exynos 1580 POWER] ⚡")
        print("-" * 50)
        
        # Qwen-Coder special prompt format
        prompt = f"<|im_start|>user\n{user_input}<|im_end|>\n<|im_start|>assistant\n"
        
        # Generate the output completely offline!
        output = llm(prompt, max_tokens=300, stop=["<|im_end|>"], echo=False)
        print(output['choices'][0]['text'].strip())
        print("\n" + "-" * 50 + "\n")
        
    else:
        print(f"✅ [DECISION]: GENERAL CHAT.")
        print(f"🗣️ [AI]: Main ek coding-focused AI hu. Please mujhse code likhne, bugs fix karne ya app banane ke sawaal puchiye.\n")

