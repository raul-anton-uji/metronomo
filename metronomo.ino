#include "Config.h"
#include "Interfaz.h"
#include "Visuals.h"

unsigned long ultimoPulso = 0;
bool ledEncendido = false;

// Variables para control de refresco del LCD
int ultimoBPM = 0;
String ultimoCompasStr = "";
bool ultimoEstadoEjecucion = false;

void setup() {
    Serial.begin(9600);
    setupInterfaz();
    setupVisuals();
    actualizarPantalla(); // Dibujo inicial
}

void loop() {
    // 1. Siempre leer el botón de Start/Stop
    gestionarStartStop();
    
    // 2. Leer el resto de controles
    gestionarBoton();
    gestionarTapTempo();
    gestionarCompas();
    
    int bpm = obtenerBPM();
    String compas = obtenerCompasActual();
    bool estadoActual = estaEjecutando();

    // 3. Refrescar LCD solo si algo ha cambiado
    if (bpm != ultimoBPM || compas != ultimoCompasStr || estadoActual != ultimoEstadoEjecucion) {
        actualizarPantalla();
        ultimoBPM = bpm;
        ultimoCompasStr = compas;
        ultimoEstadoEjecucion = estadoActual;
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
        // En Stop forzamos LED apagado
        apagarLED();
        ledEncendido = false;
    }
}