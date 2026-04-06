#include "Config.h"
#include "Interfaz.h"
#include "Visuals.h"

unsigned long ultimoPulso = 0;
bool ledEncendido = false;

// Variables para control de refresco del LCD
int ultimoBPM = 0;
String ultimoCompasStr = "";
bool ultimoEstadoEjecucion = false;
unsigned long ultimoRefrescoLCD = 0;

void setup() {
    Serial.begin(9600);
    setupInterfaz();
    setupVisuals();
    actualizarPantalla();
}

void loop() {
    // 1. Siempre vigilar el Start/Stop
    gestionarStartStop();
    
    // 2. Leer controles (bloqueados internamente si STOP)
    gestionarBoton();
    gestionarTapTempo();
    gestionarCompas();
    
    int bpm = obtenerBPM();
    String compas = obtenerCompasActual();
    bool estadoActual = estaEjecutando();

    // 3. Refrescar LCD si cambian datos O para limpiar notificaciones (cada 200ms)
    if (bpm != ultimoBPM || compas != ultimoCompasStr || estadoActual != ultimoEstadoEjecucion || (millis() - ultimoRefrescoLCD > 200)) {
        actualizarPantalla();
        ultimoBPM = bpm;
        ultimoCompasStr = compas;
        ultimoEstadoEjecucion = estadoActual;
        ultimoRefrescoLCD = millis();
    }

    // 4. Salida de Pulso (LED RGB)
    if (estadoActual) {
        unsigned long tiempoActual = millis();
        unsigned long intervalo = 60000 / bpm;

        if (tiempoActual - ultimoPulso >= (intervalo / 2)) {
            ultimoPulso = tiempoActual;
            if (!ledEncendido) {
                actualizarColorLED(bpm);
                ledEncendido = true;
            } else {
                apagarLED();
                ledEncendido = false;
            }
        }
    } else {
        apagarLED();
        ledEncendido = false;
    }
}