/*========================
  COMENTARIOS 
========================
VERSION:08/08/2024
*/

// MODOS DE FUNCIONAMIENTO:
//  on--> activación manual
//  cycle--> no se tienen en cuenta los datos mqtt ni la hora. Las luces funcionan de forma cíclica (modo de emergencia)
//  auto--> modo por defecto. Enciende y apaga a las horas indicadas

#include <Arduino.h>
#include <config.h>
#include <Preferences.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <esp_task_wdt.h>
#include <LEDPWMManager.h>
#include <WiFiManager.h>
#include <DisplayManager.h>
#include <MenuManager.h>
#include <LEDManager.h>
#include <FanManager.h>



//  VARIABLES DE PREFERENCES (ALMACENAMIENTO)
Preferences preferences;

//  Variables Temperatura
float     readingSTemp      = 0;           
float     tempMax           = TEMP_MAX;  // Asignar los valores de temperatura desde config.h
String    alarmSTemp        = "off";
String    alarmReadingSTemp = "off";
int       errorsSTemp       = 0;
//Instancia de OneWire para comunicarnos con la sonda
OneWire oneWire(ONE_WIRE_BUS_PIN);  // Usar el pin del sensor de temperatura
//Le pasamos la instancia oneWire a DallasTemperature 
DallasTemperature sensors(&oneWire);
// Ventilador (opcional)
FanManager fan(FAN_PIN, TEMP_ON_FAN, TEMP_OFF_FAN); // pin, encender >35°C, apagar <30°C


//  VARIABLE LUZ
unsigned long timeLostStart = 0; //Variable para rastrear cuándo se pierde la hora
// Inicializamos la clase control luz Power
LEDPWMManager ch0(0, LIGHT_CH_0_PIN, 500, 8);  // Usar los pines definidos para los canales de luz
LEDPWMManager ch1(1, LIGHT_CH_1_PIN, 500, 8);
LEDPWMManager ch2(2, LIGHT_CH_2_PIN, 500, 8);
LEDPWMManager ch3(3, LIGHT_CH_3_PIN, 500, 8);
// Variable que recogen el instante del valor de POWER anterior en el que se aumentó el POWER (sirven para efecto amanecer/anochecer)
unsigned long previousMillisCh0 = 0; 
unsigned long previousMillisCh1 = 0;
unsigned long previousMillisCh2 = 0; 
unsigned long previousMillisCh3 = 0;
// Retraso para aumentar/disminuir en 1 el valor de POWER durante amanecer y anochecer
int sunriseDuration = 0;  // Duración en minutos del amanecer
int sundownDuration = 0;  // Duración en minutos del anochecer
//Valores máximos de POWER en funcionamiento normal.
//Los leeremos de Preferences 
byte powerCh0Max = 0; 
byte powerCh1Max = 0;
byte powerCh2Max = 0; 
byte powerCh3Max = 0; 
//Valores máximos de POWER en funcionamiento Saver (en sobrecalentamiento con alarma activa)
byte powerCh0MaxSaver = POWER_CH_0_SAVER; 
byte powerCh1MaxSaver = POWER_CH_1_SAVER;
byte powerCh2MaxSaver = POWER_CH_2_SAVER;   
byte powerCh3MaxSaver = POWER_CH_3_SAVER; 
//Valores POWER en en el momento actual
byte powerCh0 = 0;       
byte powerCh1 = 0; 
byte powerCh2 = 0;       
byte powerCh3 = 0;
//Modo de la luz
String lightMode = "auto"; // auto, cycle, on, off
//Estado de cada canal
String stateLightCh0 = "off"; // amanecer, anochecer, ahorro, on, off
String stateLightCh1 = "off"; // amanecer, anochecer, ahorro, on, off
String stateLightCh2 = "off"; // amanecer, anochecer, ahorro, on, off
String stateLightCh3 = "off"; // amanecer, anochecer, ahorro, on, off
//Hora de amanecer y anochecer. Leeremos los valores de Preferences
byte hourSunrise;
byte hourSundown;
byte minuteSunrise;
byte minuteSundown;
 // Variable que recoge el momento en el que se inicia el encendido en modo cycle
