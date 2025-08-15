# painlessMesh Lab

**Lab:** Mesh Networking with NodeMCU (ESP8266) using `painlessMesh`  
**Author:** Md. Samir Hossain (2022-3-60-161).  

---


## 1. Overview
This project demonstrates **mesh networking** between ESP8266 NodeMCU boards using the **painlessMesh** library. It covers:
- Callback message handling
- Direct messaging to specific nodes
- Multi-hop messaging (A → B → C)
- Advantages of mesh over star topology

---

## 2. Hardware & network setup
- **Hardware:** Three NodeMCU / ESP8266 boards (Nodes A, B, C).  
- **Mesh params used in lab:**  
  - Mesh name: `CSE406`  
  - Password: `12345678`  
  - Port: `5555`  
- **Serial monitor baud:** `115200`  
- **Demo node IDs (from logs):**  
  - Node A (sender): `3299373438`  
  - Node B (relay): `4206966532`  
  - Node C (receiver): `3050603722`  

---

## 3. Files in this repository
- **Task-1** (Callback Messages)
- **Task-2** (Direct Messaging)
- **Task-3** (Multi-Hop Messaging A → B → C)
- **Report pdf**

---

## 4. Requirements
- Arduino IDE with **painlessMesh** library
- ESP8266 board package installed

---

## 5. Usage
- Flash each NodeMCU with its respective `.ino` file.
- Open serial monitors at `115200 baud`.
- Observe connection logs, direct messages, and relay behavior.