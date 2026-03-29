#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Hello from ESP32-S3!");
}

void loop() {
  delay(1000);
}