unsigned long previousMillisCycle = 0; 
bool moonMode;


//VARIABLES WiFi
boolean    wifiState        = false; // Estado de la conexión wifi       
//VARIABLES PARA LA HORA
// Instanciamos el cliente NTP
WiFiUDP       ntpUDP;
NTPClient     timeClient(ntpUDP, "pool.ntp.org", CET_OFFSET); // Usamos la hora CET por defecto
String        date                 = "null"; //fecha que extraeremos del mensaje recibido
int           hour                 = 0;
int           minute               = 0;
int           day                  = 0;
int           month                = 0;
int           year                 = 0;
int           dayOfMonth           = 0;
bool          systemInTime         = false; //determina si el sistema está en hora

//VARIABLES PANTALLA
unsigned long lastDisplayToggle = 0;
bool showAlternateDisplay = false;
const unsigned long displayInterval = 5000;  // 5 segundos

// VARIABLE WATCHDOG
#define WDT_TIMEOUT 40 // Valor en segundos que puede estar el ESP32 sin reiniciar el loop


//FUNCIÓN DEBUG PARA IMPRIMIR LOS VALORES POR LA CONSOLA
void printStatus() {
  Serial.print("Hora: ");
  if (systemInTime==false){
    Serial.println("error en el servidor NTP");
  }
  else{ 
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(" Día del mes: ");
    Serial.println(dayOfMonth);
  }
  
  Serial.print("Temperatura actual: ");
  Serial.println(readingSTemp);
  
  Serial.print("Estado de la alarma de temperatura: ");
  Serial.println(alarmSTemp);
  
  Serial.print("Estado de la alarma de sensor de temperatura: ");
  Serial.println(alarmReadingSTemp);
  
  
  Serial.print("Modo de luz: ");
  Serial.println(lightMode);
  
  Serial.print("Estado de la luz de los canales: ");
  Serial.print(stateLightCh0);
  Serial.print(" ");
  Serial.print(stateLightCh1);
  Serial.print(" ");
  Serial.print(stateLightCh2);
  Serial.print(" ");
  Serial.println(stateLightCh3);

  Serial.print("potencia actual: ");
  Serial.print(powerCh0);
  Serial.print(" ");
  Serial.print(powerCh1);
  Serial.print(" ");
  Serial.print(powerCh2);
  Serial.print(" ");
  Serial.println(powerCh3);

  Serial.print("potencia max normal: ");
  Serial.print(powerCh0Max);
  Serial.print(" ");
  Serial.print(powerCh1Max);
  Serial.print(" ");
  Serial.print(powerCh2Max);
  Serial.print(" ");
  Serial.println(powerCh3Max);

  Serial.print("Modo luna: ");
  Serial.println(moonMode);

  Serial.print("Conexion wifi: ");
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No Conectado");
  } else {
    Serial.println("Conectado");
  }

}

