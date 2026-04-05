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
    int bpm = obtenerBPM();
    unsigned long intervalo = 60000 / bpm;
    unsigned long tiempoActual = millis();

    // Comprobar si se pulsa el botón del encoder (SW)
    if (botonPresionado()) {
        Serial.println("Click en Encoder!");
    }

    // Lógica de parpadeo sin delay
    // El LED estará encendido la mitad del intervalo y apagado la otra mitad
    if (tiempoActual - ultimoPulso >= (intervalo / 2)) {
        ultimoPulso = tiempoActual;
        
        if (!ledEncendido) {
            actualizarColorLED(bpm); // Aquí se aplica tu degradado
            ledEncendido = true;
            Serial.println(bpm);
        } else {
            apagarLED();
            ledEncendido = false;
        }
    }
}