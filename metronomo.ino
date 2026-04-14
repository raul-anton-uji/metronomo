#include "Config.h"
#include "Interfaz.h"
#include "Visuals.h"

unsigned long tiempoSiguienteCambio = 0; 
bool ledEncendido = false;
int contadorPulsos = 0; // Para saber en qué parte del compás estamos

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

    // 1. Controles siempre activos
    gestionarStartStop();
    gestionarBoton();
    gestionarTapTempo();
    gestionarCompas();

    // 2. Gestión del Pulso + Sonido
    if (estadoActual) {
        unsigned long medioIntervalo = 30000UL / bpm; 

        if (tiempoActual >= tiempoSiguienteCambio) {
            if (!ledEncendido) {
                // --- INICIO DEL PULSO (Sonido y Luz) ---
                actualizarColorLED(bpm);
                
                int tiemposMax = obtenerTiemposPorCompas();
                
                if (contadorPulsos == 0) {
                    // ACENTO: Tono más agudo (1500Hz) durante 40ms
                    tone(PIN_BUZZER, 1500, 40); 
                } else {
                    // NORMAL: Tono estándar (1000Hz) durante 30ms
                    tone(PIN_BUZZER, 1000, 30);
                }
                
                contadorPulsos = (contadorPulsos + 1) % tiemposMax;
                ledEncendido = true;
            } else {
                // --- FIN DEL PULSO (Solo apagar luz) ---
                apagarLED();
                ledEncendido = false;
            }
            tiempoSiguienteCambio += medioIntervalo; 
        }

        // 3. Ventana de seguridad para el LCD
        long tiempoHastaSiguiente = (long)tiempoSiguienteCambio - (long)millis();
        if (tiempoHastaSiguiente > 40) { // Un poco más de margen por el buzzer
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
        // En Stop
        apagarLED();
        noTone(PIN_BUZZER);
        ledEncendido = false;
        contadorPulsos = 0; // Reiniciar compás al parar
        if (tiempoActual - ultimoRefrescoLCD > 200) {
            actualizarPantalla();
            ultimoRefrescoLCD = tiempoActual;
        }
        tiempoSiguienteCambio = tiempoActual; 
    }
}