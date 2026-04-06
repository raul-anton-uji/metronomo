#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <Arduino.h>

void setupInterfaz();
void checkEncoder(); 
int obtenerBPM();
void establecerBPM(int nuevoBpm); // reset
void gestionarBoton();           // lógica de los 2 segundos
void gestionarTapTempo(); // lógica del Tap Tempo
void gestionarCompas();      
String obtenerCompasActual(); // mostrar compás actual

#endif