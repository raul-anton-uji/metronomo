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
void gestionarStartStop(); // lógica del Start/Stop 
bool estaEjecutando(); // parar que el .ino ejecute lógica si está en Start
void actualizarPantalla(); // actualizar LCD con BPM y compás
int obtenerTiemposPorCompas(); // función para el buzzer
void actualizarPantallaCompleta();
void actualizarLineaLCD(int linea);

#endif