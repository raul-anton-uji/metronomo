#include "Config.h"
#include "Interfaz.h"
#include "Visuals.h"

unsigned long ultimoPulso = 0;
bool ledEncendido = false;
int ultimoBPM = 0;
String ultimoCompasStr = "";

void setup() {
    Serial.begin(9600);
    setupInterfaz();
    setupVisuals();
    Serial.println("===========================");
    Serial.println("  METRÓNOMO PRO FINAL V5   ");
    Serial.println("===========================");
}

void loop() {
    // 1. Siempre vigilar el Start/Stop
    gestionarStartStop();
    
    // 2. Procesar el resto solo si está en ejecución
    gestionarBoton();
    gestionarTapTempo();
    gestionarCompas();
    
    int bpm = obtenerBPM();
    String compas = obtenerCompasActual();

    // 3. Monitor serie (cambio de estado o BPM)
    if (bpm != ultimoBPM || compas != ultimoCompasStr) {
        Serial.print("TEMPO: "); Serial.print(bpm);
        Serial.print(" BPM | COMPÁS: "); Serial.println(compas);
        ultimoBPM = bpm; ultimoCompasStr = compas;
    }

    // 4. Parpadeo del LED
    if (estaEjecutando()) {
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