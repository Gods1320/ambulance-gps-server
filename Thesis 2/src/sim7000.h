#ifndef SIM7000_H
#define SIM7000_H

#include <TinyGsmClient.h>

// LilyGO T-SIM7000G pins
#define MODEM_TX      27
#define MODEM_RX      26
#define MODEM_PWRKEY   4
#define MODEM_PWR_ON  25

// Expose modem so main.cpp and gps.cpp can use it
extern TinyGsm modem;

// Power / network helpers
void modemPowerOn();
bool connectToNetwork();
bool connectToGPRS();

// HTTPS sender
void sendGPSData(float lat, float lon, float speed, float alt, float acc, int vsat, int usat);

#endif
