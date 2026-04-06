#include "Config.h"
#include "Interfaz.h"
#include "Visuals.h"

unsigned long tiempoSiguienteCambio = 0; 
bool ledEncendido = false;

int ultimoBPM = 0;
String ultimoCompasStr = "";
bool ultimoEstadoEjecucion = false;
unsigned long ultimoRefrescoLCD = 0;

void setup() {
    Serial.begin(9600);
    setupInterfaz();
    setupVisuals();
    actualizarPantalla();
    tiempoSiguienteCambio = millis(); 
}

void loop() {
    unsigned long tiempoActual = millis();
    int bpm = obtenerBPM();
    bool estadoActual = estaEjecutando();

    // --- 1. LECTURA DE BOTONES (Rápida, no bloquea) ---
    gestionarStartStop();
    gestionarBoton();
    gestionarTapTempo();
    gestionarCompas();

    // --- 2. GESTIÓN DEL PULSO (Prioridad Máxima) ---
    if (estadoActual) {
        unsigned long medioIntervalo = 30000UL / bpm; 

        if (tiempoActual >= tiempoSiguienteCambio) {
            if (!ledEncendido) {
                actualizarColorLED(bpm);
                ledEncendido = true;
            } else {
                apagarLED();
                ledEncendido = false;
            }
            tiempoSiguienteCambio += medioIntervalo; 
        }

        // --- 3. ACTUALIZACIÓN INTELIGENTE DEL LCD (Evita el Jitter) ---
        // Solo actualizamos el LCD si:
        // A) Algo ha cambiado.
        // B) Ha pasado suficiente tiempo desde el último refresco.
        // C) Faltan más de 30ms para el próximo pulso (Ventana de seguridad).
        
        long tiempoHastaSiguiente = (long)tiempoSiguienteCambio - (long)millis();
        
        if (tiempoHastaSiguiente > 35) { // Solo si hay "hueco" suficiente
            String compasStr = obtenerCompasActual();
            if (bpm != ultimoBPM || compasStr != ultimoCompasStr || estadoActual != ultimoEstadoEjecucion || (tiempoActual - ultimoRefrescoLCD > 400)) {
                actualizarPantalla();
                ultimoBPM = bpm;
                ultimoCompasStr = compasStr;
                ultimoEstadoEjecucion = estadoActual;
                ultimoRefrescoLCD = tiempoActual;
            }
        }
    } else {
        // Si está parado, refresco normal
        apagarLED();
        ledEncendido = false;
        if (tiempoActual - ultimoRefrescoLCD > 200) {
            actualizarPantalla();
            ultimoRefrescoLCD = tiempoActual;
        }
        tiempoSiguienteCambio = tiempoActual; 
    }
}