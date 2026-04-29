#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// Pines LED RGB
const int PIN_LED_R = 9;
const int PIN_LED_G = 10;
const int PIN_LED_B = 11;

// Pines Encoder
const int PIN_ENCODER_CLK = 2; 
const int PIN_ENCODER_DT  = 3; 
const int PIN_ENCODER_SW  = 4; 

// Pin Tap Tempo 
const int PIN_TAP_TEMPO   = 5; 

// Pin cambio de compás
const int PIN_COMPAS      = 6; 

// Pin Start/Stop 
const int PIN_START_STOP  = 7;

// Pin buzzer
const int PIN_BUZZER = 8;

// Rangos
const int MIN_BPM = 30;
const int MAX_BPM = 280;

// Configuración LCD
const int LCD_COLS = 20;
const int LCD_FILAS = 4;
const int LCD_ADDR = 0x27; 

#endif