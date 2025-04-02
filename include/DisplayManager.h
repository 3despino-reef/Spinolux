#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Arduino.h>

// Definir constantes de la pantalla
#define WIDTH_DISPLAY 128
#define HEIGHT_DISPLAY 64

// Prototipos de funciones
void setupDisplay();
void updateDisplay0(String lightMode, byte powerCh0, byte powerCh1, byte powerCh2, byte powerCh3, float readingSTemp, int hour, int minute, boolean wifiState);
void updateDisplay1(byte hourSunrise, byte minuteSunrise, byte hourSundown, byte minuteSundown, int sunriseDuration, int sundownDuration, byte powerCh0Max, byte powerCh1Max, byte powerCh2Max, byte powerCh3Max);
void showMenuCarrusel(String options[], byte numberOptions, byte selectedOption);
void drawScrollbar(byte selectedOption, byte numberOptions);
void showPowerAdjust(byte selectedCh, byte selectedOption);
void showTimeAdjust(byte hour, byte minute, byte selectedOption, bool editing);
void showMessageSave();
void drawSaveIcon(int x, int y);
void showRampAdjust(byte selectedOption, String rampType);


#endif // DISPLAY_MANAGER_H
