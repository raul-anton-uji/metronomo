
#include "Interfaz.h"
#include "Config.h"
#include <LiquidCrystal_I2C.h>

// --- Inicialización del LCD --- //
LiquidCrystal_I2C lcd(0x27, 20, 4); 

// --- Variables de Estado Global --- //
volatile int bpmActual = 120;      // BPM actual del metrónomo
bool ejecutando = true;            // Indica si el metrónomo está en ejecución

// --- Variables de Notificación --- //
String mensajeNotificacion = "";      // Mensaje a mostrar en la pantalla
unsigned long tiempoNotificacion = 0; // Momento en que se mostró la notificación
bool resetYaEjecutado = false;        // Evita múltiples resets por pulsación larga

// --- Variables del Encoder Rotatorio --- //
// Tabla de estados para decodificar el movimiento del encoder
const int8_t TABLA_ENCODER[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};
static uint8_t estadoEncoder = 0;     // Estado actual del encoder
static int8_t contadorPasos = 0;      // Acumula pasos para cambiar BPM

// --- Variables de Botones --- //
unsigned long tiempoInicioPulsacion = 0; // Momento en que se pulsó el botón del encoder
bool pulsadoAnteriormente = false;       // Estado previo del botón del encoder
bool tapAnteriorEstado = false;          // Estado previo del botón TAP
bool compasPresionadoActualmente = false;// Estado previo del botón compás
bool ssPresionadoActualmente = false;    // Estado previo del botón start/stop

// --- Variables para Tap Tempo y Compás --- //
unsigned long ultimoTap = 0;             // Último tiempo en que se pulsó TAP
unsigned long tiemposTap[4] = {0, 0, 0, 0}; // Array para almacenar intervalos entre taps
int indiceTap = 0;                       // Índice para el array de taps
int indiceCompas = 2;                    // Índice del compás seleccionado (por defecto 4/4)
const char* listaCompases[] = {"2/4", "3/4", "4/4"}; // Lista de compases disponibles


// --- Configuración inicial de la interfaz y hardware --- //
void setupInterfaz() {
    // Configura los pines de entrada/salida
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    pinMode(PIN_ENCODER_SW, INPUT_PULLUP);
    pinMode(PIN_TAP_TEMPO, INPUT_PULLUP);
    pinMode(PIN_COMPAS, INPUT_PULLUP);
    pinMode(PIN_START_STOP, INPUT_PULLUP);
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);
    
    // Configura interrupciones para el encoder rotatorio
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), checkEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT), checkEncoder, CHANGE);

    // Inicializa el bus I2C y la pantalla LCD
    Wire.begin();
    Wire.setClock(400000); // Bus I2C a 400kHz para reducir el tiempo de bloqueo
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("   METRONOMO PyR  "); // Mensaje de bienvenida
    delay(1000);
    lcd.clear();
}


// --- Rutina de interrupción para el encoder rotatorio --- //
// Ajusta el BPM según el giro del encoder
void checkEncoder() {
    if (!ejecutando) return; // Solo responde si el metrónomo está activo
    estadoEncoder <<= 2;
    if (digitalRead(PIN_ENCODER_CLK)) estadoEncoder |= 0x02; 
    if (digitalRead(PIN_ENCODER_DT))  estadoEncoder |= 0x01;
    int8_t movimiento = TABLA_ENCODER[estadoEncoder & 0x0F]; // Decodifica el movimiento
    if (movimiento != 0) {
        contadorPasos += movimiento;
        // Cambia el BPM cada 4 pasos completos
        if (contadorPasos >= 4) { bpmActual++; contadorPasos = 0; } 
        else if (contadorPasos <= -4) { bpmActual--; contadorPasos = 0; }
    }
    // Limita el BPM a los valores permitidos
    if (bpmActual < MIN_BPM) bpmActual = MIN_BPM;
    if (bpmActual > MAX_BPM) bpmActual = MAX_BPM;
}


// --- Gestiona el botón de Start/Stop --- //
void gestionarStartStop() {
    bool botonSS = (digitalRead(PIN_START_STOP) == LOW);
    if (botonSS && !ssPresionadoActualmente) ssPresionadoActualmente = true;
    if (!botonSS && ssPresionadoActualmente) {
        ejecutando = !ejecutando; // Alterna entre ejecutando y detenido
        ssPresionadoActualmente = false;
    }
}


