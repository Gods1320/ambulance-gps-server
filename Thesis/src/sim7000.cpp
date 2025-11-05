#include "sim7000.h"

// ---------- Pins ----------
#define SerialAT Serial1
#define MODEM_TX      27
#define MODEM_RX      26
#define MODEM_PWRKEY   4
#define MODEM_PWR_ON  25

const char* apn = "internet.globe.com.ph";

// Create modem + client
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
  delay(200);
}

void sim7000_init() {
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  modemPowerOn();
  delay(6000);
  modem.restart();
}

bool ensureNetwork() {
  return modem.waitForNetwork(15000);
}

bool ensureGPRS() {
  if (!modem.isGprsConnected()) {
    modem.gprsConnect(apn);
  }
  return modem.isGprsConnected();
}
