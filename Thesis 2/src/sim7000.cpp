#include "sim7000.h"
#include <Arduino.h>

#define SerialMon Serial
#define SerialAT  Serial1

#define MODEM_TX      27
#define MODEM_RX      26
#define MODEM_PWRKEY   4
#define MODEM_PWR_ON  25

const char* apnList[] = {
  "internet.globe.com.ph",
  "internet"
};
const int apnCount = 2;

const char* host = "ambulance-gps-server-production.up.railway.app";
const int httpPort = 80;
const char* endpoint = "/post_gps";   // not php anymore

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

void modemPowerOn() {
  pinMode(MODEM_PWR_ON, OUTPUT);
  pinMode(MODEM_PWRKEY, OUTPUT);

  digitalWrite(MODEM_PWR_ON, HIGH);
  delay(100);
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(500);
  digitalWrite(MODEM_PWRKEY, LOW);
  delay(500);
}

bool connectToNetwork() {
  SerialMon.println("\n📡 Checking Network...");
  return modem.waitForNetwork(8000);
}

bool connectToGPRS() {
  SerialMon.println("🌐 Activating GPRS...");
  for (int i = 0; i < apnCount; i++) {
    if (modem.gprsConnect(apnList[i])) return true;
  }
  return false;
}

void sendGPSData(float lat, float lon, float speed, float alt, float acc, int vsat, int usat) {

  // Build URL query string
  String url = String(endpoint) +
  "?lat=" + String(lat, 6) +
  "&lon=" + String(lon, 6) +
  "&speed=" + speed +
  "&alt=" + alt +
  "&vsat=" + vsat +
  "&usat=" + usat +
  "&acc=" + acc;

client.connect(host, httpPort);


  SerialMon.println("\n📤 Sending GPS Data (HTTP):");
  SerialMon.println("http://" + String(host) + ":" + String(httpPort) + url);

if (!client.connect(host, httpPort)) {
  SerialMon.println("❌ HTTP Connection Failed");
  return;
}

client.print(
  String("GET ") + endpoint + "?lat=" + lat + "&lon=" + lon + "&speed=" + speed + 
  "&alt=" + alt + "&vsat=" + vsat + "&usat=" + usat + "&acc=" + acc + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" +
  "Connection: close\r\n\r\n"
);

  SerialMon.println("\n----- SERVER RESPONSE -----");
  unsigned long timeout = millis();
  while (millis() - timeout < 6000) {
    while (client.available()) {
      SerialMon.write(client.read());
    }
  }
  SerialMon.println("\n----- END RESPONSE -----");

  client.stop();
  SerialMon.println("✅ Data Sent!\n");
}

