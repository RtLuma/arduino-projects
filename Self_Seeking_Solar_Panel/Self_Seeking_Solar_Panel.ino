#include <avr/interrupt.h> 
#include <avr/io.h> 

volatile bool ZERO = false;

void YAW_AXIS_ZERO(void)
{ 
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    digitalWrite(9, !digitalRead(9));
    digitalWrite(7, !digitalRead(7));
    ZERO = !ZERO;
}
                              
void setup(void) 
{ 
    Serial.begin(9600);
    pinMode(2, INPUT); 
    pinMode(7, OUTPUT); 
    pinMode(9, OUTPUT); 
    attachInterrupt(digitalPinToInterrupt(2), YAW_AXIS_ZERO, RISING);
    digitalWrite(9, HIGH);
    digitalWrite(7, LOW);
    while(!ZERO);
} 
                               
void loop(void) { 
  auto t = millis();
  while(millis() - t < 7000);
  t = millis() - t;
  while (!Serial);
  Serial.println(t, DEC);
  delay(10);
  ((void (*)())NULL)();
} 
