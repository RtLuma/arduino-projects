#include <avr/interrupt.h>
#include <avr/io.h>

#define YAW_MOTOR_PIN_1  9
#define YAW_MOTOR_PIN_2  4
#define YAW_MOTOR_ENABLE 5

#define YAW_INTERUPT_PIN 2

#define PITCH_MOTOR_PIN_1  6
#define PITCH_MOTOR_PIN_2  7
#define PITCH_MOTOR_ENABLE 8

#define PHOTO_BR A4
#define PHOTO_TR A7
#define PHOTO_BL A5
#define PHOTO_TL A6

volatile bool ZERO = false;

void yawTurn(bool ccw) {
  digitalWrite(YAW_MOTOR_ENABLE, HIGH);
  digitalWrite(YAW_MOTOR_PIN_2, ccw);
  digitalWrite(YAW_MOTOR_PIN_1, !ccw);
}

void yawStop(void) {
  digitalWrite(YAW_MOTOR_ENABLE, LOW);
  digitalWrite(YAW_MOTOR_PIN_2, LOW);
  digitalWrite(YAW_MOTOR_PIN_1, LOW);
}

void pitchTurn(bool ccw) {
  digitalWrite(PITCH_MOTOR_ENABLE, HIGH);
  digitalWrite(PITCH_MOTOR_PIN_2, ccw);
  digitalWrite(PITCH_MOTOR_PIN_1, !ccw);
}

void pitchStop(void) {
  digitalWrite(PITCH_MOTOR_ENABLE, LOW);
  digitalWrite(PITCH_MOTOR_PIN_2, LOW);
  digitalWrite(PITCH_MOTOR_PIN_1, LOW);
}



void YAW_AXIS_ZERO(void) {
  Serial.println("Interupted!");
  yawStop();
  for (volatile uint32_t wait = 0; wait < 0xFFFFF;) wait++;

  Serial.println("Interupt ended!");
  ZERO = !ZERO;
}

void setup(void) {
  Serial.begin(9600);
  digitalWrite(3, LOW); //cuz physics is gay lmao
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  pinMode(YAW_INTERUPT_PIN,  INPUT);

  int OUTPUTS[] = {
    YAW_MOTOR_ENABLE,
    YAW_MOTOR_PIN_1,
    YAW_MOTOR_PIN_2,
    PITCH_MOTOR_ENABLE,
    PITCH_MOTOR_PIN_1,
    PITCH_MOTOR_PIN_2,
    PHOTO_BR,
    PHOTO_TR,
    PHOTO_TL,
    PHOTO_BL
  };

  for (auto p : OUTPUTS) pinMode(p, OUTPUT);

  yawTurn(false);

  //  while (!digitalRead(YAW_INTERUPT_PIN)) Serial.println(digitalRead(YAW_INTERUPT_PIN));

  //  attachInterrupt(digitalPinToInterrupt(YAW_INTERUPT_PIN), YAW_AXIS_ZERO, RISING);
  //  while (!ZERO);

  while (!ZERO) ZERO = digitalRead(YAW_INTERUPT_PIN);
  digitalWrite(13, LOW);
  yawTurn(true);
  delay(4000);
  digitalWrite(13, HIGH);

  do ZERO = digitalRead(YAW_INTERUPT_PIN); while (!ZERO);
  digitalWrite(13, LOW);
  yawTurn(false);
  delay(1000);

  //

  Serial.println("Initialized");


}


void loop(void) {

  uint16_t BL = analogRead(PHOTO_BL) + 60,
           TL = analogRead(PHOTO_TL) + 40,
           TR = analogRead(PHOTO_TR) + 25,
           BR = analogRead(PHOTO_BR);

  uint32_t up    = (TR + TL) >> 1;
  uint32_t down  = (BL + BR) >> 1;
  uint32_t right = (BR + TR) >> 1;
  uint32_t left  = (BL + TL) >> 1;

  int32_t dLR = right - left;
  int32_t dUD = down  - up;

  //  Serial.print(analogRead(PHOTO_BL)+60);
  //  Serial.print(",");
  //  Serial.print(analogRead(PHOTO_TL)+40);
  //  Serial.print(",");
  //  Serial.print(analogRead(PHOTO_TR)+25);
  //  Serial.print(",");
  //  Serial.println(analogRead(PHOTO_BR));

  if (abs(dLR) < 5) yawStop();
  else yawTurn(dLR < 1);

  if (abs(dUD) < 5) pitchStop();
  else pitchTurn(dUD < 1);

  //    if ( dUD < -1) {
  //      digitalWrite(PITCH_MOTOR_ENABLE, HIGH);
  //      digitalWrite(PITCH_MOTOR_PIN_1, LOW);
  //      digitalWrite(PITCH_MOTOR_PIN_2, HIGH);
  //    }
  //    else if (dUD > 1) {
  //      digitalWrite(PITCH_MOTOR_ENABLE, HIGH);
  //      digitalWrite(PITCH_MOTOR_PIN_1, HIGH);
  //      digitalWrite(PITCH_MOTOR_PIN_2, LOW);
  //    }
  //    else digitalWrite(PITCH_MOTOR_ENABLE, LOW);

  delay(100);

}
