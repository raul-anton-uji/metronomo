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
    Serial.println("  METRÓNOMO INICIADO PRO  ");
    Serial.println("===========================");
}

void loop() {
    // Procesar Entradas
    gestionarBoton();     // Pin 4: Reset/Salto
    gestionarTapTempo();  // Pin 5: Tap Tempo
    gestionarCompas();    // Pin 6: Compás (Cambia al soltar)
    
    int bpm = obtenerBPM();
    String compas = obtenerCompasActual();

    // Mostrar información si algo cambia (BPM o Compás)
    if (bpm != ultimoBPM || compas != ultimoCompasStr) {
        Serial.print("TEMPO: "); Serial.print(bpm);
        Serial.print(" BPM | COMPÁS: "); Serial.println(compas);
        ultimoBPM = bpm;
        ultimoCompasStr = compas;
    }

    // Lógica de Parpadeo del LED
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