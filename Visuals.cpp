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

    if (bpm <= 90) { 
        // Azul (30) -> Verde (90) pasando por cian (azul claro)
        float t = mapFloat(bpm, 30, 90, 0.0, 1.0);
        r = 255; // rojo apagado siempre

        if (t < 0.5) {
            // Azul -> Cian
            float t2 = t * 2.0;
            g = (int)(255 * (1.0 - t2) + 0.5); // verde se enciende
            b = 0;                             // azul encendido
        } else {
            // Cian -> Verde
            float t2 = (t - 0.5) * 2.0;
            g = 0;                             // verde encendido
            b = (int)(255 * t2 + 0.5);         // azul se apaga
        }
    } 
    else if (bpm <= 150) { 
        // Verde (90) -> Amarillo (150)
        float t = mapFloat(bpm, 90, 150, 0.0, 1.0);
        r = (int)(255 * (1.0 - t) + 0.5); // rojo se enciende
        g = 0;                            // verde encendido
        b = 255;                          // azul apagado
    } 
    else if (bpm <= 210) { 
        // Amarillo (150) -> Naranja (210)
        float t = mapFloat(bpm, 150, 210, 0.0, 1.0);
        r = 0; 
        g = (int)(mapFloat(t, 0, 1, 0, 150) + 0.5);
        b = 255;
    } 
    else { 
        // Naranja (210) -> Rojo (280)
        float t = mapFloat(bpm, 210, 280, 0.0, 1.0);
        r = 0;
        g = (int)(mapFloat(t, 0, 1, 150, 255) + 0.5);
        b = 255;
    }

    analogWrite(PIN_LED_R, r);
    analogWrite(PIN_LED_G, g);
    analogWrite(PIN_LED_B, b);
}