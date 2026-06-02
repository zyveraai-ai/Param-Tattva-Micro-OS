# ⚛️ Param-Tattva: 1KB Cognitive Edge Micro-Kernel

Param-Tattva is a bare-metal, ultra-lightweight AI micro-kernel written in pure C. It completely bypasses modern massive LLM architectures by utilizing Hyperdimensional Computing (HDC) and Spiking Neural Networks (SNN) logic. 

Designed for Edge devices, Swarm Drones, and IoT, it compresses semantic data into mathematical hashes using XOR logic and runs on a strictly locked **1KB RAM footprint**.

## 🚀 The Architecture (API-First)
This is not a GUI application. It is a headless, hack-proof POSIX socket server running strictly on `127.0.0.1:8108`. It provides a completely decentralized cognitive backend for your hardware.

### API Routes for Developers:

**1. Check System State**
`GET http://localhost:8108/api/state`
*Returns the strict 1KB memory limits and kernel status.*

**2. Manifest & SNN Spike (Learn)**
`GET http://localhost:8108/api/manifest?query=obstacle`
*Hashes the query, folds it into the `brain.bin` using XOR, and triggers an SNN spike if the threshold is crossed.*

**3. Karma Protocol (Unlearn/Undo)**
`GET http://localhost:8108/api/karma?query=obstacle`
*Mathematically reverses the XOR binding to remove specific knowledge without increasing memory overhead.*

## 🛠️ How to Build & Ignite

Step 1: Clone the engine
```bash
git clone [https://github.com/zyveraai-ai/Param-Tattva-Micro-OS.git](https://github.com/zyveraai-ai/Param-Tattva-Micro-OS.git)
cd Param-Tattva-Micro-OS
