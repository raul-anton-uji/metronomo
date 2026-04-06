#include "Interfaz.h"
#include "Config.h"

volatile int bpmActual = 120;

// Variables para el Encoder (Máquina de estados)
const int8_t TABLA_ENCODER[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};
static uint8_t estadoEncoder = 0;
static int8_t contadorPasos = 0;

// Variables para el Botón
unsigned long tiempoInicioPulsacion = 0;
bool pulsadoAnteriormente = false;
const unsigned long TIEMPO_RESET = 2000; // 3 segundos para reset
const unsigned long DEBOUNCE_MIN = 50;   // Tiempo mínimo para evitar ruido

void setupInterfaz() {
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    pinMode(PIN_ENCODER_SW, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), checkEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT), checkEncoder, CHANGE);
}

void checkEncoder() {
    estadoEncoder <<= 2;
    if (digitalRead(PIN_ENCODER_CLK))  estadoEncoder |= 0x02; 
    if (digitalRead(PIN_ENCODER_DT)) estadoEncoder |= 0x01;
    
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

    // El giro mantiene sus límites normales (no circular)
    if (bpmActual < MIN_BPM) bpmActual = MIN_BPM;
    if (bpmActual > MAX_BPM) bpmActual = MAX_BPM;
}

void gestionarBoton() {
    bool botonPulsado = (digitalRead(PIN_ENCODER_SW) == LOW);
    unsigned long duracion = 0;

    // 1. Detectamos cuando se empieza a pulsar
    if (botonPulsado && !pulsadoAnteriormente) {
        tiempoInicioPulsacion = millis();
        pulsadoAnteriormente = true;
    }

    // 2. Detectamos cuando se SUELTA el botón
    if (!botonPulsado && pulsadoAnteriormente) {
        duracion = millis() - tiempoInicioPulsacion;
        pulsadoAnteriormente = false;

        // Si la pulsación fue corta (menor a 3s) pero válida (>50ms)
        if (duracion < TIEMPO_RESET && duracion > DEBOUNCE_MIN) {
            bpmActual += 30;
            
            // Lógica CIRCULAR: Si pasa del máximo, vuelve al mínimo
            if (bpmActual > MAX_BPM) {
                bpmActual = MIN_BPM;
            }
            Serial.print("Pulsación corta: +30 BPM. Ahora: "); Serial.println(bpmActual);
        }
    }

    // 3. Detectamos si se alcanza el tiempo de RESET mientras sigue pulsado
    if (botonPulsado && pulsadoAnteriormente) {
        if (millis() - tiempoInicioPulsacion >= TIEMPO_RESET) {
            bpmActual = 120; // RESET
            Serial.println(">>> RESET MANTENIDO: 120 BPM <<<");
            
            // Bloqueamos hasta que suelte para no procesar una pulsación corta al soltar
            while(digitalRead(PIN_ENCODER_SW) == LOW);
            pulsadoAnteriormente = false;
        }
    }
}

int obtenerBPM() {
    return bpmActual;
}