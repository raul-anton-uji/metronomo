# Metrónomo

Proyecto de desarrollo de un metrónomo digital basado en Arduino, diseñado para ofrecer una referencia temporal precisa mediante estímulos visuales y táctiles.

## Características

- Control de tempo (BPM) mediante encoder rotatorio  
- Retroalimentación visual mediante LED RGB y auditiva mediante buzzer pasivo
- Interfaz con pantalla LCD  
- Alta precisión temporal utilizando `micros()`  
- Interacción inmediata con botones y encoder  

## Tecnologías utilizadas

- Arduino Uno  
- Interfaz por display LCD (necesaria la librería LiquidCrystal I2C (1.1.2) de Frank B.)  
- Programación en C/C++  

## Posibles mejoras

- Soporte para compases adicionales (5/4, 7/8, etc.)  
- Función de muteo manteniendo el conteo interno  
- Salida MIDI para sincronización con otros dispositivos  

## Objetivo

El objetivo del proyecto es diseñar un sistema embebido robusto y preciso que mejore la experiencia de práctica musical, manteniendo una arquitectura escalable para futuras ampliaciones.