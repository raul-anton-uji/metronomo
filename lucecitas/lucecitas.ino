// pin del LED
const int ledPinR = 9;
const int ledPinG = 10;
const int ledPinB = 11;

void setup() {
  // Configuramos el pin como salida
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinB, OUTPUT);
}

void loop() {
  digitalWrite(ledPinR, LOW);
  digitalWrite(ledPinG, HIGH);
  digitalWrite(ledPinB, HIGH);   
  delay(1000);                  
  digitalWrite(ledPinR, HIGH);
  digitalWrite(ledPinG, LOW); 
  digitalWrite(ledPinB, HIGH);
  delay(1000);                  
  digitalWrite(ledPinR, HIGH);
  digitalWrite(ledPinG, HIGH);
  digitalWrite(ledPinB, LOW);    
  delay(1000);                  
}