// FUNCIÓN PARA EL CONTROL DE LAS LUCES
void LEDPowerManegerWrapper() {  

  // Si el sistema pierde la hora y está en AUTO, iniciamos el conteo
  if (!systemInTime && lightMode == "auto") {
    if (timeLostStart == 0) {
        timeLostStart = millis(); // Registramos el tiempo de inicio de la pérdida de hora
    }
    else if (millis() - timeLostStart >= 900000) { // 900000 ms = 15 minutos
        lightMode = "cycle"; // Cambiamos a modo cycle si han pasado 15 minutos sin hora
        Serial.println("Sistema sin hora durante 15 minutos. Cambio a modo CYCLE.");
    }
  } 
  else if (systemInTime) {
      timeLostStart = 0; // Reiniciamos el temporizador si la hora se recupera
  }

  //recogemos los valores de POWER actuales
  powerCh0 = ch0.getPowerCh();
  powerCh1 = ch1.getPowerCh();
  powerCh2 = ch2.getPowerCh();
  powerCh3 = ch3.getPowerCh();

  previousMillisCh0 = ch0.update(
    lightMode, hour, minute, hourSunrise, minuteSunrise, hourSundown, minuteSundown, sunriseDuration, sundownDuration, powerCh0Max, powerCh0MaxSaver, alarmReadingSTemp, moonMode, previousMillisCh0, previousMillisCycle, stateLightCh0);
  //el modo luna solo se aplica al canal 0. al otro le pasamos un false para que no se active
  previousMillisCh1 = ch1.update(
    lightMode, hour, minute, hourSunrise, minuteSunrise, hourSundown, minuteSundown, sunriseDuration, sundownDuration, powerCh1Max, powerCh1MaxSaver, alarmReadingSTemp, false, previousMillisCh1, previousMillisCycle, stateLightCh1);

previousMillisCh2 = ch2.update(
    lightMode, hour, minute, hourSunrise, minuteSunrise, hourSundown, minuteSundown, sunriseDuration, sundownDuration, powerCh2Max, powerCh2MaxSaver, alarmReadingSTemp, false, previousMillisCh2, previousMillisCycle, stateLightCh2);

previousMillisCh3 = ch3.update(
    lightMode, hour, minute, hourSunrise, minuteSunrise, hourSundown, minuteSundown, sunriseDuration, sundownDuration, powerCh3Max, powerCh3MaxSaver, alarmReadingSTemp, false, previousMillisCh3, previousMillisCycle, stateLightCh3);

}



bool isLastSunday(int month, int dayOfMonth, int year) {
  // Calculamos el día de la semana para el último día del mes
  tm timeinfo = { 0 };
  timeinfo.tm_year = year - 1900;
  timeinfo.tm_mon = month - 1;  // Mes es 0-11
  timeinfo.tm_mday = 1; // Primer día del mes
  mktime(&timeinfo); // Llenar la estructura tm

  // Día de la semana para el primer día del mes (0 - Domingo, 1 - Lunes, etc.)
  int firstDayOfWeek = timeinfo.tm_wday;

  // El último domingo se calcula como el día del mes del último domingo
  int lastSunday = 31 - (firstDayOfWeek + 6) % 7;
  
  // Verificamos si el día actual es el último domingo del mes
  return dayOfMonth == lastSunday;
}

// Función para ajustar la hora por horario de verano (CEST)
int adjustForDST(int rawHour, int dayOfMonth, int month, int year) {
  // Verificar si estamos en horario de verano (último domingo de marzo) o en horario estándar (último domingo de octubre)
  if (month == DST_START_MONTH && isLastSunday(month, dayOfMonth, year)) {
    return rawHour + 1;  // Ajustamos la hora al horario de verano (CEST)
  } else if (month == DST_END_MONTH && isLastSunday(month, dayOfMonth, year)) {
    return rawHour - 1;  // Regresamos a horario estándar (CET)
  } else {
    return rawHour;  // Si no es el día del cambio de hora, mantenemos la hora actual
  }
}
//FUNCIÓN PARA EL CONTROL DE LA HORA
void checkTime(){
  // Nos guardamos el resultado en una variable y una vez puesto en hora no comprobamos mas.
  // la propia libreria lo comprueba por nosotros cada minuto.
  // Si no tenemos red ni lo intentamos...
  if(!systemInTime && WiFi.isConnected()) {
    Serial.println("Comprobando servidor NTP.");
    systemInTime = timeClient.update();
    if(!systemInTime) {
      systemInTime = timeClient.forceUpdate();
    }
  }
  
  timeClient.update();
  date = timeClient.getFormattedDate();
  hour = timeClient.getHours();
  minute = timeClient.getMinutes();
  day = timeClient.getDay();

  // Formato: "YYYY-MM-DD HH:MM:SS"
  year = date.substring(0, 4).toInt();
  month = date.substring(5, 7).toInt();
  dayOfMonth = date.substring(8, 10).toInt();
  
  // Ajustar la hora según el horario de verano (si corresponde)
  hour = adjustForDST(hour, dayOfMonth, month, year);
}


