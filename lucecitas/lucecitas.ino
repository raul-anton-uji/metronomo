// Pines de LED RGB (activo a nivel bajo)
const int ledPinR = 9;
const int ledPinG = 10;
const int ledPinB = 11;

// BPM a usar
int bpm = 100; // Por defecto

// Variables para cálculo de tiempo
long interval; // Duración de un “beat” en ms

void setup() {
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinB, OUTPUT);

  calcularIntervalo();
}

void loop() {
  // Colores según BPM
  if (bpm < 80) {
    // Bajo BPM → verde
    analogWrite(ledPinR, 255);   // Rojo apagado
    analogWrite(ledPinG, 0);     // Verde al máximo
    analogWrite(ledPinB, 255);   // Azul apagado
  } else if (bpm < 140) {
    // BPM medio → amarillo
    analogWrite(ledPinR, 0);     // Rojo al máximo
    analogWrite(ledPinG, 175);     // Verde al máximo
    analogWrite(ledPinB, 255);   // Azul apagado
  } else {
    // BPM alto → rojo
    analogWrite(ledPinR, 0);     // Rojo al máximo
    analogWrite(ledPinG, 255);   // Verde apagado
    analogWrite(ledPinB, 255);   // Azul apagado
  }

  // Parpadeo al ritmo del BPM
  delay(interval / 2); // Mantener encendido medio beat

  // Apagar LED (nivel alto)
  analogWrite(ledPinR, 255);
  analogWrite(ledPinG, 255);
  analogWrite(ledPinB, 255);

  delay(interval / 2); // Apagado medio beat
}

// Función para calcular intervalo de cada beat en ms
void calcularIntervalo() {
  interval = 60000 / bpm; // 60000 ms en 1 minuto
}