#include "Interfaz.h"
#include "Config.h"

volatile int bpmActual = 120;
bool ejecutando = true; 

// --- Variables Encoder ---
const int8_t TABLA_ENCODER[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};
static uint8_t estadoEncoder = 0;
static int8_t contadorPasos = 0;

// --- Variables Botones ---
unsigned long tiempoInicioPulsacion = 0;
bool pulsadoAnteriormente = false;
bool tapAnteriorEstado = false;
bool compasPresionadoActualmente = false;
bool ssPresionadoActualmente = false; 

unsigned long ultimoCambioCompas = 0;
unsigned long ultimoTap = 0;
unsigned long tiemposTap[4] = {0, 0, 0, 0};
int indiceTap = 0;
int indiceCompas = 2;
const char* listaCompases[] = {"2/4", "3/4", "4/4"};

void setupInterfaz() {
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    pinMode(PIN_ENCODER_SW, INPUT_PULLUP);
    pinMode(PIN_TAP_TEMPO, INPUT_PULLUP);
    pinMode(PIN_COMPAS, INPUT_PULLUP);
    pinMode(PIN_START_STOP, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), checkEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT), checkEncoder, CHANGE);
}

void checkEncoder() {
    if (!ejecutando) return; // Bloqueo si está en STOP

    estadoEncoder <<= 2;
    if (digitalRead(PIN_ENCODER_CLK)) estadoEncoder |= 0x02; 
    if (digitalRead(PIN_ENCODER_DT))  estadoEncoder |= 0x01;
    int8_t movimiento = TABLA_ENCODER[estadoEncoder & 0x0F];
    
    if (movimiento != 0) {
        contadorPasos += movimiento;
        if (contadorPasos >= 4) { bpmActual++; contadorPasos = 0; } 
        else if (contadorPasos <= -4) { bpmActual--; contadorPasos = 0; }
    }
    // Aplicar límites
    if (bpmActual < MIN_BPM) bpmActual = MIN_BPM;
    if (bpmActual > MAX_BPM) bpmActual = MAX_BPM;
}

void gestionarStartStop() {
    bool botonSS = (digitalRead(PIN_START_STOP) == LOW);
    if (botonSS && !ssPresionadoActualmente) ssPresionadoActualmente = true;
    if (!botonSS && ssPresionadoActualmente) {
        ejecutando = !ejecutando;
        Serial.println(ejecutando ? ">>> SISTEMA: START" : ">>> SISTEMA: STOP");
        ssPresionadoActualmente = false;
    }
}

void gestionarBoton() {
    if (!ejecutando) return; 
    bool botonPulsado = (digitalRead(PIN_ENCODER_SW) == LOW);

    if (botonPulsado && !pulsadoAnteriormente) {
        tiempoInicioPulsacion = millis();
        pulsadoAnteriormente = true;
    }

    if (botonPulsado && pulsadoAnteriormente) {
        if (millis() - tiempoInicioPulsacion >= 3000) {
            bpmActual = 120;
            Serial.println(">>> ACCIÓN: RESET A 120 BPM <<<");
            while(digitalRead(PIN_ENCODER_SW) == LOW);
            pulsadoAnteriormente = false;
            return;
        }
    }

    if (!botonPulsado && pulsadoAnteriormente) {
        unsigned long duracion = millis() - tiempoInicioPulsacion;
        pulsadoAnteriormente = false;
        if (duracion > 50) {
            bpmActual += 30;
            if (bpmActual > MAX_BPM) bpmActual = MIN_BPM;
            Serial.println(">>> ACCIÓN: SALTO +30 BPM <<<");
        }
    }
}

void gestionarTapTempo() {
    if (!ejecutando) return; 
    bool tapPulsado = (digitalRead(PIN_TAP_TEMPO) == LOW);
    unsigned long ahora = millis();

    if (tapPulsado && !tapAnteriorEstado && (ahora - ultimoTap > 150)) {
        unsigned long diferencia = ahora - ultimoTap;
        if (diferencia < 2000) {
            tiemposTap[indiceTap] = diferencia;
            indiceTap = (indiceTap + 1) % 4;
            unsigned long suma = 0; int v = 0;
            for(int i=0; i<4; i++) { if(tiemposTap[i]>0){suma+=tiemposTap[i]; v++;} }
            
            if (v > 0) { 
                bpmActual = 60000 / (suma / v); 
                // BLOQUEO EN MAX_BPM
                if (bpmActual > MAX_BPM) bpmActual = MAX_BPM;
                if (bpmActual < MIN_BPM) bpmActual = MIN_BPM;
            }
        } else { 
            indiceTap = 0; 
            for(int i=0; i<4; i++) tiemposTap[i] = 0; 
        }
        ultimoTap = ahora;
    }
    tapAnteriorEstado = tapPulsado;
}

void gestionarCompas() {
    if (!ejecutando) return; 
    bool botonC = (digitalRead(PIN_COMPAS) == LOW);
    if (botonC && !compasPresionadoActualmente) compasPresionadoActualmente = true;
    if (!botonC && compasPresionadoActualmente) {
        indiceCompas = (indiceCompas + 1) % 3;
        Serial.print(">>> COMPÁS: "); Serial.println(listaCompases[indiceCompas]);
        compasPresionadoActualmente = false;
    }
}

bool estaEjecutando() { return ejecutando; }
int obtenerBPM() { return bpmActual; }
String obtenerCompasActual() { return String(listaCompases[indiceCompas]); }