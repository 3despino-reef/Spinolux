#include <ButtonManager.h>
#include <EasyButton.h>
#include <config.h>

//  Variables Botón
EasyButton button(BUTTON_PIN, true,200); 
byte numberLongClicks = 0;
byte numberShortClicks = 0;
extern String alarmSTemp;
extern String alarmReadingSTemp;
extern String lightMode; // auto, cycle, on

void setupButton (){
  button.begin();
  button.onPressed(onPressed);
  button.onPressedFor(3000, onPressedForDuration); //definimos la función que se ejecutará en una pulsación larga (duraciónpulsacion, callback)
}
void onPressed() {
// se ejecuta cada vez que hacemos una pulsación corta
  numberShortClicks++;
}
void onPressedForDuration() {
// se ejecuta cada vez que hacemos una pulsación larga
  numberLongClicks++;

}

void checkButton() {
    button.read();
}


void resetShortClicks() {
  numberShortClicks = 0;
}

void resetLongClicks() {
  numberLongClicks = 0;
}

void resetClicks() {
  numberLongClicks = 0;
  numberShortClicks = 0;
}

int getShortClicksCount() {
  return numberShortClicks;
}

int getLongClicksCount() {
  return numberLongClicks;
}