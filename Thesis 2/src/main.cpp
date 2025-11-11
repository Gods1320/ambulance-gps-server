#include <Arduino.h>
#include "sim7000.h"
#include "gps.h"

#define SerialMon Serial
#define SerialAT  Serial1

void setup() {
  SerialMon.begin(115200);
  delay(1000);

  SerialMon.println("\n🚀 Booting GPS Tracker...");

  // Start modem UART
  SerialAT.begin(115200, SERIAL_8N1, 26, 27);  // RX, TX (LilyGO T-SIM7000G default)

  modemPowerOn();      // Power the modem module
  delay(8000);

  modem.restart();     // Soft restart
  modem.sendAT("+SGPIO=0,4,1,1"); // Enable GPS power pin on T-SIM
  modem.waitResponse(5000);

  modem.enableGPS();
  SerialMon.println("🛰 GPS Enabled. Getting Fix...");
}

void loop() {
  float lat, lon, speed, alt, acc;
  int vsat, usat;

  // Get GPS fix
  if (!getGPSFix(lat, lon, speed, alt, acc, vsat, usat)) {
    SerialMon.println("⏳ GPS Searching...");
    delay(2000);
    return;
  }

  SerialMon.println("\n✅ GPS FIX!");
  SerialMon.printf("Lat: %.6f | Lon: %.6f\n", lat, lon);
  SerialMon.printf("Speed: %.2f km/h | Alt: %.2f m\n", speed, alt);
  SerialMon.printf("Satellites: Visible=%d Used=%d | Accuracy: ±%.1f m\n\n", vsat, usat, acc);

  // Connect to network
  if (!connectToNetwork()) {
    SerialMon.println("❌ Network Failed");
    delay(5000);
    return;
  }

  if (!connectToGPRS()) {
    SerialMon.println("❌ GPRS Failed");
    delay(5000);
    return;
  }

  // Send coordinates
  sendGPSData(lat, lon, speed, alt, acc, vsat, usat);

  SerialMon.println("⏳ Waiting 10s...\n");
  delay(10000);
}
