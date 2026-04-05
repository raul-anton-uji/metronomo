#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <Arduino.h>

void setupInterfaz();
void checkEncoder(); 
int obtenerBPM();
void establecerBPM(int nuevoBpm); // Nueva: para poder resetear desde fuera si hiciera falta
void gestionarBoton();           // Nueva: maneja la lógica de los 3 segundos

#endif