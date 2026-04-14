#include "Config.h"
#include "Interfaz.h"
#include "Visuals.h"

double tiempoSiguienteCambioMicros = 0; 
bool ledEncendido = false;
int contadorPulsos = 0;

int ultimoBPM = 0;
String ultimoCompasStr = "";
bool ultimoEstadoEjecucion = false;
unsigned long ultimoRefrescoLCD = 0;
int lineaAActualizar = 0; // Para el refresco fraccionado

void setup() {
    Serial.begin(9600);
    setupInterfaz();
    setupVisuals();
    actualizarPantallaCompleta(); // Una vez al inicio
    tiempoSiguienteCambioMicros = (double)micros(); 
}

void loop() {
    unsigned long ahoraMicros = micros();
    unsigned long ahoraMillis = millis();
    int bpm = obtenerBPM();
    bool estadoActual = estaEjecutando();

    // 1. Prioridad 1: Controles (Lectura ultra-veloz)
    gestionarStartStop();
    gestionarBoton();
    gestionarTapTempo();
    gestionarCompas();

    if (estadoActual) {
        double medioIntervaloMicros = 30000000.0 / (double)bpm;

        // 2. Prioridad 2: El Pulso (Matemática de arrastre de error)
        if ((double)ahoraMicros >= tiempoSiguienteCambioMicros) {
            if (!ledEncendido) {
                actualizarColorLED(bpm);
                int tiemposMax = obtenerTiemposPorCompas();
                if (contadorPulsos == 0) tone(PIN_BUZZER, 1500, 25); 
                else tone(PIN_BUZZER, 1000, 15);
                contadorPulsos = (contadorPulsos + 1) % tiemposMax;
                ledEncendido = true;
            } else {
                apagarLED();
                ledEncendido = false;
            }
            tiempoSiguienteCambioMicros += medioIntervaloMicros; 
        }

        // 3. Prioridad 3: LCD Fraccionado (Solo 10ms de bloqueo por vuelta)
        double tiempoRestante = tiempoSiguienteCambioMicros - (double)ahoraMicros;
        
        // Con 20ms de margen es suficiente porque ahora cada línea es muy rápida
        if (tiempoRestante > 20000.0) { 
            String compasStr = obtenerCompasActual();
            // Si algo cambia, forzamos refresco de esa línea específica
            if (bpm != ultimoBPM || compasStr != ultimoCompasStr || estadoActual != ultimoEstadoEjecucion || (ahoraMillis - ultimoRefrescoLCD > 100)) {
                
                actualizarLineaLCD(lineaAActualizar); // Solo actualiza UNA línea
                lineaAActualizar = (lineaAActualizar + 1) % 4;
                
                if (lineaAActualizar == 0) { // Solo resetear marcas cuando damos la vuelta completa
                    ultimoBPM = bpm;
                    ultimoCompasStr = compasStr;
                    ultimoEstadoEjecucion = estadoActual;
                    ultimoRefrescoLCD = ahoraMillis;
                }
            }
        }
    } else {
        // En STOP todo es relajado
        apagarLED();
        noTone(PIN_BUZZER);
        ledEncendido = false;
        contadorPulsos = 0;
        if (ahoraMillis - ultimoRefrescoLCD > 200) {
            actualizarPantallaCompleta();
            ultimoRefrescoLCD = ahoraMillis;
        }
        tiempoSiguienteCambioMicros = (double)micros(); 
    }
}