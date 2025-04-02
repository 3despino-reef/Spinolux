#include "DisplayManager.h"

// Crear objeto de la pantalla OLED
Adafruit_SSD1306 display(WIDTH_DISPLAY, HEIGHT_DISPLAY, &Wire, -1);

// Configuración inicial de la pantalla
void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error al inicializar la pantalla OLED.");
    //salimos de la función si hay un error con la pantalla
    return;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

// Actualizar la pantalla con los datos actuales
void updateDisplay0(String lightMode, byte powerCh0, byte powerCh1, byte powerCh2, byte powerCh3, float readingSTemp, int hour, int minute, boolean wifiState) {

  display.clearDisplay();// limpia pantalla
  display.setCursor (0, 0);	
  display.print(hour); 	 
  display.print(":"); 	 
  display.print(minute); 	 

  display.setCursor (50, 0);		
  display.print("Wifi"); 
  if (wifiState == true) {
    display.print("OK"); 	
  }
  else{ display.print("ERROR"); }

  display.setCursor (5, 12);		
  display.print("Ch azul ------- "); 	
  display.print(powerCh0); 	
  display.print("%"); 
  display.setCursor (5, 23);		
  display.print("Ch blanco+G+R - "); 
  display.print(powerCh1); 
  display.print("%"); 	 
  display.setCursor (5, 34);		
  display.print("Ch UV --------- "); 	 
  display.print(powerCh2); 
  display.print("%"); 	  
  display.setCursor (5, 45);		
  display.print("Ch azul real -- "); 	 
  display.print(powerCh3); 	 	
  display.print("%");  
  display.setCursor (90, 57);	
  display.print(readingSTemp); 	 
  display.print("C"); 	 
  display.setCursor (0, 57);		
  display.print("Modo:"); 	 
  display.print(lightMode); 	 
  display.display();			// muestra en pantalla todo lo establecido anteriormente

}

void updateDisplay1(byte hourSunrise, byte minuteSunrise, byte hourSundown, byte minuteSundown, 
  int sunriseDuration, int sundownDuration, 
  byte powerCh0Max, byte powerCh1Max, byte powerCh2Max, byte powerCh3Max) { 
display.clearDisplay(); // Limpia pantalla

// Hora de encendido (Amanecer)
display.setCursor(0, 0);
display.print("Amanece a las ");
if (hourSunrise < 10) display.print("0"); 
display.print(hourSunrise);
display.print(":");
if (minuteSunrise < 10) display.print("0");
display.print(minuteSunrise);

// Hora de apagado (Anochecer)
display.setCursor(0, 12);
display.print("Anochece a las ");
if (hourSundown < 10) display.print("0"); 
display.print(hourSundown);
display.print(":");
if (minuteSundown < 10) display.print("0");
display.print(minuteSundown);

// Duración amanecer
display.setCursor(0, 24);
display.print("Rampas: ");
display.print(sunriseDuration);
display.print(" min/");
display.print(sundownDuration);
display.print(" min");

// Potencia máxima de los canales
display.setCursor(5, 40);
display.print("CH0: "); display.print(powerCh0Max); display.print("% ");
display.print("CH1: "); display.print(powerCh1Max); display.print("%");

display.setCursor(5, 52);
display.print("CH2: "); display.print(powerCh2Max); display.print("% ");
display.print("CH3: "); display.print(powerCh3Max); display.print("%");

display.display(); // Muestra en pantalla todo lo establecido anteriormente
}



void showMenuCarrusel(String options[], byte numberOptions, byte selectedOption) {
  display.clearDisplay();
  // Manejo de los elementos anterior, actual y siguiente con límites circulares
  String previous = (selectedOption == 0) ? options[numberOptions - 1] : options[selectedOption - 1];
  String current  = options[selectedOption];
  String next     = (selectedOption == numberOptions - 1) ? options[0] : options[selectedOption + 1];

  // Mostrar el elemento anterior
  display.setCursor(10, 10);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print(previous);

  // Resaltar el elemento actual
  display.drawRect(5, 25, 110, 15, SSD1306_WHITE); 
  display.setCursor(10, 30);
  display.setTextSize(1);
  display.print(current);

  // Mostrar el siguiente elemento
  display.setCursor(10, 50);
  display.print(next);

  // Dibujar barra de desplazamiento lateral
  drawScrollbar(selectedOption, numberOptions);

  display.display();
}
void drawScrollbar(byte selectedOption, byte numberOptions) {
  byte barHeight = HEIGHT_DISPLAY / numberOptions;
  byte barY = selectedOption * barHeight;

  // Dibujar el fondo de la barra
  display.drawRect(120, 0, 7, HEIGHT_DISPLAY, SSD1306_WHITE);
  display.fillRect(121, barY, 5, barHeight, SSD1306_WHITE);
}

void showPowerAdjust(byte selectedOption, byte selectedCh) {
  display.clearDisplay();
  display.setCursor(5, 5);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("Canal ");
  display.print(selectedCh);

  // Dibujar la barra base (vacía)
  byte barX = 10;  // Posición X de la barra
  byte barY = 30;  // Posición Y de la barra
  byte barWidth = 80;  // Ancho total de la barra
  byte barHeight = 10; // Altura de la barra

  display.drawRect(barX, barY, barWidth, barHeight, SSD1306_WHITE); // Marco de la barra

  // Llenar la barra proporcionalmente (cada unidad en selectedOption representa 5%)
  byte fillWidth = (selectedOption * barWidth) / 20; // Escalamos a la barra (20 = 100%)
  display.fillRect(barX, barY, fillWidth, barHeight, SSD1306_WHITE);

  // Mostrar el porcentaje a la derecha de la barra
  display.setCursor(barX + barWidth + 10, barY + 2); // Posicionamos a la derecha
  display.print(selectedOption * 5); // Convertimos a porcentaje
  display.print("%");

  display.display();
}


