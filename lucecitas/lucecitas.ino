// Pines de LED RGB (PWM)
const int ledPinR = 9;
const int ledPinG = 10;
const int ledPinB = 11;

// Variables de control
int bpm = 30; 
long interval;

void setup() {
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinB, OUTPUT);
  Serial.begin(9600); // Para ver el progreso en el monitor
}

void loop() {
  // --- BLOQUE DE PRUEBA: Recorrido de 30 a 280 BPM ---
  for (bpm = 30; bpm <= 280; bpm += 5) {
    
    interval = 60000 / bpm; // Recalculamos el tiempo por cada latido
    
    int r, g, b;

    // Gradiente: Azul (30) -> Verde -> Amarillo -> Naranja -> Rojo (280)
    // RECUERDA: 0 es encendido total, 255 es apagado total
    
    if (bpm <= 90) { 
      // Azul -> Verde
      float t = mapFloat(bpm, 30, 90, 0.0, 1.0);
      r = 255;                   // Rojo apagado
      g = 255 * (1 - t);         // Verde va encendiendo (baja hacia 0)
      b = 255 * t;               // Azul va apagando (sube hacia 255)
    } 
    else if (bpm <= 150) { 
      // Verde -> Amarillo (Amarillo es Rojo + Verde)
      float t = mapFloat(bpm, 90, 150, 0.0, 1.0);
      r = 255 * (1 - t);         // Rojo va encendiendo
      g = 0;                     // Verde encendido total
      b = 255;                   // Azul apagado
    } 
    else if (bpm <= 210) { 
      // Amarillo -> Naranja (Naranja es Rojo al 100% y Verde al 50% aprox)
      float t = mapFloat(bpm, 150, 210, 0.0, 1.0);
      r = 0;                     // Rojo encendido total
      g = mapFloat(t, 0, 1, 0, 150); // Verde sube (se apaga un poco para hacer naranja)
      b = 255;                   // Azul apagado
    } 
    else { 
      // Naranja -> Rojo
      float t = mapFloat(bpm, 210, 280, 0.0, 1.0);
      r = 0;                     // Rojo encendido total
      g = mapFloat(t, 0, 1, 150, 255); // Verde se apaga del todo
      b = 255;                   // Azul apagado
    }

    // Monitorización
    Serial.print("BPM: "); Serial.println(bpm);

    // Ejecutar el latido (Blink)
    // Encender con el color calculado
    analogWrite(ledPinR, r);
    analogWrite(ledPinG, g);
    analogWrite(ledPinB, b);
    delay(interval / 2);

    // Apagar LED (Todo a 255 en ánodo común)
    analogWrite(ledPinR, 255);
    analogWrite(ledPinG, 255);
    analogWrite(ledPinB, 255);
    delay(interval / 2);
  }
}

// Función auxiliar para mapeo de decimales
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}