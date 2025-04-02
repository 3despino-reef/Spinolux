#include "LEDPWMManager.h"
#include <math.h>

LEDPWMManager::LEDPWMManager(const byte ch, int pinCh, const int frequency, const int resolution) 
    : _channel(ch), _PWMChannel(0) {
    
    // Configuramos el pin como OUTPUT
    pinMode(pinCh, OUTPUT);
    ledcSetup(ch, frequency, resolution);
    ledcAttachPin(pinCh, ch);
    
    // Iniciamos con el canal en un valor bajo 
    ledcWrite(ch, 0);
}

// Función para convertir porcentaje de potencia a valor PWM (0-255)
int LEDPWMManager::convertPowerToPWM(int powerPercentage) const {
    float exponent = 2.2; // Simulación de percepción visual
    int pwmValue = round(pow((float)powerPercentage / 100.0, exponent) * 255);
    
    // Limitar valores dentro del rango PWM
    return constrain(pwmValue, 0, 255);
}

// Función para convertir PWM (0-255) a porcentaje de potencia (0-100%)
int LEDPWMManager::convertPWMToPower(int pwmValue) const {
    float exponent = 2.2;
    int powerPercentage = round(pow((float)pwmValue / 255.0, 1 / exponent) * 100);
    
    return constrain(powerPercentage, 0, 100);
}

// Función para el amanecer con ajuste de velocidad de transición
unsigned long LEDPWMManager::sunrise(int sunriseDuration, int PWMTargetMax, unsigned long previousMillis) {  
    if (millis() - previousMillis > 60000) { // Se ejecuta una vez por minuto
        previousMillis = millis();
        
        if (_PWMChannel < PWMTargetMax) {
            int stepSize = (PWMTargetMax - _PWMChannel) / max(1, sunriseDuration); // Dividir el cambio en pasos
            _PWMChannel += max(1, stepSize); // Aseguramos que al menos aumente en 1. Evitamos error de división de enteros
            if (_PWMChannel > PWMTargetMax) _PWMChannel = PWMTargetMax; // No superar el máximo
            ledcWrite(_channel, _PWMChannel);
        }
    }
    return previousMillis;
}

// Función para el anochecer con ajuste de velocidad de transición
unsigned long LEDPWMManager::sundown(int sundownDuration, int PWMMin, unsigned long previousMillis) {  
    if (millis() - previousMillis > 60000) { // Se ejecuta una vez por minuto
        previousMillis = millis();

        if (_PWMChannel > PWMMin) {
            int stepSize = (_PWMChannel - PWMMin) / max(1, sundownDuration);
            _PWMChannel -= max(1, stepSize);
            if (_PWMChannel < PWMMin) _PWMChannel = PWMMin; // Asegurar que no baje por debajo del mínimo
            ledcWrite(_channel, _PWMChannel);
        }
    }
    return previousMillis;
}


// Método para actualizar el estado de la luz
unsigned long LEDPWMManager::update(
    const String &lightMode, 
    int hour, 
    int minute, 
    int hourSunrise, 
    int minuteSunrise, 
    int hourSundown, 
    int minuteSundown, 
    int sunriseDuration,
    int sundownDuration, 
    int powerMaxNormal,  
    int powerMaxSaver, 
    String alarmReadingSTemp, 
    bool moonMode,   // <-- Se usa solo moonMode para decidir el PWM mínimo
    unsigned long previousMillis, 
    unsigned long &previousMillisCycle, 
    String &state
) {
    // Convertimos el porcentaje de potencia al valor PWM adecuado
    int PWMTargetMax = convertPowerToPWM((alarmReadingSTemp == "on") ? powerMaxSaver : powerMaxNormal);

    // Si moonMode está activado, el PWM mínimo debe ser 1 en lugar de 0
    int PWMMin = (moonMode == true) ? 1 : 0;

    // Convertimos horas y minutos a minutos absolutos para comparación
    int currentTime = hour * 60 + minute;
    int sunriseTime = hourSunrise * 60 + minuteSunrise;
    int sundownTime = hourSundown * 60 + minuteSundown;

    // Si el sistema está en AUTO y el canal está apagado, pero el modo luna está activado, forzamos el PWM a 1
    if (lightMode == "auto" && _PWMChannel == 0 && moonMode) {
        _PWMChannel = 1;
        ledcWrite(_channel, _PWMChannel);
    }

    if (lightMode == "auto") {
        if (currentTime >= sunriseTime && currentTime < sundownTime) { // AMANECER y ENCENDIDO 
            state = (_PWMChannel < PWMTargetMax) ? "amaneciendo" : "encendido";
            previousMillis = sunrise(sunriseDuration, PWMTargetMax, previousMillis);
        } 
        else { // ANOCHECER
            state = (_PWMChannel > PWMMin) ? "anocheciendo" : (_PWMChannel == 0 ? "apagado" : "luna");
            previousMillis = sundown(sundownDuration, PWMMin, previousMillis);
        }
    }
    else if (lightMode == "on") {
        state = (_PWMChannel < PWMTargetMax) ? "amaneciendo" : "encendido";
        previousMillis = sunrise(0, PWMTargetMax, previousMillis); // Encendido inmediato
    } 
    else if (lightMode == "off") {
        state = (_PWMChannel > PWMMin) ? "anocheciendo" : (_PWMChannel == 0 ? "apagado" : "luna");
        previousMillis = sundown(0, PWMMin, previousMillis); // Apagado inmediato
    }
    else if (lightMode == "cycle") {
        if ((millis() - previousMillisCycle > 86400000) || (previousMillisCycle == 0)) {
            previousMillisCycle = millis();
        }
        if (millis() - previousMillisCycle < 36000000) { // Ciclo de encendido
            state = (_PWMChannel < PWMTargetMax) ? "amaneciendo" : "encendido";
            previousMillis = sunrise(sunriseDuration, PWMTargetMax, previousMillis);
        } 
        else { // Ciclo de apagado
            state = (_PWMChannel > PWMMin) ? "anocheciendo" : (_PWMChannel == 0 ? "apagado" : "luna");
            previousMillis = sundown(sundownDuration, PWMMin, previousMillis);
        }
    }
    
    return previousMillis;
}


// Getter para obtener el valor actual del canal 
int LEDPWMManager::getPowerCh() const {
    return convertPWMToPower(_PWMChannel);
}