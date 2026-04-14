#include "Interfaz.h"
#include "Config.h"
#include <LiquidCrystal_I2C.h>

// Inicialización del LCD (Asegúrate de que la dirección sea 0x27 o 0x3F)
LiquidCrystal_I2C lcd(0x27, 20, 4); 

volatile int bpmActual = 120;
bool ejecutando = true;

// --- Variables de Notificación ---
String mensajeNotificacion = "";
unsigned long tiempoNotificacion = 0;
bool resetYaEjecutado = false; 

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

unsigned long ultimoTap = 0;
unsigned long tiemposTap[4] = {0, 0, 0, 0};
int indiceTap = 0;
int indiceCompas = 2; // Empieza por defecto en 4/4
const char* listaCompases[] = {"2/4", "3/4", "4/4"};

void setupInterfaz() {
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    pinMode(PIN_ENCODER_SW, INPUT_PULLUP);
    pinMode(PIN_TAP_TEMPO, INPUT_PULLUP);
    pinMode(PIN_COMPAS, INPUT_PULLUP);
    pinMode(PIN_START_STOP, INPUT_PULLUP);
    pinMode(PIN_BUZZER, OUTPUT); // Pin 8 configurado para el buzzer
    
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), checkEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT), checkEncoder, CHANGE);

    Wire.begin();
    Wire.setClock(400000); // Overclock del bus I2C para evitar jitter
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("   METRONOMO PRO   ");
    delay(1000);
}

void checkEncoder() {
    if (!ejecutando) return;
    estadoEncoder <<= 2;
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

void gestionarStartStop() {
    bool botonSS = (digitalRead(PIN_START_STOP) == LOW);
    if (botonSS && !ssPresionadoActualmente) ssPresionadoActualmente = true;
    if (!botonSS && ssPresionadoActualmente) {
        ejecutando = !ejecutando;
        ssPresionadoActualmente = false;
    }
}

void gestionarBoton() {
    if (!ejecutando) return; 
    bool botonPulsado = (digitalRead(PIN_ENCODER_SW) == LOW);

    if (botonPulsado && !pulsadoAnteriormente) {
        tiempoInicioPulsacion = millis();
        pulsadoAnteriormente = true;
        resetYaEjecutado = false; 
    }

    if (botonPulsado && pulsadoAnteriormente && !resetYaEjecutado) {
        if (millis() - tiempoInicioPulsacion >= 3000) {
            bpmActual = 120;
            mensajeNotificacion = "RESET: 120 BPM OK ";
            tiempoNotificacion = millis();
            resetYaEjecutado = true; 
        }
    }

    if (!botonPulsado && pulsadoAnteriormente) {
        unsigned long duracion = millis() - tiempoInicioPulsacion;
        pulsadoAnteriormente = false;
        if (!resetYaEjecutado && duracion > 50) {
            bpmActual += 30;
            if (bpmActual > MAX_BPM) bpmActual = MIN_BPM;
            mensajeNotificacion = "SALTO: +30 BPM    ";
            tiempoNotificacion = millis();
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
                if (bpmActual > MAX_BPM) bpmActual = MAX_BPM;
                if (bpmActual < MIN_BPM) bpmActual = MIN_BPM;
            }
        } else { 
            indiceTap = 0; for(int i=0; i<4; i++) tiemposTap[i] = 0; 
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
        compasPresionadoActualmente = false;
    }
}

// Nueva función para retornar el divisor del compás para el buzzer
int obtenerTiemposPorCompas() {
    if (indiceCompas == 0) return 2; // 2/4
    if (indiceCompas == 1) return 3; // 3/4
    return 4;                        // 4/4
}

void actualizarPantalla() {
    lcd.setCursor(0, 0);
    lcd.print("STATUS: ");
    lcd.print(ejecutando ? "RUNNING   " : "STOPPED   ");

    lcd.setCursor(0, 1);
    lcd.print("TEMPO:  ");
    lcd.print(bpmActual);
    lcd.print(" BPM    "); 

    lcd.setCursor(0, 2);
    lcd.print("TIME SIG: ");
    lcd.print(listaCompases[indiceCompas]);
    lcd.print("      ");

    lcd.setCursor(0, 3);
    if (mensajeNotificacion != "" && (millis() - tiempoNotificacion < 2000)) {
        lcd.print(">> ");
        lcd.print(mensajeNotificacion);
        lcd.print("   ");
    } else {
        mensajeNotificacion = ""; 
        if (ejecutando) lcd.print("> > > BEAT ON < < < ");
        else           lcd.print("--- STANDBY ---     ");
    }
}

bool estaEjecutando() { return ejecutando; }
int obtenerBPM() { return bpmActual; }
String obtenerCompasActual() { return String(listaCompases[indiceCompas]); }