// --- Gestiona el botón del encoder (pulsador central) --- //
// Pulsación corta: suma 30 BPM. Pulsación larga (>3s): resetea a 120 BPM.
void gestionarBoton() {
    if (!ejecutando) return; 
    bool botonPulsado = (digitalRead(PIN_ENCODER_SW) == LOW);

    // Detecta el inicio de la pulsación
    if (botonPulsado && !pulsadoAnteriormente) {
        tiempoInicioPulsacion = millis();
        pulsadoAnteriormente = true;
        resetYaEjecutado = false; 
    }

    // Si se mantiene pulsado más de 3 segundos, resetea BPM
    if (botonPulsado && pulsadoAnteriormente && !resetYaEjecutado) {
        if (millis() - tiempoInicioPulsacion >= 3000) {
            bpmActual = 120;
            mensajeNotificacion = "RESET: 120 BPM OK ";
            tiempoNotificacion = millis();
            resetYaEjecutado = true; 
        }
    }

    // Si se suelta antes de 3 segundos, suma 30 BPM
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


// --- Gestiona el botón TAP TEMPO --- //
void gestionarTapTempo() {
    if (!ejecutando) return; 
    bool tapPulsado = (digitalRead(PIN_TAP_TEMPO) == LOW);
    unsigned long ahora = millis();

    // Detecta flanco de bajada (pulsación)
    if (tapPulsado && !tapAnteriorEstado && (ahora - ultimoTap > 150)) {
        unsigned long diferencia = ahora - ultimoTap;
        if (diferencia < 2000) {
            // Guarda el intervalo entre taps
            tiemposTap[indiceTap] = diferencia;
            indiceTap = (indiceTap + 1) % 4;
            // Calcula el promedio de los últimos taps
            unsigned long suma = 0; int v = 0;
            for(int i=0; i<4; i++) { if(tiemposTap[i]>0){suma+=tiemposTap[i]; v++;} }
            if (v > 0) { 
                bpmActual = 60000 / (suma / v); // Convierte ms a BPM
                if (bpmActual > MAX_BPM) bpmActual = MAX_BPM;
                if (bpmActual < MIN_BPM) bpmActual = MIN_BPM;
            }
        } else { 
            // Si el intervalo es muy largo, reinicia el conteo
            indiceTap = 0; for(int i=0; i<4; i++) tiemposTap[i] = 0; 
        }
        ultimoTap = ahora;
    }
    tapAnteriorEstado = tapPulsado;
}


// --- Gestiona el botón de compás --- //
void gestionarCompas() {
    if (!ejecutando) return; 
    bool botonC = (digitalRead(PIN_COMPAS) == LOW);
    if (botonC && !compasPresionadoActualmente) compasPresionadoActualmente = true;
    if (!botonC && compasPresionadoActualmente) {
        indiceCompas = (indiceCompas + 1) % 3; // Cambia al siguiente compás
        compasPresionadoActualmente = false;
    }
}


// --- Devuelve la cantidad de tiempos según el compás seleccionado --- //
int obtenerTiemposPorCompas() {
    if (indiceCompas == 0) return 2; // 2/4
    if (indiceCompas == 1) return 3; // 3/4
    return 4;                        // 4/4
}


// --- FUNCIONES DE PANTALLA (Refresco Fraccionado) --- //

// Refresca todas las líneas de la pantalla LCD
void actualizarPantallaCompleta() {
    for(int i=0; i<4; i++) {
        actualizarLineaLCD(i);
    }
}

// Actualiza una línea específica de la pantalla LCD
// 0: Estado, 1: Tempo, 2: Compás, 3: Notificación o estado
void actualizarLineaLCD(int linea) {
    switch (linea) {
        case 0:
            lcd.setCursor(0, 0);
            lcd.print("STATUS: ");
            lcd.print(ejecutando ? "RUNNING   " : "STOPPED   ");
            break;
        case 1:
            lcd.setCursor(0, 1);
            lcd.print("TEMPO:  ");
            lcd.print(bpmActual);
            lcd.print(" BPM    ");
            break;
        case 2:
            lcd.setCursor(0, 2);
            lcd.print("TIME SIG: ");
            lcd.print(listaCompases[indiceCompas]);
            lcd.print("      ");
            break;
        case 3:
            lcd.setCursor(0, 3);
            // Si hay mensaje de notificación reciente, lo muestra
            if (mensajeNotificacion != "" && (millis() - tiempoNotificacion < 2000)) {
                lcd.print(">> ");
                lcd.print(mensajeNotificacion);
                lcd.print("   ");
            } else {
                mensajeNotificacion = ""; 
                if (ejecutando) lcd.print("> > > BEAT ON < < < ");
                else           lcd.print("--- STANDBY ---     ");
            }
            break;
    }
}


// --- Funciones auxiliares para obtener el estado actual --- //
bool estaEjecutando() { return ejecutando; }
int obtenerBPM() { return bpmActual; }
String obtenerCompasActual() { return String(listaCompases[indiceCompas]); }