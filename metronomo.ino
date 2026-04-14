#include "Config.h"
#include "Interfaz.h"
#include "Visuals.h"

unsigned long tiempoSiguienteCambioMicros = 0; 
bool ledEncendido = false;
int contadorPulsos = 0;

int ultimoBPM = 0;
String ultimoCompasStr = "";
bool ultimoEstadoEjecucion = false;
unsigned long ultimoRefrescoLCD = 0;

void setup() {
    Serial.begin(9600);
    setupInterfaz();
    setupVisuals();
    actualizarPantalla();
    tiempoSiguienteCambioMicros = micros(); 
}

void loop() {
    unsigned long tiempoActualMicros = micros();
    unsigned long tiempoActualMillis = millis(); // Para tareas lentas
    int bpm = obtenerBPM();
    bool estadoActual = estaEjecutando();

    // 1. Controles (Lectura ultra rápida)
    gestionarStartStop();
    gestionarBoton();
    gestionarTapTempo();
    gestionarCompas();

    // 2. Gestión del Pulso con precisión de microsegundos
    if (estadoActual) {
        // Calculamos el medio intervalo en microsegundos (30 millones / bpm)
        unsigned long medioIntervaloMicros = 30000000UL / bpm; 

        if (tiempoActualMicros >= tiempoSiguienteCambioMicros) {
            if (!ledEncendido) {
                // --- PUNTO CRÍTICO DE SINCRONÍA ---
                actualizarColorLED(bpm);
                
                int tiemposMax = obtenerTiemposPorCompas();
                if (contadorPulsos == 0) {
                    tone(PIN_BUZZER, 1500, 30); // Acento
                } else {
                    tone(PIN_BUZZER, 1000, 20); // Normal
                }
                
                contadorPulsos = (contadorPulsos + 1) % tiemposMax;
                ledEncendido = true;
            } else {
                apagarLED();
                ledEncendido = false;
            }
            // Sincronía matemática perfecta
            tiempoSiguienteCambioMicros += medioIntervaloMicros; 
        }

        // 3. Ventana de exclusión del LCD
        // Solo permitimos LCD si faltan más de 40ms (40000us) para el próximo evento
        long tiempoRestante = (long)tiempoSiguienteCambioMicros - (long)micros();
        
        if (tiempoRestante > 40000L) { 
            String compasStr = obtenerCompasActual();
            if (bpm != ultimoBPM || compasStr != ultimoCompasStr || estadoActual != ultimoEstadoEjecucion || (tiempoActualMillis - ultimoRefrescoLCD > 400)) {
                actualizarPantalla();
                ultimoBPM = bpm;
                ultimoCompasStr = compasStr;
                ultimoEstadoEjecucion = estadoActual;
                ultimoRefrescoLCD = tiempoActualMillis;
            }
        }
    } else {
        // Modo Standby
        apagarLED();
        noTone(PIN_BUZZER);
        ledEncendido = false;
        contadorPulsos = 0;
        if (tiempoActualMillis - ultimoRefrescoLCD > 200) {
            actualizarPantalla();
            ultimoRefrescoLCD = tiempoActualMillis;
        }
        tiempoSiguienteCambioMicros = micros(); 
    }
}