# ⚛️ Param-Tattva: 1KB Cognitive Edge Micro-Kernel

Param-Tattva is an ultra-lightweight, POSIX-compliant AI micro-kernel written in pure C. It completely bypasses modern, bloated LLM architectures by utilizing **Hyperdimensional Computing (HDC)** and **Spiking Neural Networks (SNN)** logic. 

Designed for Edge devices, Swarm Drones, and Mission-Critical hardware, it compresses semantic data into mathematical hashes and runs on a strictly locked **1KB RAM footprint**. 

---

## 🚀 Mission-Critical Deployments

Traditional AI crashes in extreme environments. Param-Tattva thrives in them:

* **🛰️ Space Exploration & Satellites (Radiation Resilience):** HDC data is holographic and distributed. If cosmic rays cause "bit-flips" in the 1KB memory, the mathematical hash degrades gracefully instead of crashing. It provides zero-latency cognitive telemetry for rovers idling near 0 Watts.
* **🏎️ Automotive & Industrial (Anomaly Detection):** Instead of streaming heavy raw data over a car's CAN bus, sensors running this kernel can fold engine vibration data into a local hash. If the baseline shifts, it triggers an instant SNN spike, sending a tiny 32-bit warning to the central computer.
* **🐝 Swarm Robotics (Low-Bandwidth):** Swarm drones operating in GPS-denied environments can broadcast O(1) mathematical hash states to each other over LoRa, sharing spatial awareness without complex JSON parsing.

---

## 🧠 The Math Behind the Magic

To achieve an O(1) memory footprint without standard libraries or heap allocation (`malloc`), Param-Tattva relies on two paradigms:

1. **Reversible HDC (XOR Folding):** Raw data isn't stored as strings. It is mapped to high-dimensional bit-vectors and "folded" onto the exact same 1024-byte memory block (`brain.bin`) using Bitwise XOR. The memory never expands. 
2. **SNN (Spiking Neural Networks):** There are no floating-point weights. The engine tracks binary action potentials. When the bundled XOR hash reaches a density threshold, the node "spikes" (outputs a `1`), triggering an event.

---

## 🌐 The Asymmetric Architecture

This is **not** a GUI application. It is a headless, hack-proof POSIX socket server (running strictly on `127.0.0.1:8108`). 

Param-Tattva refuses to waste memory on UI rendering or string manipulation. It only crunches math. To interact with it, developers must build a **"Smart Client Layer"** (like a Web Dashboard or Python script) that routes natural language and internet APIs, while the Edge OS remains locked down.

*Target Hardware: Micro-Linux SBCs (Raspberry Pi Zero), Termux (Android Edge), or POSIX-compliant RTOS (ESP32/Zephyr).*

---

## 📡 API Reference for Developers

Once ignited, the engine exposes a REST-like interface over raw TCP sockets.

### 1. Check System State
Retrieves the current memory limit and runtime status.

    GET http://localhost:8108/api/state

**Response:**

    {"os":"Param-Tattva", "memory_limit_bytes":1024, "status":"running"}

### 2. Manifest & SNN Spike (Learn)
Hashes the query, folds it into the memory block, and calculates the action potential.

    GET http://localhost:8108/api/manifest?query=obstacle_detected

**Response:**

    {"status":"success", "operation":"fold", "node_hash":543216, "snn_spike":1}

### 3. Karma Protocol (Unlearn / Undo)
Because XOR is mathematically reversible, the Karma Protocol removes specific knowledge by XORing the exact same hash again—unlearning data without increasing memory overhead.

    GET http://localhost:8108/api/karma?query=obstacle_detected

**Response:**

    {"status":"success", "operation":"reversed", "node_removed":543216}

---

## 🛠️ How to Build & Ignite (Linux / Termux / macOS)

**Step 1: Clone the repository**

    git clone https://github.com/zyveraai-ai/Param-Tattva-Micro-OS.git
    cd Param-Tattva-Micro-OS

**Step 2: Compile the C-Kernel**
*(Compiles directly using host POSIX headers)*

    clang param-tattva.c -o param-tattva

**Step 3: Ignite the Engine**

    ./param-tattva

*(The terminal will output: [STATUS] Engine Active & Locked on 127.0.0.1:8108)*

---
*Built with focus on the Physics of Compute. Open Source for the Edge.*
