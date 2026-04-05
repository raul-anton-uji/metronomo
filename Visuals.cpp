#include "Visuals.h"
#include "Config.h"

void setupVisuals() {
    pinMode(PIN_LED_R, OUTPUT);
    pinMode(PIN_LED_G, OUTPUT);
    pinMode(PIN_LED_B, OUTPUT);
    apagarLED();
}

void apagarLED() {
    analogWrite(PIN_LED_R, 255);
    analogWrite(PIN_LED_G, 255);
    analogWrite(PIN_LED_B, 255);
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void actualizarColorLED(int bpm) {
    int r, g, b;

    // --- Tu lógica de degradado ---
    if (bpm <= 90) { 
        // Azul (30) -> Verde (90)
        float t = mapFloat(bpm, 30, 90, 0.0, 1.0);
        r = 255;                // Rojo apagado
        g = 255 * (1 - t);      // Verde va encendiendo (baja a 0)
        b = 255 * t;            // Azul va apagando (sube a 255)
    } 
    else if (bpm <= 150) { 
        // Verde (90) -> Amarillo (150)
        float t = mapFloat(bpm, 90, 150, 0.0, 1.0);
        r = 255 * (1 - t);      // Rojo va encendiendo
        g = 0;                  // Verde encendido total
        b = 255;                // Azul apagado
    } 
    else if (bpm <= 210) { 
        // Amarillo (150) -> Naranja (210)
        float t = mapFloat(bpm, 150, 210, 0.0, 1.0);
        r = 0;                  // Rojo encendido total
        g = mapFloat(t, 0, 1, 0, 150); // Verde sube (se apaga un poco para naranja)
        b = 255;                // Azul apagado
    } 
    else { 
        // Naranja (210) -> Rojo (280)
        float t = mapFloat(bpm, 210, 280, 0.0, 1.0);
        r = 0;                  // Rojo encendido total
        g = mapFloat(t, 0, 1, 150, 255); // Verde se apaga del todo
        b = 255;                // Azul apagado
    }

    analogWrite(PIN_LED_R, r);
    analogWrite(PIN_LED_G, g);
    analogWrite(PIN_LED_B, b);
}