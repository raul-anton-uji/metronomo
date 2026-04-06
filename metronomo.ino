#include "Config.h"
#include "Interfaz.h"
#include "Visuals.h"

unsigned long tiempoSiguienteCambio = 0; 
bool ledEncendido = false;

// Control de refresco
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

    if (estadoActual) {
        // Cálculo de alta precisión
        unsigned long medioIntervalo = 30000UL / bpm; 

        // PRIORIDAD 1: Cambio del LED (Momento crítico)
        if (tiempoActual >= tiempoSiguienteCambio) {
            if (!ledEncendido) {
                actualizarColorLED(bpm);
                ledEncendido = true;
            } else {
                apagarLED();
                ledEncendido = false;
            }
            // Sincronía absoluta para evitar deriva
            tiempoSiguienteCambio += medioIntervalo; 

            // PRIORIDAD 2: El resto del código se ejecuta JUSTO después del pulso
            // Esto garantiza que el LCD no interrumpa el inicio del siguiente pulso
            gestionarStartStop();
            gestionarBoton();
            gestionarTapTempo();
            gestionarCompas();

            String compasStr = obtenerCompasActual();
            
            // Refresco inteligente: solo si cambia algo o han pasado 400ms
            if (bpm != ultimoBPM || compasStr != ultimoCompasStr || estadoActual != ultimoEstadoEjecucion || (millis() - ultimoRefrescoLCD > 400)) {
                actualizarPantalla();
                ultimoBPM = bpm;
                ultimoCompasStr = compasStr;
                ultimoEstadoEjecucion = estadoActual;
                ultimoRefrescoLCD = millis();
            }
        }
    } else {
        // Modo STANDBY: Respuesta rápida de controles
        apagarLED();
        ledEncendido = false;
        tiempoSiguienteCambio = millis();
        
        gestionarStartStop();
        gestionarBoton();
        gestionarTapTempo();
        gestionarCompas();
        
        if (millis() - ultimoRefrescoLCD > 200) {
            actualizarPantalla();
            ultimoRefrescoLCD = millis();
        }
    }
}