#ifndef VISUALS_H
#define VISUALS_H

#include <Arduino.h>

void setupVisuals();
void actualizarColorLED(int bpm);
void apagarLED();
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);

#endif