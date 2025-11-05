#include <Arduino.h>
#include "sim7000.h"
#include "gps.h"

#define HOST "ungainful-propublication-hien.ngrok-free.dev"  // change later
#define PORT 80

void gps_init() {
  modem.sendAT("+SGPIO=0,4,1,1");
  modem.waitResponse(10000);
  modem.enableGPS();
}

void sendGPSData() {
  float lat, lon, speed, alt, acc;
  int vsat, usat, year, month, day, hour, min, sec;

  if (modem.getGPS(&lat, &lon, &speed, &alt, &vsat, &usat, &acc,
                   &year, &month, &day, &hour, &min, &sec)) {

    if (!ensureNetwork() || !ensureGPRS()) return;

    String uri = "/gps_api/post_gps.php?lat=" + String(lat, 6) +
                 "&lon=" + String(lon, 6) +
                 "&speed=" + String(speed) +
                 "&alt=" + String(alt) +
                 "&vsat=" + String(vsat) +
                 "&usat=" + String(usat) +
                 "&acc=" + String(acc);

    if (client.connect(HOST, PORT)) {
      client.print(String("GET ") + uri + " HTTP/1.1\r\n" +
                   "Host: " + HOST + "\r\n" +
                   "Connection: close\r\n\r\n");
      delay(500);
      client.stop();
      Serial.println("✅ GPS Sent");
    }
  } else {
    Serial.println("⏳ Waiting for GPS Fix...");
  }
}
