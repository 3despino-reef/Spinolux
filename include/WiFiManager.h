#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

void    setupWiFi();
boolean checkWiFi();  // Función que será llamada en el loop

#endif