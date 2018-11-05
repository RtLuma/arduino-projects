uint16_t t;
volatile bool left = false;

void motorZero(void) {
   left = !left;
   turnLeft(left);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
}

volatile void turnLeft(bool left) {
  if (left) { digitalWrite(7, HIGH); digitalWrite(9, LOW); }
  else      { digitalWrite(7, LOW);  digitalWrite(9, HIGH); }
}

void setup() {
  pinMode(9, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  
  randomSeed(analogRead(0));

  pinMode(2, INPUT_PULLUP);
  attachInterrupt(0, motorZero, RISING);

  
}

void loop() {
turnLeft(left);  
}
