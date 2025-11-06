#define TINY_GSM_MODEM_SIM7000
#include <Arduino.h>
#include <TinyGsmClient.h>
#include <TinyGSM.h>

#include "sim7000.h"
#include "gps.h"

#define SerialMon Serial
#define SerialAT  Serial1

#define MODEM_TX      27
#define MODEM_RX      26
#define MODEM_PWRKEY   4
#define MODEM_PWR_ON  25

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);   // ✅ NORMAL CLIENT (we enable SSL via connect(..., true))

const char* apnList[] = {
  "internet.globe.com.ph",
  "internet"
};
const int apnCount = 2;

const char* host = "ambulance-gps-server-production.up.railway.app";
const int httpsPort = 443;
const char* endpoint = "/post_gps";

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

  // Construct JSON body
  String body = "{";
  body += "\"lat\":" + String(lat, 6) + ",";
  body += "\"lon\":" + String(lon, 6) + ",";
  body += "\"speed\":" + String(speed) + ",";
  body += "\"alt\":" + String(alt) + ",";
  body += "\"acc\":" + String(acc) + ",";
  body += "\"vsat\":" + String(vsat) + ",";
  body += "\"usat\":" + String(usat);
  body += "}";

  SerialMon.println("\n🔐 HTTPS POST with SIM7000 internal stack:");
  SerialMon.println(body);

  modem.sendAT("+HTTPTERM");  
  modem.waitResponse(2000);

  modem.sendAT("+HTTPINIT");
  modem.waitResponse(2000);

  modem.sendAT("+HTTPPARA=\"CID\",1");
  modem.waitResponse(2000);

  modem.sendAT("+HTTPPARA=\"URL\",\"https://ambulance-gps-server-production.up.railway.app/post_gps\"");
  modem.waitResponse(2000);

  modem.sendAT("+HTTPPARA=\"CONTENT\",\"application/json\"");
  modem.waitResponse(2000);

  modem.sendAT("+HTTPSSL=1");   // Enable HTTPS mode
  modem.waitResponse(2000);

  modem.sendAT("+HTTPDATA=" + String(body.length()) + ",5000");
  if (modem.waitResponse(6000) == 1) {
    SerialAT.print(body);
    modem.waitResponse(6000);
  }

  modem.sendAT("+HTTPACTION=1"); // POST action
  if (modem.waitResponse(15000) == 1) {
    SerialMon.println("✅ POST Sent!");
  } else {
    SerialMon.println("⚠️ No HTTPACTION response (network slow?)");
  }

  // Read server response
  SerialMon.println("\n----- SERVER RESPONSE -----");
  modem.sendAT("+HTTPREAD");
  if (modem.waitResponse(10000) == 1) {
    while (SerialAT.available()) {
      SerialMon.write(SerialAT.read());
    }
  }
  SerialMon.println("\n----- END RESPONSE -----\n");

  modem.sendAT("+HTTPTERM");
  modem.waitResponse(2000);
}


