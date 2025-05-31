#include "MenuManager.h"

// Definir los estados del menú
enum MenuState {
    MAIN_MENU,
    MODE_MENU,
    POWER_MENU,
    POWER_ADJUST,
    SUNRISE_MENU,
    SUNDOWN_MENU,
    MOON_MENU,
    SUNRISE_RAMP_ADJUST, 
    SUNDOWN_RAMP_ADJUST  
};

extern byte        powerCh0Max; 
extern byte        powerCh1Max;
extern byte        powerCh2Max;  
extern byte        powerCh3Max;
extern String      lightMode;
extern byte        hourSunrise;
extern byte        minuteSunrise;
extern byte        hourSundown;
extern byte        minuteSundown;
extern int         sunriseDuration; 
extern int         sundownDuration; 
extern boolean     moonMode;

extern Preferences preferences;
extern String      alarmSTemp;
extern String      alarmReadingSTemp;

// Variables globales del menú
bool menuActive          = false;
MenuState menuState      = MAIN_MENU;
// Variables para ajuste de canal
int selectedCh           = 0;
// Variable para ajuste de hora amanecer o anochecer
byte selectedHour        = 0;
byte selectedMinute      = 0;
byte selectedOption      = 0;
boolean firstEntry       = true;
boolean editing          = false;

// Variables de interacción del botón
unsigned long lastInteractionTime = 0;
const unsigned long menuTimeout = 20000;  // 20 segundos de inactividad
byte lastnumberShortClicks = 0;  // Guarda el último número de pulsaciones cortas

// Inicia el pulsador para controlar el menú
void setupMenuControls() {
    setupButton(); // Inicializa el botón
}

// Función para obtener el estado del menú
bool isMenuActive() {
    return menuActive;
}

//  Maneja la lógica de navegación del menú según los clics del botón
void processMenuNavigation() {
    checkButton(); // Actualiza los clics mediante EasyButton

    // Obtener el estado actual de clics de forma consistente
    int shortClicks = getShortClicksCount();
    int longClicks = getLongClicksCount();

    // Si el número de clics cortos ha cambiado, reiniciar el tiempo de inactividad
    if (shortClicks != lastnumberShortClicks) {
        lastInteractionTime = millis();
        lastnumberShortClicks = shortClicks;
    }

    // Si pulsación larga y no estamos en el menú → activamos menú
    if (longClicks > 0 && !menuActive) {
        menuActive = true;
        menuState = MAIN_MENU;
        lastInteractionTime = millis();
        editing = false;

        resetClicks(); // limpiamos para evitar heredar clics
        return; // importante: evitamos procesar clicks residuales y entrar a menuActive sin haber vuelto a leer los clicks
    }

    // Si ya estamos en el menú, procesamos con los valores actuales
    if (menuActive) {
        handleMenu(shortClicks, longClicks);
        checkMenuTimeout();
    }
}

//  Verifica si el menú debe cerrarse por inactividad
void checkMenuTimeout() {
    if (millis() - lastInteractionTime > menuTimeout) {
        Serial.println("Menú cerrado por inactividad");
        menuActive = false;
        resetClicks();
        firstEntry = true;
    }
}

// Manejo de la navegación del menú
void handleMenu(byte numberShortClicks, byte numberLongClicks) {
    switch (menuState) {
        case MAIN_MENU: handleMainMenu(getShortClicksCount(), getLongClicksCount()); break;
        case MODE_MENU: handleModeMenu(getShortClicksCount(), getLongClicksCount()); break;
        case POWER_MENU: handlePowerMenu(getShortClicksCount(), getLongClicksCount()); break;
        case POWER_ADJUST: handlePowerAdjust(getShortClicksCount(), getLongClicksCount()); break;
        case SUNRISE_MENU: handleSunriseAdjust(getShortClicksCount(), getLongClicksCount()); break;
        case SUNDOWN_MENU:  handleSundownAdjust(getShortClicksCount(), getLongClicksCount()); break;
        case SUNRISE_RAMP_ADJUST: handleSunriseRampAdjust(getShortClicksCount(), getLongClicksCount()); break; 
        case SUNDOWN_RAMP_ADJUST: handleSundownRampAdjust(getShortClicksCount(), getLongClicksCount()); break;
        case MOON_MENU: handleMoonMode(getShortClicksCount(), getLongClicksCount()); break;
    }
}

