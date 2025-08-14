// B.ino
// Node B - middle / relay. Handles FORWARD_TO requests.

#include "painlessMesh.h"

#define MESH_PREFIX "CSE406"
#define MESH_PASSWORD "12345678"
#define MESH_PORT 5555

Scheduler userScheduler;
painlessMesh mesh;

void printNeighboursJsonLike() {
  std::list<uint32_t> nodeList = mesh.getNodeList();
  String s = "[";
  bool first = true;
  for (auto id : nodeList) {
    if (!first) s += ",";
    s += String(id);
    first = false;
  }
  s += "]";
  Serial.println(s);
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("[B] Received from %u : %s\n", from, msg.c_str());

  // If application-level forward request: "FORWARD_TO:<dest>:<payload>"
  if (msg.startsWith("FORWARD_TO:")) {
    int colonPos = msg.indexOf(':', 11); // find the second colon after the prefix
    if (colonPos > 11) {
      String destStr = msg.substring(11, colonPos);
      uint32_t destId = (uint32_t) destStr.toInt();
      String payload = msg.substring(colonPos + 1);
      Serial.printf("[B] Parsed forward request -> dest=%u payload='%s'\n", destId, payload.c_str());
      bool ok = mesh.sendSingle(destId, payload);
      Serial.printf("[B] Forwarded to %u. sendSingle returned %s\n", destId, ok ? "true" : "false");
    } else {
      Serial.println("[B] Malformed FORWARD_TO message (no second colon).");
    }
  } else {
    // regular message that might be forwarded by library or direct to B
    Serial.printf("[B] Normal message content: %s\n", msg.c_str());
  }
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("[B] New Connection: %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.println("[B] Changed connections - topology:");
  String topo = mesh.subConnectionJson(true);
  Serial.println(topo);

  Serial.print("[B] Known nodes JSON-like: ");
  printNeighboursJsonLike();

  // start delay meas to all known nodes to observe link quality
  std::list<uint32_t> nodeList = mesh.getNodeList();
  for (auto id : nodeList) {
    if (mesh.startDelayMeas(id)) {
      Serial.printf("[B] started delay measurement to %u\n", id);
    } else {
      Serial.printf("[B] could NOT start delay measurement to %u\n", id);
    }
  }
}

void nodeDelayReceivedCallback(uint32_t nodeId, int32_t delay) {
  Serial.printf("[B] Delay node=%u delay=%d us\n", nodeId, delay);
}

void setup() {
  Serial.begin(115200);
  delay(100);
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION | COMMUNICATION | MESH_STATUS );
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeDelayReceived([](uint32_t nodeId, int32_t delay){ nodeDelayReceivedCallback(nodeId, delay); });

  Serial.printf("[B] Middle node started. nodeId=%u\n", mesh.getNodeId());
}

void loop() {
  mesh.update();
}
