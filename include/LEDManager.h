// LedManager.h
#ifndef LEDMANAGER_H
#define LEDMANAGER_H

#include <Arduino.h> 

void setupLED();
void updateLEDStatus(String alarmSTemp, String alarmReadingSTemp, bool wifiState, bool systemInTime);


#endif