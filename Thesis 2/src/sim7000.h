#ifndef SIM7000_H
#define SIM7000_H

#define MODEM_TX      27
#define MODEM_RX      26
#define MODEM_PWRKEY   4
#define MODEM_PWR_ON  25

#include <TinyGsmClient.h>

extern TinyGsm modem;

void modemPowerOn();
bool connectToNetwork();
bool connectToGPRS();
void sendGPSData(float lat, float lon, float speed, float alt, float acc, int vsat, int usat);

#endif
