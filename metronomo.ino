#include "Config.h"
#include "Interfaz.h"
#include "Visuals.h"

unsigned long ultimoPulso = 0;
bool ledEncendido = false;

void setup() {
    Serial.begin(9600);
    setupInterfaz();
    setupVisuals();
    Serial.println("Metronomo listo con degradado.");
}

void loop() {
    // 1. Gestionar entradas
    gestionarBoton(); // <--- Esta función ahora controla el reset por tiempo
    int bpm = obtenerBPM();
    
    // 2. Imprimir BPM solo si cambia (tu lógica anterior)
    static int ultimoBPMVisible = 0;
    if (bpm != ultimoBPMVisible) {
        Serial.print("Tempo: "); Serial.println(bpm);
        ultimoBPMVisible = bpm;
    }

    // 3. Lógica del Metrónomo (Parpadeo LED)
    unsigned long intervalo = 60000 / bpm;
    unsigned long tiempoActual = millis();

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
}