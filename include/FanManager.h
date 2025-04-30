// fanControl.h
#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

#include <Arduino.h>

class FanController {
  private:
    int fanPin;
    float tempOnThreshold;
    float tempOffThreshold;
    String alarmTemp;
    String alarmSensor;
    float temperature;
    bool fanState;

  public:
    FanController(int pin, float onThreshold, float offThreshold) {
      fanPin = pin;
      tempOnThreshold = onThreshold;
      tempOffThreshold = offThreshold;
      fanState = false;
      pinMode(fanPin, OUTPUT);
      digitalWrite(fanPin, LOW); // Ventilador apagado por defecto
    }

    void updateFan(float currentTemp, String alarmTempStatus, String alarmSensorStatus) {
      temperature = currentTemp;
      alarmTemp = alarmTempStatus;
      alarmSensor = alarmSensorStatus;

      // Activar ventilador si temperatura supera el umbral o si hay alarmas
      if (temperature >= tempOnThreshold || alarmTemp == "on" || alarmSensor == "on") {
        turnFanOn();
      }
      // Apagar ventilador solo si la temperatura baja del umbral de apagado y no hay alarmas
      else if (temperature < tempOffThreshold && alarmTemp != "on" && alarmSensor != "on") {
        turnFanOff();
      }
    }

    void turnFanOn() {
      if (!fanState) {
        digitalWrite(fanPin, HIGH);
        fanState = true;
        Serial.println("Ventilador ENCENDIDO");
      }
    }

    void turnFanOff() {
      if (fanState) {
        digitalWrite(fanPin, LOW);
        fanState = false;
        Serial.println("Ventilador APAGADO");
      }
    }

    bool isFanOn() {
      return fanState;
    }
};

#endif