// Manejo del menú principal
void handleMainMenu(byte numberShortClicks, byte numberLongClicks) {
    String options []= {"Modo", "Reset Alarmas", "Canales", "Hora amanecer", "Hora anochecer", "Modo luna", "Rampa Amanecer", "Rampa Anochecer", "Salir"};
    byte numberOptions = sizeof(options) / sizeof(options[0]); // Calculamos el tamaño ANTES
    showMenuCarrusel(options, numberOptions, numberShortClicks);
    if (numberLongClicks == 1) {
        switch (numberShortClicks) {
            case 0: menuState = MODE_MENU; break;
            case 1: 
                alarmReadingSTemp = "off";
                alarmSTemp = "off";
                showMessageSave();
                menuActive = false;
                break;
            case 2: menuState = POWER_MENU; break;
            case 3: menuState = SUNRISE_MENU; break;
            case 4: menuState = SUNDOWN_MENU; break;
            case 5: menuState = MOON_MENU; break;
            case 6: menuState = SUNRISE_RAMP_ADJUST; break; 
            case 7: menuState = SUNDOWN_RAMP_ADJUST; break; 
        }
        resetClicks();
        return; // Salimos para no seguir usando los clics ya reseteados
    }
    if (numberShortClicks >= numberOptions) resetShortClicks();
}

// Manejo del menú de modo de luz
void handleModeMenu(byte numberShortClicks, byte numberLongClicks) {
    String options [] = {"On", "Auto", "Cycle", "Off", "Salir"};
    byte numberOptions = sizeof(options) / sizeof(options[0]); // Calculamos el tamaño ANTES
    showMenuCarrusel(options, numberOptions, numberShortClicks);

    if (numberLongClicks == 1) {
        preferences.begin("datos", false);
        switch (numberShortClicks) {
            case 0: 
                lightMode = "on"; 
                preferences.putString("lightMode", lightMode);
                break;
            case 1: 
                lightMode = "auto"; 
                preferences.putString("lightMode", lightMode);
                break;
            case 2: 
                lightMode = "cycle"; 
                preferences.putString("lightMode", lightMode);
                break;
            case 3: 
                lightMode = "off";
                preferences.putString("lightMode", lightMode); 
                break;
        }
        preferences.end();
        resetClicks();
        showMessageSave();
        menuActive = false; // Cerrar menú
        return; // Evitamos seguir usando clicks ya reseteados
    }

    if (numberShortClicks >= numberOptions) resetShortClicks();
}

// Manejo del menú de selección de potencia
void handlePowerMenu(byte numberShortClicks, byte numberLongClicks) {
    String options [] = {"Ch0", "Ch1", "Ch2", "Ch3", "Salir"};
    byte numberOptions = sizeof(options) / sizeof(options[0]); // Calculamos el tamaño ANTES
    showMenuCarrusel(options, numberOptions, numberShortClicks);
    if (numberLongClicks == 1) {
        if (numberShortClicks <= 3) {
            selectedCh = numberShortClicks;
            menuState = POWER_ADJUST;
        }
        resetClicks();
        return; // Evitamos seguir usando clicks ya reseteados
    }
    if (numberShortClicks >= numberOptions) resetShortClicks();
}

// Manejo del ajuste de potencia
void handlePowerAdjust(byte numberShortClicks, byte numberLongClicks) {
    showPowerAdjust(numberShortClicks, selectedCh);
    if (numberShortClicks >= 21) resetShortClicks();

    if (numberLongClicks == 1) {
        preferences.begin("datos", false);
        int powerValue = numberShortClicks * 5;

        switch (selectedCh) {
            case 0: preferences.putInt("powerCh0Max", powerValue); powerCh0Max = powerValue; break;
            case 1: preferences.putInt("powerCh1Max", powerValue); powerCh1Max = powerValue; break;
            case 2: preferences.putInt("powerCh2Max", powerValue); powerCh2Max = powerValue; break;
            case 3: preferences.putInt("powerCh3Max", powerValue); powerCh3Max = powerValue; break;
        }

        preferences.end();
        showMessageSave();
        menuActive = false; // Cerrar menú
        resetClicks();
        return; // Evitamos seguir usando clicks ya reseteados
    }
}

