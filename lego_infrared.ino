#define CHARGE_TIME 2000
#define ANALOG_PIN A0
#define DIGITAL_PIN 14

uint8_t inline l_map(uint16_t reading) {
  return (((uint16_t)(reading - 90)) << 8) / 70;
}

#define HISTORY 100

uint8_t history[HISTORY] = {0};
uint32_t sum = 0;

uint8_t read(void) {
  pinMode(DIGITAL_PIN, INPUT);

  analogRead(ANALOG_PIN);
  uint16_t reading =   l_map(analogRead(ANALOG_PIN));

  pinMode(DIGITAL_PIN, OUTPUT);
  digitalWrite(DIGITAL_PIN, HIGH);
  
  return reading;
}

void setup() {
  Serial.begin(9600);
  pinMode(      DIGITAL_PIN, OUTPUT );
  digitalWrite( DIGITAL_PIN, HIGH   );

  delay(50);

  for (uint32_t readings=0; readings < HISTORY; readings++) {
    history[readings] = read();
    sum += history[readings];
  } 
  
}

void loop() {

  sum -= history[0];
  for (uint8_t i = 1; i < HISTORY; i++) history[i - 1] = history[i];
  history[HISTORY - 1] = read();
  sum += history[HISTORY - 1];
  

  uint32_t avg = sum / HISTORY;  
  uint32_t stdv = 0;

  for (uint8_t r : history) stdv += sq(r - avg);
  stdv /= HISTORY;

//  if (stdv > 30) Serial.println("obstruction");
//  else Serial.println();

  Serial.print(avg);
  Serial.print(",");
  Serial.print(stdv);
  Serial.print(",");
  Serial.println(abs(history[HISTORY - 1]-(uint8_t)avg));
  //    delay(50);
}
