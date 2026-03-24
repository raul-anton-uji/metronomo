// Pines de LED RGB (activo a nivel bajo)
const int ledPinR = 9;
const int ledPinG = 10;
const int ledPinB = 11;

// BPM
int bpm = 110; // puedes cambiarlo para probar

// Variables
long interval; // Duración de un “beat” en ms

void setup() {
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinB, OUTPUT);

  calcularIntervalo();
}

void loop() {
  // Limitar BPM a rango 30-280
  if (bpm < 30) bpm = 30;
  if (bpm > 280) bpm = 280;

  // Calcular color en gradiente según BPM
  // Azul (30 bpm) → Verde → Amarillo → Naranja → Rojo (280 bpm)
  int r, g, b;

  if (bpm <= 80) {
    // Azul → Verde
    float t = mapFloat(bpm, 30, 80, 0.0, 1.0);
    r = 255;               // Rojo apagado
    g = 255 * (1 - t);     // Verde disminuye
    b = 255 * t;           // Azul aumenta
  } else if (bpm <= 140) {
    // Verde → Amarillo
    float t = mapFloat(bpm, 80, 140, 0.0, 1.0);
    r = 255 * (1 - t);     // Rojo empieza a aparecer
    g = 255 * (1 - 0.2*t); // Verde algo se atenúa
    b = 255;               // Azul apagado
  } else if (bpm <= 200) {
    // Amarillo → Naranja
    float t = mapFloat(bpm, 140, 200, 0.0, 1.0);
    r = 0;                  // Rojo al máximo
    g = 175 * (1 - t);      // Verde disminuye para naranja
    b = 255;                // Azul apagado
  } else {
    // Naranja → Rojo
    float t = mapFloat(bpm, 200, 280, 0.0, 1.0);
    r = 0;                  // Rojo al máximo
    g = 100 * (1 - t);      // Verde se atenúa
    b = 255;                // Azul apagado
  }

  // Escribir color al LED
  analogWrite(ledPinR, r);
  analogWrite(ledPinG, g);
  analogWrite(ledPinB, b);

  // Parpadeo al ritmo de los BPM
  delay(interval / 2);
  // Apagar LED
  analogWrite(ledPinR, 255);
  analogWrite(ledPinG, 255);
  analogWrite(ledPinB, 255);
  delay(interval / 2);
}

// Función para calcular intervalo de cada beat en ms
void calcularIntervalo() {
  interval = 60000 / bpm;
}

// Función auxiliar: mapFloat similar a map() pero para float
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}