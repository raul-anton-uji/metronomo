#include "Interfaz.h"
#include "Config.h"

volatile int bpmActual = 120;

// Variables para el Encoder (Máquina de estados)
const int8_t TABLA_ENCODER[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};
static uint8_t estadoEncoder = 0;
static int8_t contadorPasos = 0;

// Variables para el Botón (Reset 3 segundos)
unsigned long tiempoInicioPulsacion = 0;
bool pulsadoAnteriormente = false;
const unsigned long TIEMPO_RESET = 3000; // 3000ms = 3 segundos

void setupInterfaz() {
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    pinMode(PIN_ENCODER_SW, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), checkEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT), checkEncoder, CHANGE);
}

void checkEncoder() {
    estadoEncoder <<= 2;
    if (digitalRead(PIN_ENCODER_DT))  estadoEncoder |= 0x02; 
    if (digitalRead(PIN_ENCODER_CLK)) estadoEncoder |= 0x01;
    
    int8_t movimiento = TABLA_ENCODER[estadoEncoder & 0x0F];
    
    if (movimiento != 0) {
        contadorPasos += movimiento;
        if (contadorPasos >= 4) {
            bpmActual++;
            contadorPasos = 0;
        } 
        else if (contadorPasos <= -4) {
            bpmActual--;
            contadorPasos = 0;
        }
    }

    if (bpmActual < MIN_BPM) bpmActual = MIN_BPM;
    if (bpmActual > MAX_BPM) bpmActual = MAX_BPM;
}

void gestionarBoton() {
    bool botonPulsado = (digitalRead(PIN_ENCODER_SW) == LOW);

    // Detectamos el inicio de la pulsación
    if (botonPulsado && !pulsadoAnteriormente) {
        tiempoInicioPulsacion = millis();
        pulsadoAnteriormente = true;
    }

    // Si soltamos el botón antes de los 3 segundos
    if (!botonPulsado && pulsadoAnteriormente) {
        pulsadoAnteriormente = false;
    }

    // Si se mantiene pulsado, comprobamos el tiempo
    if (botonPulsado && pulsadoAnteriormente) {
        if (millis() - tiempoInicioPulsacion >= TIEMPO_RESET) {
            bpmActual = 120; // RESET
            Serial.println(">>> RESET: 120 BPM <<<");
            
            // Esperar a que suelte el botón para no repetir el reset instantáneamente
            while(digitalRead(PIN_ENCODER_SW) == LOW);
            pulsadoAnteriormente = false;
        }
    }
}

int obtenerBPM() {
    return bpmActual;
}