#define TINY_GSM_MODEM_SIM7000
#include <Arduino.h>
#include <TinyGsmClient.h>

#define SerialMon Serial
#define SerialAT  Serial1

// Pins (LilyGO T-SIM7000G)
#define MODEM_TX      27
#define MODEM_RX      26
#define MODEM_PWRKEY   4
#define MODEM_PWR_ON  25

TinyGsm modem(SerialAT);

// APN for Globe PH
const char* APN = "internet"; // Smart Prepaid

// Server (Railway)
const char* HOST = "ambulance-gps-server-production.up.railway.app";
const char* ENDPOINT = "/post_gps";

// ---------------- POWER ON MODEM ----------------
void modemPowerOn() {
  pinMode(MODEM_PWR_ON, OUTPUT);
  pinMode(MODEM_PWRKEY, OUTPUT);

  digitalWrite(MODEM_PWR_ON, HIGH);
  delay(100);
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(100);
  digitalWrite(MODEM_PWRKEY, LOW);
  delay(1500);
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(5000);
}

// ---------------- NETWORK REGISTRATION ----------------
bool connectToNetwork() {
  SerialMon.println("\n📡 Registering to LTE network...");

  modem.sendAT("+CMNB=1"); // LTE-M only
  modem.waitResponse();

  modem.sendAT("+CNMP=38"); // Force LTE only mode
  modem.waitResponse();

  if (!modem.waitForNetwork(30000)) {
    SerialMon.println("❌ Network registration failed.");
    return false;
  }

  SerialMon.println("✅ Network Registered");
  return true;
}

// ---------------- PACKET DATA ATTACH ----------------
bool connectToGPRS() {
  SerialMon.println("🌐 Enabling mobile data...");

  modem.sendAT("+CGDCONT=1,\"IP\",\"internet\"");
  modem.waitResponse();

  modem.sendAT("+CGATT=1");
  if (modem.waitResponse(15000, "+CGATT: 1") != 1) {
    SerialMon.println("❌ Attach failed.");
    return false;
  }

  modem.sendAT("+CNACT=1,\"internet\"");
  if (modem.waitResponse(15000, "+CNACT: 1,") != 1) {
    SerialMon.println("❌ PDP activation failed.");
    return false;
  }

  SerialMon.println("✅ Mobile Data Active 🌍");
  return true;
}

// ---------------- SEND GPS DATA ----------------
void sendGPSData(float lat, float lon, float speed, float alt, float acc, int vsat, int usat) {

  String body = "{";
  body += "\"lat\":" + String(lat, 6) + ",";
  body += "\"lon\":" + String(lon, 6) + ",";
  body += "\"speed\":" + String(speed) + ",";
  body += "\"alt\":" + String(alt) + ",";
  body += "\"acc\":" + String(acc) + ",";
  body += "\"vsat\":" + String(vsat) + ",";
  body += "\"usat\":" + String(usat);
  body += "}";

  SerialMon.println("\n🔐 HTTPS POST with SIM7000 stack:");
  SerialMon.println(body);

  modem.sendAT("+HTTPTERM");
  modem.waitResponse();

  modem.sendAT("+HTTPINIT");
  modem.waitResponse();

  modem.sendAT("+HTTPPARA=\"CID\",1");
  modem.waitResponse();

  modem.sendAT("+HTTPPARA=\"CONTENT\",\"application/json\"");
  modem.waitResponse();

  modem.sendAT("+HTTPPARA=\"URL\",\"https://" + String(HOST) + ENDPOINT + "\"");
  modem.waitResponse();

  modem.sendAT("+HTTPSSL=1");
  modem.waitResponse();

  modem.sendAT("+HTTPDATA=" + String(body.length()) + ",5000");
  if (modem.waitResponse(6000) == 1) {
    SerialAT.print(body);
    modem.waitResponse(6000);
  }

  modem.sendAT("+HTTPACTION=1");
  if (modem.waitResponse(15000, "+HTTPACTION:") == 1)
    SerialMon.println("✅ POST Completed!");
  else
    SerialMon.println("⚠️ POST may still have gone through");

  SerialMon.println("\n----- SERVER RESPONSE -----");
  modem.sendAT("+HTTPREAD");
  modem.waitResponse(10000);
  while (SerialAT.available()) SerialMon.write(SerialAT.read());
  SerialMon.println("\n----- END RESPONSE -----");

  modem.sendAT("+HTTPTERM");
  modem.waitResponse();
}
