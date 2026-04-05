#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <Arduino.h>

void setupInterfaz();
void checkEncoder(); 
int obtenerBPM();
bool botonPresionado(); // <--- AÑADE ESTA LÍNEA AQUÍ

#endif