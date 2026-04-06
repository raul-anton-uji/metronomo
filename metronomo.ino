#include "Config.h"
#include "Interfaz.h"
#include "Visuals.h"

unsigned long ultimoPulso = 0;
bool ledEncendido = false;
int ultimoBPMRegistrado = 0; // Para detectar cambios

void setup() {
    Serial.begin(9600);
    setupInterfaz();
    setupVisuals();
    Serial.println("--- Metrónomo Pro v2.0 ---");
    Serial.print("Tempo inicial: "); Serial.print(obtenerBPM()); Serial.println(" BPM");
}

void loop() {
    // 1. Procesar Entradas
    gestionarBoton();
    gestionarTapTempo();
    
    int bpm = obtenerBPM();

    // 2. MOSTRAR BPM SI CAMBIA
    if (bpm != ultimoBPMRegistrado) {
        Serial.print(">>> Nuevo Tempo: ");
        Serial.print(bpm);
        Serial.println(" BPM");
        ultimoBPMRegistrado = bpm;
    }

    // 3. Lógica de Parpadeo
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
}