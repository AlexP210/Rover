// DHT Macros
#include "DHT.h"
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 

// Light sensor macros
#define LIGHTPIN A0  // Analog pin for light readings
#define LEDPIN 13 // LED Pin

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Start serial port
  Serial.begin(9600);
  // Start DHT sensor
  dht.begin();
  // Start Light sensor
  pinMode(LEDPIN, OUTPUT);
  pinMode(LIGHTPIN, INPUT);
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Read humidity
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  
  // Read light level
  digitalWrite(LEDPIN, LOW);
  int l = analogRead(LIGHTPIN);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(l)) {
    Serial.println(F("Failed to read!"));
    return;
  }

  Serial.print(F("light: "));
  Serial.print(l);
  Serial.print(", ");

  Serial.print(F("humidity: "));
  Serial.print(h);
  Serial.print(F("%, "));

  Serial.print(F("temperature: "));
  Serial.print(t);
  Serial.print(F("C"));
  Serial.print(F("\n"));
}
