#pragma once
#include <Arduino.h>

#define TINY_GSM_MODEM_SIM7000    // <<< REQUIRED so TinyGSM knows which modem we use
#include <TinyGsmClient.h>        // <<< Now TinyGSM types exist

// These are defined in sim7000.cpp
extern TinyGsm modem;
extern TinyGsmClient client;

void sim7000_init();
bool ensureNetwork();
bool ensureGPRS();
