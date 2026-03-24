// Pines de LED RGB
const int ledPinR = 9;
const int ledPinG = 10;
const int ledPinB = 11;

// BPM a usar
int bpm = 100; // Por defecto, puedes cambiarlo

// Variables para cálculo de tiempo
long interval; // Duración de un “beat” en ms

void setup() {
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinB, OUTPUT);

  calcularIntervalo();
}

void loop() {
  if (bpm < 80) {
    // Bajo BPM → LED verde
    digitalWrite(ledPinR, HIGH);
    digitalWrite(ledPinG, LOW);
    digitalWrite(ledPinB, HIGH);
    delay(interval / 2);  
    digitalWrite(ledPinR, HIGH);
    digitalWrite(ledPinG, HIGH);
    digitalWrite(ledPinB, HIGH);
    delay(interval / 2);  

  } else if (bpm < 140) {
    // BPM medio → LED amarillo
    digitalWrite(ledPinR, LOW);
    digitalWrite(ledPinG, LOW);
    digitalWrite(ledPinB, HIGH);
    delay(interval / 2);  // Encendido medio beat
    digitalWrite(ledPinR, HIGH);
    digitalWrite(ledPinG, HIGH);
    digitalWrite(ledPinB, HIGH);
    delay(interval / 2);  // Encendido medio beat

  } else {
    // BPM alto → LED rojo
    digitalWrite(ledPinR, LOW);
    digitalWrite(ledPinG, HIGH);
    digitalWrite(ledPinB, HIGH);
    delay(interval / 2);  // Encendido medio beat
    digitalWrite(ledPinR, HIGH);
    digitalWrite(ledPinG, HIGH);
    digitalWrite(ledPinB, HIGH);
    delay(interval / 2);  // Encendido medio beat
  }
}

// Función para calcular intervalo de cada beat en ms
void calcularIntervalo() {
  interval = 60000 / bpm; // 60000 ms en 1 minuto
}