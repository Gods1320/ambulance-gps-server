#ifndef SIM7000_H
#define SIM7000_H

#include <TinyGsmClient.h>

extern TinyGsm modem;
extern TinyGsmClient client;

void modemPowerOn();
bool connectToNetwork();
bool connectToGPRS();
void sendGPSData(float lat, float lon, float speed, float alt, float acc, int vsat, int usat);

#endif
