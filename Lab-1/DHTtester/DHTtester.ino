#include "DHT.h"

// Define the pin where the DHT sensor is connected
#define DHTPIN 2

// Define the sensor type (DHT11, DHT22 or DHT21)
#define DHTTYPE DHT11

// Create a DHT sensor object
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);           // Start serial communication at 9600 baud
  Serial.println(F("DHTxx test"));  
  dht.begin();                  // Initialize the DHT sensor
}

void loop() {
  delay(1000); // Wait 1 seconds between readings (DHT sensors are slow)

  float h = dht.readHumidity();            // Read relative humidity
  float t = dht.readTemperature();         // Read temperature in Celsius
  float f = dht.readTemperature(true);     // Read temperature in Fahrenheit

  // If any reading fails, display error and skip this cycle
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Calculate heat index (feels-like temperature)
  float hif = dht.computeHeatIndex(f, h);          // In Fahrenheit
  float hic = dht.computeHeatIndex(t, h, false);   // In Celsius

  // Display the results
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
}
