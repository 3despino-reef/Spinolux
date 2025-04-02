#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>

void setupButton();
void onPressed();
void onPressedForDuration();
void checkButton();
void resetShortClicks();  
void resetLongClicks();   
void resetClicks(); 
int getShortClicksCount();
int getLongClicksCount();


#endif // BUTTON_MANAGER_H
