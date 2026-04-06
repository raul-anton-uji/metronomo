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
    
    Serial.println("  METRÓNOMO INICIADO PRO  ");
}

void loop() {
    // 1. Procesar Entradas
    gestionarBoton();     // Pin 4: Reset/Salto
    gestionarTapTempo();  // Pin 5: Tap Tempo
    gestionarCompas();    // Pin 6: Compás
    
    int bpm = obtenerBPM();
    String compas = obtenerCompasActual();

    // 2. Mostrar información si algo cambia
    if (bpm != ultimoBPM || compas != ultimoCompasStr) {
        Serial.print("TEMPO: "); Serial.print(bpm);
        Serial.print(" BPM | COMPÁS: "); Serial.println(compas);
        ultimoBPM = bpm;
        ultimoCompasStr = compas;
    }

    // 3. Lógica de Parpadeo (LED RGB)
    unsigned long tiempoActual = millis();
    unsigned long intervalo = 60000 / bpm;

    // Parpadeo simétrico (encendido la mitad del intervalo)
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