#include "Interfaz.h"
#include "Config.h"

volatile int bpmActual = 120;
unsigned long ultimaPulsacion = 0;
const int debounceDelay = 150; 

// Variables para el filtro de giro
volatile int ultimoEstadoCLK;

void setupInterfaz() {
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    pinMode(PIN_ENCODER_SW, INPUT_PULLUP);
    
    // Leemos el estado inicial para tener una referencia
    ultimoEstadoCLK = digitalRead(PIN_ENCODER_CLK);
    
    // Usamos CHANGE para una lectura más precisa, pero con filtro
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), checkEncoder, CHANGE);
}

void checkEncoder() {
    int estadoCLK = digitalRead(PIN_ENCODER_CLK);

    // Solo actuamos si el estado ha cambiado (filtro de ruido)
    if (estadoCLK != ultimoEstadoCLK) {
        // Al bajar el CLK (flanco de bajada), miramos el DT para la dirección
        if (estadoCLK == LOW) {
            if (digitalRead(PIN_ENCODER_DT) == HIGH) {
                bpmActual++; // Sentido horario
            } else {
                bpmActual--; // Sentido antihorario
            }
        }
    }
    
    ultimoEstadoCLK = estadoCLK;
    
    // Mantener dentro de los límites
    if (bpmActual < MIN_BPM) bpmActual = MIN_BPM;
    if (bpmActual > MAX_BPM) bpmActual = MAX_BPM;
}

bool botonPresionado() {
    if (digitalRead(PIN_ENCODER_SW) == LOW) {
        if (millis() - ultimaPulsacion > debounceDelay) {
            ultimaPulsacion = millis();
            return true;
        }
    }
    return false;
}

int obtenerBPM() {
    return bpmActual;
}