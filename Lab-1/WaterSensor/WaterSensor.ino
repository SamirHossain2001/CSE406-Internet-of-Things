
void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud
  Serial.println("Water sensor reading started...");
}

void loop() {
  int sensorValue = analogRead(A0);  // Read raw analog value from water sensor

  Serial.print("Water sensor value: ");
  Serial.print(sensorValue);

  // Interpret water level based on the sensor value
  if (sensorValue > 600) {
    Serial.println(" --> High water level detected.");
  } else if (sensorValue > 300) {
    Serial.println(" --> Medium water level.");
  } else {
    Serial.println(" --> Low or no water detected.");
  }

  delay(1000);  // Wait 1 second before next reading
}
