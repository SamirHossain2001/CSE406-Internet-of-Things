#define LED_RED 13
#define LED_YELLOW 12
#define LED_GREEN 8
#define BUZZER 7

void setup(){
  Serial.begin(9600);     // Communication started with 9600 baud
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}

void loop(){
  int sensor = analogRead(A0); // Incoming analog signal read and appointed sensor
  Serial.println(sensor);      // Wrote serial port

  // Reset all outputs
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  noTone(BUZZER);

  if (sensor > 20 && sensor <= 100) {
    digitalWrite(LED_GREEN, HIGH); // turn on green led
    tone(BUZZER, 500, 200); // Short low beep for low water level
    delay(200);
  }
  else if (sensor > 100 && sensor <= 200) {
    digitalWrite(LED_YELLOW, HIGH); // turn on yellow led
    tone(BUZZER, 1000, 200); // Medium beep for medium water level
    delay(200);
    noTone(BUZZER);
    delay(200);
    tone(BUZZER, 1000, 200); // Double beep pattern
    delay(200);
  }
  else if (sensor > 200) {
    digitalWrite(LED_RED, HIGH); // turn on red led
    tone(BUZZER, 1500, 300); // Longer high-pitched beep for high water level
    delay(300);
    noTone(BUZZER);
    delay(100);
    tone(BUZZER, 1500, 300); // Triple beep pattern
    delay(300);
    noTone(BUZZER);
    delay(100);
    tone(BUZZER, 1500, 300);
    delay(300);
  }
}