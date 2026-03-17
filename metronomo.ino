void setup() {
  pinMode(13, OUTPUT);       // LED integrado
  Serial.begin(9600);        // Iniciar comunicación serie
}

void loop() {
  digitalWrite(13, HIGH);    // Encender LED
  Serial.println("LED ON");
  delay(1000);

  digitalWrite(13, LOW);     // Apagar LED
  Serial.println("LED OFF");
  delay(1000);
}