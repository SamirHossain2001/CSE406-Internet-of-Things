// A.ino
// Node A - policy based: choose direct vs via B using delay measurement

#include "painlessMesh.h"

#define MESH_PREFIX "CSE406"
#define MESH_PASSWORD "12345678"
#define MESH_PORT 5555

Scheduler userScheduler;
painlessMesh mesh;

const uint32_t TARGET_NODE_ID = 3050603722UL; // C
const uint32_t MIDDLE_NODE_ID = 4206966532UL; // B

const int32_t DELAY_THRESHOLD_US = 70000; // tune as needed (70 ms)

String pendingPayload = "";
bool awaitingMeasurement = false;

Task taskSendMessage( TASK_SECOND * 5 , TASK_FOREVER, [](){
  pendingPayload = "Hello from A (policy)";
  // Ask library to measure delay to target
  if (mesh.startDelayMeas(TARGET_NODE_ID)) {
    awaitingMeasurement = true;
    Serial.printf("[A] Requested delay measurement to %u\n", TARGET_NODE_ID);
  } else {
    // fallback: measure to middle node (B)
    Serial.printf("[A] startDelayMeas to %u failed. Trying middle %u\n", TARGET_NODE_ID, MIDDLE_NODE_ID);
    if (mesh.startDelayMeas(MIDDLE_NODE_ID)) {
      awaitingMeasurement = true;
      Serial.printf("[A] Requested delay measurement to middle %u\n", MIDDLE_NODE_ID);
    } else {
      Serial.printf("[A] Could not start delay meas to middle either. Skipping this round.\n");
      pendingPayload = "";
      awaitingMeasurement = false;
    }
  }
});

void nodeDelayReceivedCallback(uint32_t nodeId, int32_t delay) {
  Serial.printf("[A] Delay result node=%u delay=%d us\n", nodeId, delay);
  if (!awaitingMeasurement) return;

  if (nodeId == TARGET_NODE_ID) {
    if (delay >= 0 && delay < DELAY_THRESHOLD_US) {
      // direct send
      bool ok = mesh.sendSingle(TARGET_NODE_ID, pendingPayload);
      Serial.printf("[A] Direct send to %u (delay=%d). sendSingle returned %s\n", TARGET_NODE_ID, delay, ok ? "true" : "false");
    } else {
      // ask B to forward
      String msg = "FORWARD_TO:" + String(TARGET_NODE_ID) + ":" + pendingPayload;
      bool ok = mesh.sendSingle(MIDDLE_NODE_ID, msg);
      Serial.printf("[A] Asking B (%u) to forward (delay=%d). sendSingle returned %s\n", MIDDLE_NODE_ID, delay, ok ? "true" : "false");
    }
    pendingPayload = "";
    awaitingMeasurement = false;
    return;
  }

  if (nodeId == MIDDLE_NODE_ID) {
    if (delay >= 0 && delay < DELAY_THRESHOLD_US) {
      // good path to B — ask B to forward to target
      String msg = "FORWARD_TO:" + String(TARGET_NODE_ID) + ":" + pendingPayload;
      bool ok = mesh.sendSingle(MIDDLE_NODE_ID, msg);
      Serial.printf("[A] No good direct path; asked B to forward. sendSingle returned %s\n", ok ? "true" : "false");
    } else {
      Serial.printf("[A] Middle node delay too high (%d). Not sending.\n", delay);
    }
    pendingPayload = "";
    awaitingMeasurement = false;
    return;
  }
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("[A] Received from %u : %s\n", from, msg.c_str());
}

void setup() {
  Serial.begin(115200);
  delay(100);
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION | COMMUNICATION );
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);

  mesh.onReceive(&receivedCallback);
  mesh.onNodeDelayReceived([](uint32_t nodeId, int32_t delay){ nodeDelayReceivedCallback(nodeId, delay); });

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();

  Serial.printf("[A] Node started. nodeId=%u\n", mesh.getNodeId());
}

void loop() {
  mesh.update();
}
