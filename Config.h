#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Pines LED RGB
const int PIN_LED_R = 9;
const int PIN_LED_G = 10;
const int PIN_LED_B = 11;

// Pines Encoder
const int PIN_ENCODER_CLK = 2; // Antes PIN_ENCODER_A
const int PIN_ENCODER_DT  = 3; // Antes PIN_ENCODER_B
const int PIN_ENCODER_SW  = 4; // El pulsador

// Rangos
const int MIN_BPM = 30;
const int MAX_BPM = 280;

#endif