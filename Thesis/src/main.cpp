#include <Arduino.h>
#include "sim7000.h"
#include "gps.h"

void setup() {
  Serial.begin(115200);
  delay(2000);
  sim7000_init();
  gps_init();
}

void loop() {
  sendGPSData();
  delay(10000);
}