void handleSunriseAdjust(byte numberShortClicks, byte numberLongClicks) {
     //cuando entramos, nos traemos los valores de la hora guardada en memoria 
    if (firstEntry) {
        selectedHour = hourSunrise;
        selectedMinute = minuteSunrise;
        firstEntry = false;
    }
    showTimeAdjust(selectedHour, selectedMinute, selectedOption, editing);
    //cuando entramos, nos traemos los valores de la hora guardada en memoria 

    //si no estamos editando ningún campo:
    if (!editing) {
        // Navegar entre Hora → Minuto → Confirmar → Cancelar
        selectedOption = numberShortClicks;
        //si hacemos una pulsación larga, entramos en edición
        if (numberLongClicks == 1) {
            editing=true;
            resetClicks();
        }
        if (numberShortClicks >=4) {
            resetShortClicks();
            // nos volvemos a traer los valores de conteo de clicks ya reseteados
            numberShortClicks = getShortClicksCount();
        }
            
    } 
    //si hemos seleccionado un campo:
    else {
        switch (selectedOption) {
            case 0: 
                //estamos editando las horas
                //la hora de encendido será la guardada actualmente + el número de clicks 
                selectedHour = selectedHour + numberShortClicks;
                if (selectedHour >=24) hourSunrise = 0;
                if (numberLongClicks == 1) {
                    //hemos hecho una pulsación larga y salimos de la edición
                    editing=false;
                    resetClicks();
                }
                break;
            case 1: 
                //estamos editando los minutos. Vamos de 5 en 5
                //el minuto de encendido será el guardado actualmente + el número de clicks 
                selectedMinute = selectedMinute + numberShortClicks*5;
                if (selectedMinute >=60) selectedMinute = 0;
                if (numberLongClicks == 1) {
                    //hemos hecho una pulsación larga y salimos de la edición
                    editing=false;
                    resetClicks();
                }
                break;
            case 2: //le hemos dado a guardar
                preferences.begin("datos", false);
                preferences.putInt("hourSunrise", selectedHour); 
                preferences.putInt("minuteSunrise", selectedMinute);
                preferences.end();
                hourSunrise = selectedHour;
                minuteSunrise = selectedMinute;
                resetClicks();
                menuActive = false; //salimos del menu
                firstEntry = true;
                showMessageSave();
                break;
            case 3: 
                //nos salimos sin hacer nada
                resetClicks();
                menuActive = false;
                firstEntry = true;
            break;
        }
    }
}

void handleSundownAdjust(byte numberShortClicks, byte numberLongClicks) {
    // Cuando entramos, traemos los valores de la hora guardada en memoria
    if (firstEntry) {
        selectedHour = hourSundown;
        selectedMinute = minuteSundown;
        firstEntry = false;
    }
    
    showTimeAdjust(selectedHour, selectedMinute, selectedOption, editing);

    // Si no estamos editando ningún campo:
    if (!editing) {
        // Navegar entre Hora → Minuto → Confirmar → Cancelar
        selectedOption = numberShortClicks;
        // Si hacemos una pulsación larga, entramos en edición
        if (numberLongClicks == 1) {
            editing = true;
            resetClicks();
        }
        if (numberShortClicks >= 4){
            // nos volvemos a traer los valores de conteo de clicks ya reseteados
            numberShortClicks = getShortClicksCount();
            resetShortClicks();
        } 
    } 
    // Si hemos seleccionado un campo:
    else {
        switch (selectedOption) {
            case 0: 
                // Editando las horas
                // La hora de apagado será la guardada actualmente + el número de clicks 
                selectedHour = selectedHour + numberShortClicks;
                if (selectedHour >= 24) selectedHour = 0;
                if (numberLongClicks == 1) {
                    // Pulsación larga: salimos de la edición
                    editing = false;
                    resetClicks();
                }
                break;
            case 1: 
                // Editando los minutos. Vamos de 5 en 5
                // El minuto de apagado será el guardado actualmente + el número de clicks 
                selectedMinute = selectedMinute + numberShortClicks * 5;
                if (selectedMinute >= 60) selectedMinute = 0;
                if (numberLongClicks == 1) {
                    // Pulsación larga: salimos de la edición
                    editing = false;
                    resetClicks();
                }
                break;
            case 2: // Guardar los cambios
                preferences.begin("datos", false);
                preferences.putInt("hourSundown", selectedHour); 
                preferences.putInt("minuteSundown", selectedMinute);
                preferences.end();
                hourSundown = selectedHour;
                minuteSundown = selectedMinute;
                resetClicks();
                firstEntry = true;
                menuActive = false; // Salimos del menú
                showMessageSave();
                break;
            case 3: 
                // Cancelar y salir sin hacer cambios
                resetClicks();
                firstEntry = true;
                menuActive = false;
                break;
        }
    }
}