void checkTemperature() {
  sensors.requestTemperatures();
  readingSTemp = round(sensors.getTempCByIndex(0) * 10.0) / 10.0;  // Redondear a 1 decimal
  //si hay 5 errores del sensor consecutivos, activamos la alarma de sensor de temperatura
  if (readingSTemp == DEVICE_DISCONNECTED_C) {
    errorsSTemp++;
    if (errorsSTemp > 5) {
      alarmSTemp = "on";
    }
  } else {
 //si hay no hay error del sensor pero se supera la temperatura maxima, activamos la alarma de lectura temperatura
 //la alarma de lectura de temperatura se restablecerá manualmente
    errorsSTemp = 0;
    if (readingSTemp >= tempMax) {
      alarmReadingSTemp = "on";}
  }
}

void setupPreferences() {
  preferences.begin("datos", false); // false para modo lectura-escritura


  // Leer preferencias
  lightMode = preferences.getString("lightMode", "auto");
  powerCh0Max = preferences.getInt("powerCh0Max");
  powerCh1Max = preferences.getInt("powerCh1Max", 60);
  powerCh2Max = preferences.getInt("powerCh2Max", 70);
  powerCh3Max = preferences.getInt("powerCh3Max", 70);
  hourSunrise = preferences.getInt("hourSunrise", 10);
  minuteSunrise = preferences.getInt("minuteSunrise", 0);
  hourSundown = preferences.getInt("hourSundown", 20);
  minuteSundown = preferences.getInt("minuteSundown", 0);
  sunriseDuration = preferences.getInt("sunriseDuration", 30);
  sundownDuration = preferences.getInt("sundownDuration", 30);
  moonMode = preferences.getBool("moonMode", true);

  preferences.end();
}


void setup() {
  Serial.begin(115200);
  esp_task_wdt_init(WDT_TIMEOUT, true); // Iniciamos el watchdog
  esp_task_wdt_add(NULL); // Añadimos la tarea del loop al Watchdog

  
  
  setupWiFi();    //Iniciamos la conexión wifi
  setupDisplay(); // Iniciamos la pantalla
  setupMenuControls(); // Iniciamos el control del menu (el botón)
  setupLED();     // Iniciamos el LED indicador

  setupPreferences(); // Configura las preferencias iniciales

  sensors.begin(); // Inicializamos el sensor de temperatura

  timeClient.begin();
  timeClient.setTimeOffset(7200);

}

void loop() {
  esp_task_wdt_reset(); // Reset del watchdog en cada iteración del loop
  
  wifiState = checkWiFi();
  processMenuNavigation();  
  checkTemperature();
  checkTime();
  LEDPowerManegerWrapper();
  printStatus();
  updateLEDStatus(alarmSTemp, alarmReadingSTemp, wifiState, systemInTime);
  fan.updateFan(readingSTemp, alarmSTemp, alarmReadingSTemp);;
  // Solo actualizar la pantalla si el menú NO está activo
  // Solo actualizar la pantalla si el menú NO está activo
  if (!isMenuActive()) {
    unsigned long currentMillis = millis();
    
    // Alternar entre las dos pantallas cada `displayInterval` (5 segundos)
    if (currentMillis - lastDisplayToggle > displayInterval) {
      showAlternateDisplay = !showAlternateDisplay;
      lastDisplayToggle = currentMillis;
    }

    if (showAlternateDisplay) {
      updateDisplay1(hourSunrise, minuteSunrise, hourSundown, minuteSundown, sunriseDuration, sundownDuration, powerCh0Max, powerCh1Max, powerCh2Max, powerCh3Max);
    } else {
      updateDisplay0(lightMode, powerCh0, powerCh1, powerCh2, powerCh3, readingSTemp, hour, minute, wifiState);
    }
  }


  
}
