#include "gps.h"
#include <TinyGsmClient.h>

extern TinyGsm modem;

// Order: lat, lon, speed(km/h), alt(m), vsat, usat, acc(m)
bool getGPSFix(float &lat, float &lon, float &speed, float &alt, float &acc, int &vsat, int &usat) {
  return modem.getGPS(&lat, &lon, &speed, &alt, &vsat, &usat, &acc);
}
