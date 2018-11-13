#include <avr/interrupt.h> 
#include <avr/io.h> 

volatile bool ZERO = false;

void YAW_AXIS_ZERO(void)
{ 
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    digitalWrite(3, !digitalRead(3));
    digitalWrite(4, !digitalRead(4));
    ZERO = !ZERO;
}

//void PITCH_AXIS_ZERO(void)
//{ 
//    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//    digitalWrite(3, !digitalRead(3));
//    digitalWrite(4, !digitalRead(4));
//    ZERO = !ZERO;
//}
                              
void setup(void) 
{ 
    Serial.begin(9600);
    pinMode(2, OUTPUT); 
    pinMode(3, OUTPUT); 
    pinMode(4, OUTPUT); 
    pinMode(5, INPUT); 
    pinMode(A7, INPUT);
    pinMode(A6, INPUT);
    pinMode(A5, INPUT);
    pinMode(A4, INPUT);
//    attachInterrupt(digitalPinToInterrupt(2), YAW_AXIS_ZERO, RISING);
    digitalWrite(2, HIGH);
    
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
//    while(!ZERO);

} 

uint32_t t = 0;
                               
void loop(void) { 
//  volatile long unsigned int t=0;

//  while (t < 3000000) t++;
//  
//  Serial.println(t);
//  delay(10);
//  ((void (*)())NULL)();


uint32_t right = (analogRead(A4) + analogRead(A7)) >> 1;
uint32_t left  = (analogRead(A5) + analogRead(A6)) >> 1;

int  delta = right - left;

if ( delta < -1) {
  digitalWrite(2, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
}
else if (delta > 1) {
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);
}
else digitalWrite(2, LOW);

Serial.print(left);
Serial.print("\t");
Serial.println(right);
delay(100);

} 
