#include "FanManager.h"

FanManager::FanManager(int pin, float onThreshold, float offThreshold)
  : fanPin(pin), tempOnThreshold(onThreshold), tempOffThreshold(offThreshold), fanState(false) {
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin, LOW); // Inicialmente apagado
}

void FanManager::updateFan(float temperature, String alarmTemp, String alarmSensor) {
  if (alarmTemp == "on" || alarmSensor == "on" || temperature >= tempOnThreshold) {
    if (!fanState) {
      digitalWrite(fanPin, HIGH);
      fanState = true;
    }
  } else if (temperature < tempOffThreshold && alarmTemp != "on" && alarmSensor != "on") {
    if (fanState) {
      digitalWrite(fanPin, LOW);
      fanState = false;
    }
  }
}

bool FanManager::isFanOn() const {
  return fanState;
}
