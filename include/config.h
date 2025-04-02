#ifndef CONFIG_H
#define CONFIG_H

// Definir pines y configuraciones de hardware
#define ONE_WIRE_BUS_PIN      33  // Pin para el sensor de temperatura
#define FAN_PIN               4   // Pin para el ventilador
#define LIGHT_CH_0_PIN        5  // Pin para el canal AZUL
#define LIGHT_CH_1_PIN        18  // Pin para el canal BLANCO+ROJO+VERDE+CYAN
#define LIGHT_CH_2_PIN        19  // Pin para el canal UV
#define LIGHT_CH_3_PIN        23  // Pin para el canal AZUL REAL
#define BUTTON_PIN            12

// Variables relacionadas con la temperatura
#define TEMP_MAX              44  // Temperatura máxima para activar la alarma


// Variables relacionadas con la luz
#define POWER_CH_0_SAVER      80   // Valores máximos de POWER en funcionamiento Saver (en sobrecalentamiento con alarma activa)
#define POWER_CH_1_SAVER      40    // Valores máximos de POWER en funcionamiento Saver (en sobrecalentamiento con alarma activa)
#define POWER_CH_2_SAVER      0      // Valores máximos de POWER en funcionamiento Saver (en sobrecalentamiento con alarma activa)
#define POWER_CH_3_SAVER      20    // Valores máximos de POWER en funcionamiento Saver (en sobrecalentamiento con alarma activa)
// Variables de los LEDs indicadores           
#define BLUE_PIN              27
#define GREEN_PIN             25
#define RED_PIN               26 

// Variables de la conexión wifi
#define WIFI_SSID             "SSID";     //SUSTITUIR POR EL NOMBRE DEL WIFI      
#define WIFI_PASSWORD         "PASSWORD";  //SUSTITUIR POR LA CONTRASEÑA DEL WIFI

// Configura la zona horaria de España (CET +1 y CEST +2)
#define CET_OFFSET 3600        // UTC +1 (horario estándar)
#define CEST_OFFSET 7200      // UTC +2 (horario de verano)
#define DST_START_MONTH 3     // Marzo (inicio de horario de verano)
#define DST_END_MONTH 10      // Octubre (fin de horario de verano)

#endif // CONFIG_H
