#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include <DisplayManager.h>
#include <ButtonManager.h>

// Inicialización del menú
void setupMenuControls();
// Gestión de la navegación del menú
bool isMenuActive();
void processMenuNavigation();
void checkMenuTimeout();
// Funciones para manejar cada submenú
void handleMenu(byte shortClicks, byte longClicks);
void handleMainMenu(byte shortClicks, byte longClicks);
void handleModeMenu(byte shortClicks, byte longClicks);
void handlePowerMenu(byte shortClicks, byte longClicks);
void handlePowerAdjust(byte shortClicks, byte longClicks);
void handleSunriseAdjust(byte shortClicks, byte longClicks);
void handleSundownAdjust(byte shortClicks, byte longClicks);
void handleSunriseRampAdjust(byte numberShortClicks, byte numberLongClicks);
void handleSundownRampAdjust(byte numberShortClicks, byte numberLongClicks);
void handleMoonMode(byte numberShortClicks, byte numberLongClicks);

#endif // MENU_MANAGER_H