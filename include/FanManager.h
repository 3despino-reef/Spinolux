#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include <Arduino.h>

class FanController {
  private:
    int fanPin;
    float tempOnThreshold;
    float tempOffThreshold;
    bool fanState;

  public:
    FanController(int pin, float onThreshold, float offThreshold);
    void updateFan(float temperature, String alarmTemp, String alarmSensor);
    bool isFanOn() const;
};

#endif

