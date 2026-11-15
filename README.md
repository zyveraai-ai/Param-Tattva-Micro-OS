
# ⚛️ Param-Tattva OS v10.0: Enterprise Edge AGI MVP



A sovereign, ultra-lightweight **Edge AGI Micro-OS** engineered in pure **C** and **Python**, running in **<1 KB active L1-Cache RAM (`580 Bytes`)** with zero GPU dependencies.



## 📂 Clean Modular Architecture

- **`param_tattva.c`** — 580-Byte C-Kernel (2048-bit Bindu SDR, `__builtin_popcountll` 0.002ms recall, Math/GST ALU & `/api/gati` 6-DOF Robotics Reflex API).

- **`drishti_engine.c`** — Native C Vision Trainer (3D Bayes Auto-Select + 6-Ray Collinear-Resonance Object Eraser).

- **`bridge.py`** — Cortex Server & Eklavya Teacher AI Distiller.

- **`index.html`** — 4-Tab Enterprise Dashboard (Cortex, Drishti v10, Zyvera Audit Lock & Gati Robotics).



## 🚀 Quick Run

```bash

clang param_tattva.c -O3 -lm -o param-tattva -lpthread

clang drishti_engine.c -O3 -lm -o drishti-engine

./param-tattva &

python bridge.py &cd ~/Param-Tattva-Micro-OS

stty -echo



cat << 'EOF' > .gitignore

param-tattva

drishti-engine

*.bin

__pycache__/

*.gguf

