// LedManager.cpp
#include "LEDManager.h"
#include <config.h>

void setupLED() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  // Apagar todos los pines inicialmente
  digitalWrite(RED_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  
}

// Función para controlar el LED
void updateLEDStatus(String alarmSTemp, String alarmReadingSTemp, bool wifiState, bool systemInTime) {

  // Caso 1: Si hay alarmas (alarmSTemp o alarmReadingSTemp en "on"), el LED se pone rojo
  if (alarmSTemp == "on" || alarmReadingSTemp == "on") {
    digitalWrite(RED_PIN, HIGH); 
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    vTaskDelay(400 / portTICK_PERIOD_MS);
  }
  // Caso 2: Si no hay alarmas y mqttState o wifiState están en false, se pone el azul fijo
  else if (!wifiState) {
    digitalWrite(RED_PIN, LOW); 
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS); 
  }
  // Caso 3: Si no hay alarmas y mqttState y wifiState están en false pero systemInTime es false, se pone azul a parpadear
  else if (!systemInTime) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);  
    digitalWrite(BLUE_PIN, LOW);
    vTaskDelay(300 / portTICK_PERIOD_MS);  
  }
  // Caso 4: Si no ocurre nada de lo anterior, el LED parpadea en verde (como una especie de heartbeat)
  else {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);  
    digitalWrite(GREEN_PIN, LOW);
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}