#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include <Arduino.h>

class FanManager {
  private:
    int fanPin;
    float tempOnThreshold;
    float tempOffThreshold;
    bool fanState;

  public:
    FanManager(int pin, float onThreshold, float offThreshold);
    void updateFan(float temperature, String alarmTemp, String alarmSensor);
    bool isFanOn() const;
};

#endif

