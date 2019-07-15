#define DELAY 100
void setup() {pinMode(LED_BUILTIN, OUTPUT);}
void loop() {
  digitalWrite(LED_BUILTIN, HIGH); delay(DELAY);
  digitalWrite(LED_BUILTIN, LOW);  delay(DELAY);
}