void showTimeAdjust(byte hour, byte minute, byte selectedOption, bool editing) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Título del menú
  display.setCursor(5, 5);
  display.print("Seleccione la hora");

  // Posiciones de los elementos en la pantalla
  byte hourX = 30, hourY = 30;
  byte minuteX = 70, minuteY = 30;
  byte confirmX = 10, confirmY = 50;
  byte cancelX = 75, cancelY = 50;

  // Dibujar recuadro en la opción seleccionada
  if (selectedOption == 0) display.drawRect(hourX - 5, hourY - 5, 30, 15, SSD1306_WHITE);
  if (selectedOption == 1) display.drawRect(minuteX - 5, minuteY - 5, 30, 15, SSD1306_WHITE);
  if (selectedOption == 2) display.drawRect(confirmX - 5, confirmY - 5, 65, 15, SSD1306_WHITE);
  if (selectedOption == 3) display.drawRect(cancelX - 5, cancelY - 5, 60, 15, SSD1306_WHITE);

  // Si está en modo edición, poner fondo blanco y texto negro
  if (editing && selectedOption == 0) display.fillRect(hourX - 5, hourY - 5, 30, 15, SSD1306_WHITE);
  if (editing && selectedOption == 1) display.fillRect(minuteX - 5, minuteY - 5, 30, 15, SSD1306_WHITE);

  // Mostrar hora
  display.setCursor(hourX, hourY);
  if (editing && selectedOption == 0) display.setTextColor(SSD1306_BLACK); // Texto negro si editando
  if (hour < 10) display.print("0"); // Garantizar formato 2 dígitos
  display.print(hour);
  display.setTextColor(SSD1306_WHITE); // Restaurar color normal

  // Separador ":"
  display.setCursor(58, hourY);
  display.print(":");

  // Mostrar minutos
  display.setCursor(minuteX, minuteY);
  if (editing && selectedOption == 1) display.setTextColor(SSD1306_BLACK);
  if (minute < 10) display.print("0");
  display.print(minute);
  display.setTextColor(SSD1306_WHITE);

  // Opciones de Guardar y Cancelar
  display.setCursor(confirmX, confirmY);
  display.print("Guardar");

  display.setCursor(cancelX, cancelY);
  display.print("Cancelar");

  display.display();
}


void showMessageSave() { 
  display.clearDisplay();

  // Texto de guardado
  display.setCursor(20, 10);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("Guardando...");

  // Definir posiciones para la barra y el icono
  int saveIconWidth = 20;
  int saveIconHeight = 20;
  int saveIconX = 90; // Posición del disquete en X
  int saveIconY = 25;
  int progressBarX = 10; // Inicio de la barra
  int progressBarWidth = saveIconX - progressBarX - 5; // Ajustamos el ancho para no tocar el disquete
  int progressBarHeight = 4; // Ajuste fino del grosor
  
  // Centrar verticalmente la barra con el icono
  int progressBarY = saveIconY + (saveIconHeight - progressBarHeight) / 2;

  // Animación de icono de guardado
  for (int i = 0; i <= 5; i++) {
      int fillWidth = (progressBarWidth / 5) * i; // Incremento de la barra

      display.clearDisplay(); // Limpiar para evitar superposiciones
      display.setCursor(20, 10);
      display.print("Guardando...");

      drawSaveIcon(saveIconX, saveIconY); // Dibujar icono de disquete
      display.fillRect(progressBarX, progressBarY, fillWidth, progressBarHeight, SSD1306_WHITE); // Barra de progreso
      display.display();
      delay(200); // Pausa para animación
  }

  delay(500); // Espera antes de limpiar pantalla
  display.clearDisplay();
  display.display();
}

// Función para dibujar el icono de disquete
void drawSaveIcon(int x, int y) {
  // Cuadrado exterior del disquete
  display.drawRect(x, y, 20, 20, SSD1306_WHITE);

  // Cuadrado interno simulando la etiqueta del disquete
  display.fillRect(x + 4, y + 4, 12, 6, SSD1306_WHITE);

  // Zona de la muesca de guardado
  display.fillRect(x + 6, y + 15, 8, 3, SSD1306_WHITE); // Ajuste de la muesca
}

void showRampAdjust(byte selectedOption, String rampType) {
  display.clearDisplay();
  display.setCursor(5, 5);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("Ajuste ");
  display.print(rampType); // Muestra "Amanecer" o "Anochecer"

  // Dibujar la barra base (vacía)
  byte barX = 10;  // Posición X de la barra
  byte barY = 30;  // Posición Y de la barra
  byte barWidth = 80;  // Ancho total de la barra
  byte barHeight = 10; // Altura de la barra

  display.drawRect(barX, barY, barWidth, barHeight, SSD1306_WHITE); // Marco de la barra

  // Llenar la barra proporcionalmente (máximo 120 minutos)
  byte fillWidth = (selectedOption * barWidth) / 120; // Escalamos a la barra
  display.fillRect(barX, barY, fillWidth, barHeight, SSD1306_WHITE);

  // Mostrar el tiempo en minutos a la derecha de la barra
  display.setCursor(barX + barWidth + 3, barY + 2); 
  display.print(selectedOption); // Minutos
  display.print("min");

  display.display();
}
