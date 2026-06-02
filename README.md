# ⚛️ Param-Tattva: 1KB Cognitive Edge Micro-Kernel

Param-Tattva is a bare-metal, ultra-lightweight AI micro-kernel written in pure C. It completely bypasses modern massive LLM architectures by utilizing Hyperdimensional Computing (HDC) and Spiking Neural Networks (SNN) logic. 

Designed for Edge devices, Swarm Drones, and IoT, it compresses semantic data into mathematical hashes and runs on a strictly locked **1KB RAM footprint**. 

## 🧠 The Architecture (Headless API)

This is not a GUI application. It is a headless, hack-proof POSIX socket server running strictly on 127.0.0.1:8108. It acts as an independent cognitive backend for your hardware, allowing developers to route sensor data directly through its engine without internet dependency.

### Core Mechanics:
* O(1) Memory Footprint: Uses Reversible Bitwise XOR to fold infinite semantic hashes onto the exact same 1024-byte memory block (brain.bin). The memory never expands.
* Event-Driven Compute: The kernel idles near 0W until an SNN threshold is breached, preventing thermal throttling in enclosed hardware.
* Multi-Lingual Hashing: Processes any UTF-8 input string, converting raw text/data into 32-bit mathematical nodes.

---

## 📡 API Reference for Developers

Once the kernel is running, it exposes three primary endpoints. Send standard HTTP GET requests to interact with the engine.

### 1. Check System State
Retrieves the current memory limit and runtime status.

Endpoint: 
GET http://localhost:8108/api/state

Response:
{"os":"Param-Tattva", "memory_limit_bytes":1024, "status":"running"}


### 2. Manifest & SNN Spike (Learn)
Hashes the query, folds it into the memory block, and calculates the action potential.

Endpoint: 
GET http://localhost:8108/api/manifest?query=obstacle_detected

Response:
{"status":"success", "operation":"fold", "node_hash":543216, "snn_spike":1}


### 3. Karma Protocol (Unlearn/Undo)
Mathematically reverses the XOR binding to remove specific knowledge without increasing memory overhead.

Endpoint: 
GET http://localhost:8108/api/karma?query=obstacle_detected

Response:
{"status":"success", "operation":"reversed", "node_removed":543216}

---

## 🛠️ How to Build & Ignite (Linux / Termux / macOS)

Step 1: Clone the repository
git clone https://github.com/zyveraai-ai/Param-Tattva-Micro-OS.git
cd Param-Tattva-Micro-OS

Step 2: Compile the bare-metal C-Kernel
clang param-tattva.c -o param-tattva

Step 3: Ignite the Engine
./param-tattva

(The terminal will output: [STATUS] Engine Active & Locked on 127.0.0.1:8108)

---
Built with focus on the Physics of Compute. Open Source for the Edge.
