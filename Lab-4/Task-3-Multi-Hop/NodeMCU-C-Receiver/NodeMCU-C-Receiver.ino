// C_receiver.ino
// Node C - final destination. Sends ACK back to the origin.

#include "painlessMesh.h"

#define MESH_PREFIX "CSE406"
#define MESH_PASSWORD "12345678"
#define MESH_PORT 5555

Scheduler userScheduler;
painlessMesh mesh;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("[C] Received from %u : %s\n", from, msg.c_str());

  // Send an ACK back to original sender
  String ack = String("ACK from C to ") + String(from) + " : got your message";
  bool ok = mesh.sendSingle(from, ack);
  Serial.printf("[C] Sent ACK to %u. sendSingle returned %s\n", from, ok ? "true" : "false");
}

void changedConnectionCallback() {
  Serial.println("[C] Changed connections - topology:");
  String topo = mesh.subConnectionJson(true);
  Serial.println(topo);
}

void nodeDelayReceivedCallback(uint32_t nodeId, int32_t delay) {
  Serial.printf("[C] Delay node=%u delay=%d us\n", nodeId, delay);
}

void setup() {
  Serial.begin(115200);
  delay(100);
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION | COMMUNICATION );
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);

  mesh.onReceive(&receivedCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeDelayReceived([](uint32_t nodeId, int32_t delay){ nodeDelayReceivedCallback(nodeId, delay); });

  Serial.printf("[C] Receiver node started. nodeId=%u\n", mesh.getNodeId());
}

void loop() {
  mesh.update();
}
