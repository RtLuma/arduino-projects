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
    pinMode(3, INPUT); 
    pinMode(7, OUTPUT); 
    pinMode(9, OUTPUT); 
    attachInterrupt(digitalPinToInterrupt(3), YAW_AXIS_ZERO, RISING);
    digitalWrite(9, HIGH);
    digitalWrite(7, LOW);
    while(!ZERO);
} 
                               
void loop(void) { 
  int t = millis();
  while(ZERO);
  t = millis() - t;
  Serial.println(t, DEC);
  ((void (*)())NULL)();
} 
