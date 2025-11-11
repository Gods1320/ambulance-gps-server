#include <Arduino.h>

// LilyGO T-SIM7000G Pins
#define MODEM_PWRKEY 4
#define MODEM_PWR_ON 25
#define MODEM_RX 26
#define MODEM_TX 27

#define SerialMon Serial
#define SerialAT  Serial1

void modemPowerOn() {
  pinMode(MODEM_PWR_ON, OUTPUT);
  pinMode(MODEM_PWRKEY, OUTPUT);

  digitalWrite(MODEM_PWR_ON, HIGH);
  delay(50);

  // PWRKEY press
  digitalWrite(MODEM_PWRKEY, LOW);
  delay(1500);
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(5000);
}

void sendAT(const char *cmd, uint16_t timeout = 2000) {
  SerialMon.print(">> "); SerialMon.println(cmd);
  SerialAT.print(cmd); SerialAT.print("\r");

  unsigned long start = millis();
  while (millis() - start < timeout) {
    if (SerialAT.available())
      SerialMon.write(SerialAT.read());
  }
  SerialMon.println();
}

void setup() {
  SerialMon.begin(115200);
  delay(300);
  SerialMon.println("\n🔧 SIM7000 Automatic LTE-M Recovery (Globe PH)");

  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  modemPowerOn();

  // Try to wake modem
  for (int i = 0; i < 6; i++) {
    SerialMon.printf("🔎 AT probe %d/6...\n", i + 1);
    SerialAT.print("AT\r");
    delay(500);
    if (SerialAT.find("OK")) break;
  }

  SerialMon.println("\n📟 Device Info:");
  sendAT("ATE0");
  sendAT("ATI");
  sendAT("AT+CPIN?");
  sendAT("AT+CBC");

  SerialMon.println("\n📡 Unlocking LTE Bands (Band 3 + Band 28 for Globe)...");
  sendAT("AT+CBANDCFG=\"CAT-M\",3,28", 4000);

  SerialMon.println("\n📶 LTE-M Only Mode...");
  sendAT("AT+CMNB=1");
  sendAT("AT+CNMP=38");

  SerialMon.println("\n🌐 Set Globe APN...");
  sendAT("AT+CGDCONT=1,\"IP\",\"internet.globe.com.ph\"");

  SerialMon.println("\n🔄 Restarting radio...");
  sendAT("AT+CFUN=1,1", 3000);
  delay(7000); // Wait for module to fully reboot

  SerialMon.println("\n⏱ Waiting for Network...");
}

void loop() {

  sendAT("AT+CSQ");     // Signal level
  sendAT("AT+CREG?");   // LTE Network registration
  sendAT("AT+CGREG?");  // Data registration
  sendAT("AT+CGATT?");  // Packet attach

  // If signal is still 99,99 → Not receiving LTE
  // **MOVE OUTDOORS / NEAR WINDOW**
  // Globe Band 28 often does NOT penetrate indoors well.

  SerialMon.println("⏳ Waiting 5s...\n");
  delay(5000);
}
