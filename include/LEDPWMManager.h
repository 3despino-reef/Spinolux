#ifndef LEDPWM_MANAGER_H
#define LEDPWM_MANAGER_H

#include <Arduino.h>

class LEDPWMManager {
public:
    LEDPWMManager(const byte channel, int pinChannel, const int frequency, const int resolution);
    
    // Método para actualizar el estado del LED según el modo de luz y la hora
    unsigned long update(
        const String &lightMode, 
        int hour, 
        int minute, 
        int hourSunrise, 
        int minuteSunrise, 
        int hourSundown, 
        int minuteSundown, 
        int sunriseDuration,  // Duración en minutos del amanecer
        int sundownDuration,  // Duración en minutos del anochecer
        int powerMaxNormal,  // En porcentaje (0-100)
        int powerMaxSaver,   // En porcentaje (0-100)
        String alarmReadingSTemp, 
        bool moonMode,     
        unsigned long previousMillis, 
        unsigned long &previousMillisCycle, 
        String &state
    );

    int getPowerCh() const; // Ahora devuelve el porcentaje de potencia (0-100%)

private:
    byte _channel;
    byte _PWMChannel; // Almacena el valor PWM actual (0-255)

    unsigned long sunrise(int sunriseDuration, int PWMTargetMax, unsigned long previousMillis);
    unsigned long sundown(int sundownDuration, int PWMMin, unsigned long previousMillis);

    // Conversión de potencia a valor PWM (0-255) y viceversa (ahora ambas son `const`)
    int convertPowerToPWM(int powerPercentage) const;
    int convertPWMToPower(int pwmValue) const;
};

#endif // LEDPWM_MANAGER_H