void handleSunriseRampAdjust(byte numberShortClicks, byte numberLongClicks) {
    static byte selectedOption = 0;  // Variable persistente dentro de la función

    // Nos traemos el valor guardado al entrar, dividiendo en pasos de 5 minutos
    if (firstEntry) {
        selectedOption = sunriseDuration / 5;  // Convertir a unidades de "5 minutos"
        firstEntry = false;
    }

    // Actualizamos la interfaz con el valor actual en minutos
    showRampAdjust(selectedOption * 5, "Amanecer");

    // Si hay una pulsación corta, aumentamos en pasos de 5 minutos
    if (numberShortClicks > 0) {
        selectedOption += numberShortClicks;  // Incrementamos en pasos de 5 minutos
        if (selectedOption * 5 > 120) { // Máximo de 120 minutos
            selectedOption = 120 / 5;
        }
        resetShortClicks(); // Limpiamos el contador de clics para la próxima iteración
    }

    // Guardar el valor cuando se hace una pulsación larga
    if (numberLongClicks == 1) {
        preferences.begin("datos", false);
        preferences.putInt("sunriseDuration", selectedOption * 5);  // Guardar en minutos reales
        preferences.end();

        sunriseDuration = selectedOption * 5;  // Aplicar el nuevo valor
        showMessageSave();

        menuActive = false;
        firstEntry = true;
        resetClicks();
    }
}



void handleSundownRampAdjust(byte numberShortClicks, byte numberLongClicks) {
    static byte selectedOption = 0;  // Variable persistente dentro de la función

    // Nos traemos el valor guardado al entrar, dividiendo en pasos de 5 minutos
    if (firstEntry) {
        selectedOption = sundownDuration / 5;  // Convertir a unidades de "5 minutos"
        firstEntry = false;
    }

    // Actualizamos la interfaz con el valor actual en minutos
    showRampAdjust(selectedOption * 5, "Anochecer");

    // Si hay una pulsación corta, aumentamos en pasos de 5 minutos
    if (numberShortClicks > 0) {
        selectedOption += numberShortClicks;  // Incrementamos en pasos de 5 minutos
        if (selectedOption * 5 > 120) { // Máximo de 120 minutos
            selectedOption = 120 / 5;
        }
        resetShortClicks(); // Limpiamos el contador de clics para la próxima iteración
    }

    // Guardar el valor cuando se hace una pulsación larga
    if (numberLongClicks == 1) {
        preferences.begin("datos", false);
        preferences.putInt("sundownDuration", selectedOption * 5);  // Guardar en minutos reales
        preferences.end();

        sundownDuration = selectedOption * 5;  // Aplicar el nuevo valor
        showMessageSave();

        menuActive = false;
        firstEntry = true;
        resetClicks();
    }
}


void handleMoonMode(byte numberShortClicks, byte numberLongClicks) {
    String options[] = {"Off", "On", "Salir"};
    byte numberOptions = sizeof(options) / sizeof(options[0]);
    showMenuCarrusel(options, numberOptions, numberShortClicks);

    if (numberLongClicks == 1) {
        preferences.begin("datos", false);
        switch (numberShortClicks) {
            case 0: 
                moonMode = "off"; 
                preferences.putBool("moonMode", moonMode);
                break;
            case 1: 
                moonMode = "on"; 
                preferences.putBool("moonMode", moonMode);
                break;
            case 2: 
                menuActive = false; // Salir sin cambios
                resetClicks();
                return;
        }
        preferences.end();
        showMessageSave();
        menuActive = false;
        resetClicks();
        return; // Evitamos seguir usando clicks ya reseteados
    }
    if (numberShortClicks >= numberOptions) resetShortClicks();
}




  


   

