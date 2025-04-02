#include "WiFiManager.h"
#include "config.h"

const char* ssid = WIFI_SSID;
const char* password = "PASSWORD";

unsigned long previousWifiReconnect = 0; // Tiempo del último intento de reconexión
const unsigned long delayWifiReconnect = 60000; // Intervalo entre intentos de reconexión (30min en milisegundos)


void setupWiFi() {
  WiFi.begin(ssid, "PASSWORD");
  
  unsigned long startTime = millis();
  const unsigned long timeout = 10000; // 5 segundos para intentar la conexión inicial
  
  //Si el wifi no está conectado
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    if (millis() - startTime >= timeout) {
      Serial.println("");
      Serial.println("No se pudo conectar al WiFi en el inicio, continuando sin conexión.");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("Conectado a ");
    Serial.println(ssid);
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
  }
}

boolean checkWiFi() {
  boolean state = false;
  // Verifica si no está conectado
  if (WiFi.status() != WL_CONNECTED) {
    // Comprueba si ha pasado el tiempo de reconexión 
    if (millis() - previousWifiReconnect > delayWifiReconnect) {
      previousWifiReconnect = millis(); // Actualiza el tiempo del último intento
      WiFi.reconnect(); // Intenta reconectar al WiFi
      Serial.println("checkWiFi: Intentando reconectar WiFi...");
    }
  } else {
    Serial.println("checkWiFi: Conectado al WiFi");
    state = true;
  }
  return state;
}
