# ⚛️ Param-Tattva: 1KB Cognitive Edge Micro-Kernel

Param-Tattva is a bare-metal, ultra-lightweight AI micro-kernel written in pure C. It completely bypasses modern massive LLM architectures by utilizing Hyperdimensional Computing (HDC) and Spiking Neural Networks (SNN) logic. 

Designed for Edge devices, Swarm Drones, and IoT, it compresses semantic data into mathematical hashes and runs on a strictly locked **1KB RAM footprint**. 

## 🧠 The Architecture (Headless API)
This is not a GUI application. It is a headless, hack-proof POSIX socket server running strictly on `127.0.0.1:8108`. It acts as an independent cognitive backend for your hardware, allowing developers to route sensor data directly through its engine without internet dependency.

### Core Mechanics:
* **O(1) Memory Footprint:** Uses Reversible Bitwise XOR to fold infinite semantic hashes onto the exact same 1024-byte memory block (`brain.bin`). The memory never expands.
* **Event-Driven Compute:** The kernel idles near 0W until an SNN threshold is breached, preventing thermal throttling in enclosed hardware (e.g., aerospace, medical implants).
* **Multi-Lingual Hashing:** Processes any UTF-8 input string, converting raw text/data into 32-bit mathematical nodes.

---

## 📡 API Reference for Developers

Once the kernel is running, it exposes three primary endpoints. Send standard HTTP `GET` requests from any language (Python, JS, C++) to interact with the engine.

### 1. Check System State
Retrieves the current memory limit and runtime status.
* **Endpoint:** `GET http://localhost:8108/api/state`
* **Response:**
  ```json
  {"os":"Param-Tattva", "memory_limit_bytes":1024, "status":"running"}
