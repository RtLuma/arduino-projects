#include <avr/interrupt.h> 
#include <avr/io.h> 

#define YAW_MOTOR_ENABLE 2 // 5
#define YAW_MOTOR_PIN_1  3 
#define YAW_MOTOR_PIN_2  4

#define YAW_INTERUPT_PIN 8 // 2

#define PITCH_MOTOR_ENABLE 5 // 8
#define PITCH_MOTOR_PIN_1  6  
#define PITCH_MOTOR_PIN_2  7  

#define PHOTO_BR A4
#define PHOTO_TR A7
#define PHOTO_BL A5
#define PHOTO_TL A6

volatile bool ZERO = false;

void YAW_AXIS_ZERO(void)
{ 
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    digitalWrite(YAW_MOTOR_PIN_1, !digitalRead(YAW_MOTOR_PIN_1));
    digitalWrite(YAW_MOTOR_PIN_2, !digitalRead(YAW_MOTOR_PIN_2));
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
    pinMode(YAW_MOTOR_ENABLE, OUTPUT); 
    pinMode(YAW_MOTOR_PIN_1,  OUTPUT); 
    pinMode(YAW_MOTOR_PIN_2,  OUTPUT); 

    pinMode(PITCH_MOTOR_ENABLE, OUTPUT); 
    pinMode(PITCH_MOTOR_PIN_1,  OUTPUT); 
    pinMode(PITCH_MOTOR_PIN_2,  OUTPUT); 

    pinMode(YAW_INTERUPT_PIN,  INPUT); 
    
    pinMode(PHOTO_BR, INPUT);
    pinMode(PHOTO_BL, INPUT);
    pinMode(PHOTO_TR, INPUT);
    pinMode(PHOTO_TL, INPUT);
//    attachInterrupt(digitalPinToInterrupt(YAW_INTERUPT_PIN), YAW_AXIS_ZERO, RISING);
    
    digitalWrite(YAW_MOTOR_ENABLE, HIGH);
    
    digitalWrite(YAW_MOTOR_PIN_1, HIGH);
    digitalWrite(YAW_MOTOR_PIN_2, LOW);
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


uint32_t right = (analogRead(PHOTO_BR) + analogRead(PHOTO_TR)) >> 1;
uint32_t left  = (analogRead(PHOTO_BL) + analogRead(PHOTO_TL)) >> 1;

int  delta = right - left;

if ( delta < -1) {
  digitalWrite(YAW_MOTOR_ENABLE, HIGH);
  digitalWrite(YAW_MOTOR_PIN_1, LOW);
  digitalWrite(YAW_MOTOR_PIN_2, HIGH);
}
else if (delta > 1) {
  digitalWrite(YAW_MOTOR_ENABLE, HIGH);
  digitalWrite(YAW_MOTOR_PIN_1, HIGH);
  digitalWrite(YAW_MOTOR_PIN_2, LOW);
}
else digitalWrite(YAW_MOTOR_ENABLE, LOW);

//Serial.print(left);
//Serial.print("\t");
//Serial.println(right);

Serial.println(analogRead(PHOTO_BR));

delay(100);

} 
