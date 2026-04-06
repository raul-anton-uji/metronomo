#include "Interfaz.h"
#include "Config.h"

volatile int bpmActual = 120;

// --- Variables Encoder (Giro) ---
const int8_t TABLA_ENCODER[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};
static uint8_t estadoEncoder = 0;
static int8_t contadorPasos = 0;

// --- Variables Botón Encoder (Pin 4: Reset/Salto) ---
unsigned long tiempoInicioPulsacion = 0;
bool pulsadoAnteriormente = false;
const unsigned long TIEMPO_RESET = 2000;
const unsigned long DEBOUNCE_BOTON = 50;

// --- Variables Tap Tempo (Pin 5) ---
unsigned long ultimoTap = 0;
unsigned long tiemposTap[4] = {0, 0, 0, 0};
int indiceTap = 0;
bool tapAnteriorEstado = false;
const unsigned long DEBOUNCE_TAP = 150; 

// --- Variables Compás (Pin 6) ---
const char* listaCompases[] = {"2/4", "3/4", "4/4"};
int indiceCompas = 2; // Inicia en 4/4
bool compasAnteriorEstado = false;
unsigned long ultimoCambioCompas = 0;

void setupInterfaz() {
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    pinMode(PIN_ENCODER_SW, INPUT_PULLUP);
    pinMode(PIN_TAP_TEMPO, INPUT_PULLUP);
    pinMode(PIN_COMPAS, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), checkEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT), checkEncoder, CHANGE);
}

void checkEncoder() {
    estadoEncoder <<= 2;
    // Giro: CLK en bit alto, DT en bajo para que Derecha = ++
    if (digitalRead(PIN_ENCODER_CLK)) estadoEncoder |= 0x02; 
    if (digitalRead(PIN_ENCODER_DT))  estadoEncoder |= 0x01;
    
    int8_t movimiento = TABLA_ENCODER[estadoEncoder & 0x0F];
    if (movimiento != 0) {
        contadorPasos += movimiento;
        if (contadorPasos >= 4) { bpmActual++; contadorPasos = 0; } 
        else if (contadorPasos <= -4) { bpmActual--; contadorPasos = 0; }
    }
    if (bpmActual < MIN_BPM) bpmActual = MIN_BPM;
    if (bpmActual > MAX_BPM) bpmActual = MAX_BPM;
}

void gestionarBoton() {
    bool botonPulsado = (digitalRead(PIN_ENCODER_SW) == LOW);

    if (botonPulsado && !pulsadoAnteriormente) {
        tiempoInicioPulsacion = millis();
        pulsadoAnteriormente = true;
    }

    if (botonPulsado && pulsadoAnteriormente) {
        if (millis() - tiempoInicioPulsacion >= TIEMPO_RESET) {
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
        if (duracion > DEBOUNCE_BOTON) {
            bpmActual += 30;
            if (bpmActual > MAX_BPM) bpmActual = MIN_BPM;
            Serial.println(">>> ACCIÓN: SALTO +30 BPM <<<");
        }
    }
}

void gestionarTapTempo() {
    bool tapPulsado = (digitalRead(PIN_TAP_TEMPO) == LOW);
    unsigned long ahora = millis();

    if (tapPulsado && !tapAnteriorEstado && (ahora - ultimoTap > DEBOUNCE_TAP)) {
        unsigned long diferencia = ahora - ultimoTap;
        if (diferencia < 2000) {
            tiemposTap[indiceTap] = diferencia;
            indiceTap = (indiceTap + 1) % 4;
            unsigned long suma = 0;
            int validos = 0;
            for(int i=0; i<4; i++) { if(tiemposTap[i] > 0) { suma += tiemposTap[i]; validos++; } }
            if (validos > 0) {
                bpmActual = 60000 / (suma / validos);
                if (bpmActual < MIN_BPM) bpmActual = MIN_BPM;
                if (bpmActual > MAX_BPM) bpmActual = MAX_BPM;
            }
        } else {
            for(int i=0; i<4; i++) tiemposTap[i] = 0;
            indiceTap = 0;
        }
        ultimoTap = ahora;
    }
    tapAnteriorEstado = tapPulsado;
}

void gestionarCompas() {
    bool compasPulsado = (digitalRead(PIN_COMPAS) == LOW);
    unsigned long ahora = millis();

    if (compasPulsado && !compasAnteriorEstado && (ahora - ultimoCambioCompas > 200)) {
        indiceCompas = (indiceCompas + 1) % 3;
        Serial.print(">>> COMPÁS: "); Serial.println(listaCompases[indiceCompas]);
        ultimoCambioCompas = ahora;
    }
    compasAnteriorEstado = compasPulsado;
}

int obtenerBPM() { return bpmActual; }

String obtenerCompasActual() { return String(listaCompases[indiceCompas]); }