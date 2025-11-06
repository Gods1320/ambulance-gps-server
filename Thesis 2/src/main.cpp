#include <Arduino.h>
#include "sim7000.h"
#include "gps.h"

#define SerialMon Serial
#define SerialAT  Serial1

void setup() {
  SerialMon.begin(115200);
  delay(1000);

  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  modemPowerOn();
  delay(8000);

  modem.restart();  // ✅ Restart modem

  // ✅ Place the SSL configuration *RIGHT HERE*
  modem.sendAT("+CSSLCFG=\"SSLVERSION\",0,3");  // Use TLS 1.2
  modem.waitResponse();
  modem.sendAT("+CSSLCFG=\"CIPHER\",0,\"ALL\""); // Allow all cipher suites
  modem.waitResponse();
  modem.sendAT("+CSSLCFG=\"SECPOLICY\",0,1");   // Ignore certificate validation
  modem.waitResponse();
  modem.sendAT("+CSSLCFG=\"SNI\",0,1");         // Enable SNI for Railway/ngrok
  modem.waitResponse();

  // ✅ GPS Power ON (unchanged)
  modem.sendAT("+SGPIO=0,4,1,1");
  modem.waitResponse();
  modem.enableGPS();
  SerialMon.println("🛰 GPS Enabled. Getting Fix...");
}


void loop() {
  float lat, lon, speed, alt, acc;
  int vsat, usat;

  if (!getGPSFix(lat, lon, speed, alt, acc, vsat, usat)) {
    SerialMon.println("⏳ GPS Searching...");
    delay(2000);
    return;
  }

  SerialMon.println("\n✅ GPS FIX!");
  SerialMon.printf("Lat: %.6f | Lon: %.6f\n", lat, lon);

  if (!connectToNetwork()) return;
  if (!connectToGPRS()) return;

  sendGPSData(lat, lon, speed, alt, acc, vsat, usat);

  delay(10000);
}
