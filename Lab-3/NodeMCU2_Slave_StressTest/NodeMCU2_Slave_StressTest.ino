#include <SoftwareSerial.h>

const uint8_t RX = D5;
const uint8_t TX = D6;
SoftwareSerial mySerial(RX, TX);

// Supported baud rates
const int SUPPORTED_BAUDS[] = {9600, 38400, 115200}; // for making sure
const uint8_t BAUD_COUNT = sizeof(SUPPORTED_BAUDS) / sizeof(SUPPORTED_BAUDS[0]);

bool inHandshake = false;
int handshakeBaud = 0;

void setup() {
  Serial.begin(115200); // Debugging
  mySerial.begin(9600); // Start with 9600
  Serial.println("NodeMCU 2: UART Slave Initialized");
  delay(200);
}

void loop() {
  if (!inHandshake && mySerial.available()) {
    String cmd = mySerial.readStringUntil('\n');
    cmd.trim();

    if (cmd.startsWith("SETBAUD:")) {
      handshakeBaud = cmd.substring(8).toInt();

      // is supported?
      bool okRate = false;
      for (uint8_t i = 0; i < BAUD_COUNT; i++) {
        if (SUPPORTED_BAUDS[i] == handshakeBaud) {
          okRate = true;
          break;
        }
      }

      if (!okRate) {
        Serial.printf("NodeMCU 2: unsupported baud %d – ignoring\n", handshakeBaud);
      } else {
        inHandshake = true;
        Serial.printf("NodeMCU 2: BEGIN handshake to %d bps\n", handshakeBaud);

        // Clear junk
        mySerial.flush();
        delay(100);

        // Send confirmation
        for (uint8_t attempt = 0; attempt < 3; attempt++) {
          mySerial.printf("OK:%d\n", handshakeBaud);
          Serial.printf("NodeMCU 2: sent OK:%d (attempt %d)\n", handshakeBaud, attempt + 1);
          delay(200);
        }

        // Switch to the new baud rate
        mySerial.end();
        delay(500);
        mySerial.begin(handshakeBaud);
        Serial.printf("NodeMCU 2: switched SoftSerial to %d bps\n", handshakeBaud);
        delay(500);

        inHandshake = false;
        Serial.println("NodeMCU 2: handshake complete");
      }
    } else {
      Serial.print("NodeMCU 2: Received '");
      Serial.print(cmd);
      Serial.println("'");
      mySerial.println(cmd);
      Serial.println("NodeMCU 2: Echoed message");
    }
  }
}