#include "Interfaz.h"
#include "Config.h"

volatile int bpmActual = 120;
unsigned long ultimaPulsacion = 0;
const int debounceDelay = 500; 

// Máquina de estados: 0, 1 (derecha), -1 (izquierda)
const int8_t TABLA_ENCODER[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};
static uint8_t estadoEncoder = 0;
static int8_t contadorPasos = 0; // Para contar los 4 micro-pasos por clic

void setupInterfaz() {
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    pinMode(PIN_ENCODER_SW, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), checkEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT), checkEncoder, CHANGE);
}

void checkEncoder() {
    // 1. Leemos los pines. 
    // He invertido el orden de CLK y DT aquí para corregir el giro "al revés"
    estadoEncoder <<= 2;
    if (digitalRead(PIN_ENCODER_DT))  estadoEncoder |= 0x02; 
    if (digitalRead(PIN_ENCODER_CLK)) estadoEncoder |= 0x01;
    
    // 2. Buscamos el movimiento en la tabla
    int8_t movimiento = TABLA_ENCODER[estadoEncoder & 0x0F];
    
    if (movimiento != 0) {
        contadorPasos += movimiento;

        // 3. Solo cuando acumulamos 4 micro-pasos (un clic físico completo)
        // actualizamos el BPM de 1 en 1.
        if (contadorPasos >= 4) {
            bpmActual++;
            contadorPasos = 0;
        } 
        else if (contadorPasos <= -4) {
            bpmActual--;
            contadorPasos = 0;
        }
    }

    // 4. Límites
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