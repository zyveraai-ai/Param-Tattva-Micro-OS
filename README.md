# ⚛️ Param-Tattva: O(1) Cognitive Edge Micro-Kernel

Param-Tattva is a bare-metal, ultra-lightweight AI micro-kernel written in pure C. It bypasses modern massive LLM architectures by utilizing Hyperdimensional Computing (HDC) and Spiking Neural Networks (SNN) logic. 

Designed for Edge devices, IoT, and low-end hardware, it fetches semantic data from the web, compresses it into mathematical strings using XOR logic, and runs on a completely locked **1KB RAM footprint**.

## 🔥 Core Features
1. **O(1) Memory Architecture (Bindu):** Persistent memory overlapping using bitwise XOR (`brain.bin`).
2. **Spiking Threshold Logic:** Extreme CPU efficiency. Processes data only when action potentials cross the set threshold.
3. **Standalone C-Server:** Built-in multi-threaded POSIX socket server (`pthreads`). No external dependencies.
4. **Hacker-Proof:** Locked strictly to `127.0.0.1` (Localhost) to prevent external WiFi network breaches.

## 🚀 How to Run (Linux / Android Termux)

Step 1: Clone the engine
`git clone https://github.com/YOUR_USERNAME/Param-Tattva-Micro-OS.git`
`cd Param-Tattva-Micro-OS`

Step 2: Compile the Kernel
`clang param_tattva.c -o param_tattva -lpthread -lm`

Step 3: Ignite the OS
`./param_tattva &`

Step 4: Open UI in Browser
Open Incognito mode and go to `http://localhost:8108`
