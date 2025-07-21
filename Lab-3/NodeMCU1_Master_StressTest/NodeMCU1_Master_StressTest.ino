#include <SoftwareSerial.h>



SoftwareSerial mySerial(D5, D6); // RX = D5, TX = D6

const int baudRates[] = {9600, 38400, 115200};
const int messageSizes[] = {10, 50, 100};
const int intervals[] = {0, 10, 100};
const unsigned long testDuration = 10000; // 10 seconds per test
const int timeout = 1000; // 1000ms timeout for response
const int baudChangeTimeout = 5000; // 5s timeout for baud change confirmation

int currentBaud = 9600; // Initial baud

void setup() {
  Serial.begin(115200); // Debugging
  mySerial.begin(currentBaud);
  Serial.println("NodeMCU 1: UART Master Stress Test");
  delay(200);
}

bool changeBaudRate(int newBaud) {
  Serial.print("Master: sending SETBAUD:");
  Serial.println(newBaud);
  mySerial.print("SETBAUD:");
  mySerial.println(newBaud);

  // Wait for con
  unsigned long start = millis();
  bool confirmed = false;
  while (millis() - start < baudChangeTimeout) { // wait 5s for response
    if (mySerial.available()) {
      String resp = mySerial.readStringUntil('\n');
      resp.trim();
      if (resp == ("OK:" + String(newBaud))) {
        confirmed = true;
        Serial.print("Master: got confirmation ");
        Serial.println(resp);
        break;
      }
    }
  }
  if (!confirmed) {
    Serial.println("Master: WARNING: no confirmation from slave!");
    return false;
  }

  mySerial.end();
  delay(100); // clear
  mySerial.begin(newBaud);
  currentBaud = newBaud;
  Serial.print("Master: switched SoftSerial to ");
  Serial.print(newBaud);
  Serial.println(" bps");
  delay(100);
  return true;
}

void loop() {
  // Run tests for each baud rate
  for (int baud : baudRates) {
    Serial.print("Testing baud rate: ");
    Serial.println(baud);
    if (changeBaudRate(baud)) {
      // Run tests for each message size
      for (int size : messageSizes) {
        // Run tests for each interval
        for (int interval : intervals) {
          runTest(baud, size, interval);
          delay(2000); // Pause between tests
        }
      }
    } else {
      Serial.print("Failed to switch to ");
      Serial.print(baud);
      Serial.println(" bps, skipping tests");
    }
  }
  Serial.println("All tests complete. Restarting in 10 seconds...");
  delay(10000);
}

void runTest(int baud, int size, int interval) {
  Serial.print("Test: Baud=");
  Serial.print(baud);
  Serial.print(", Size=");
  Serial.print(size);
  Serial.print(" bytes, Interval=");
  Serial.print(interval);
  Serial.println("ms");

  unsigned long startTime = millis();
  int messagesSent = 0;
  int messagesReceived = 0;
  int errors = 0;
  unsigned long bytesSent = 0;
  int sequence = 0;

  // Generate test message
  String baseMessage = "D";
  while (baseMessage.length() < size - 6) { // Reserve 6 bytes for sequence
    baseMessage += "X";
  }

  // Run test for 10 seconds
  while (millis() - startTime < testDuration) {
    // Send message with sequence number
    String message = String(sequence) + ":" + baseMessage;
    mySerial.println(message);
    bytesSent += message.length() + 2; // Include \r\n
    messagesSent++;
    sequence++;

    // Wait for response with timeout
    unsigned long responseStart = millis();
    String receivedData = "";
    bool received = false;

    while (millis() - responseStart < timeout) {
      if (mySerial.available()) {
        char c = mySerial.read();
        if (c == '\n') {
          received = true;
          break;
        }
        receivedData += c;
      }
    }

    // Trim received data to remove \r or whitespace
    receivedData.trim();

    // Verify response
    if (received && receivedData == message) {
      messagesReceived++;
    } else {
      errors++;
      if (!received) {
        Serial.println("Timeout error");
      } else {
        Serial.print("Mismatch error: Expected '");
        Serial.print(message);
        Serial.print("', Got '");
        Serial.print(receivedData);
        Serial.println("'");
      }
    }

    // Delay for specified interval
    if (interval > 0) {
      delay(interval);
    }
  }

  // Calculate and print results
  float durationSeconds = testDuration / 1000.0;
  float throughput = bytesSent / durationSeconds; // Bytes per second
  float messageRate = messagesSent / durationSeconds; // Messages per second
  float errorRate = (errors * 100.0) / messagesSent; // Percentage

  Serial.println("Test Results:");
  Serial.print("Messages Sent: ");
  Serial.println(messagesSent);
  Serial.print("Messages Received: ");
  Serial.println(messagesReceived  );
  Serial.print("Errors: ");
  Serial.println(errors);
  Serial.print("Error Rate: ");
  Serial.print(errorRate);
  Serial.println("%");
  Serial.print("Throughput: ");
  Serial.print(throughput);
  Serial.println(" bytes/second");
  Serial.print("Message Rate: ");
  Serial.print(messageRate);
  Serial.println(" messages/second");
  Serial.println();
}