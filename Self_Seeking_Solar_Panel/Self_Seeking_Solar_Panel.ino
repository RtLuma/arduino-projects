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

volatile bool CW = false;
volatile bool UNCOIL = false;
volatile uint32_t yawclk = 0, pitclk = 0;

void yawTurn(bool cw) {
  CW = cw;
  digitalWrite(YAW_MOTOR_ENABLE, HIGH);
  digitalWrite(YAW_MOTOR_PIN_2, !cw);
  digitalWrite(YAW_MOTOR_PIN_1, cw);
}

void yawStop(void) {
  digitalWrite(YAW_MOTOR_ENABLE, LOW);
  digitalWrite(YAW_MOTOR_PIN_2, LOW);
  digitalWrite(YAW_MOTOR_PIN_1, LOW);
}

void pitchTurn(bool cw) {
  CW = cw;
  digitalWrite(PITCH_MOTOR_ENABLE, HIGH);
  digitalWrite(PITCH_MOTOR_PIN_2, !cw);
  digitalWrite(PITCH_MOTOR_PIN_1, cw);
}

void pitchStop(void) {
  digitalWrite(PITCH_MOTOR_ENABLE, LOW);
  digitalWrite(PITCH_MOTOR_PIN_2, LOW);
  digitalWrite(PITCH_MOTOR_PIN_1, LOW);
}

void YAW_ISR(void) {
  digitalWrite(13, HIGH); //Make it obvious we're unwinding.
  UNCOIL = true;
  //  CW = !CW; yawTurn(CW); //Stop, drop, and yaw the other way.
  //  while (digitalRead(YAW_INTERUPT_PIN));  //Wait for switch to drop to LOW
  //  while (!digitalRead(YAW_INTERUPT_PIN)); //Wait for switch to return HIGH
  //  CW = !CW; yawStop(); //Wires should be uncoiled.
  //  digitalWrite(13, LOW); //Make it obvious we're done unwinding.
}

void setup(void) {
  Serial.begin(9600);
  digitalWrite(3, LOW); //cuz physics is gay lmao
  pinMode(13, OUTPUT);

  pinMode(YAW_INTERUPT_PIN,  INPUT);
  attachInterrupt(digitalPinToInterrupt(YAW_INTERUPT_PIN), YAW_ISR, RISING);

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

  yawStop();
  pitchStop();

  //  yawTurn(false);
  //  pitchTurn(true);

}


void loop(void) {

  uint16_t BL = analogRead(PHOTO_BL) + 60,
           TL = analogRead(PHOTO_TL) + 40,
           TR = analogRead(PHOTO_TR) + 25,
           BR = analogRead(PHOTO_BR);

  uint16_t up    = (TR + TL) >> 1,
           down  = (BL + BR) >> 1,
           right = (BR + TR) >> 1,
           left  = (BL + TL) >> 1;

  int32_t dLR = right - left,
          dUD = down  - up;

    Serial.print(analogRead(PHOTO_BL)+60);
    Serial.print(",");
    Serial.print(analogRead(PHOTO_TL)+40);
    Serial.print(",");
    Serial.print(analogRead(PHOTO_TR)+25);
    Serial.print(",");
    Serial.println(analogRead(PHOTO_BR));

  if (abs(dLR) < 5) yawStop();
  else yawTurn(dLR < 1);

  if (abs(dUD) < 3) pitchStop();
  else pitchTurn(dUD > 1);

  if (!pitclk) pitchStop();
  else pitclk += CW ? -1 : 1;
  if (pitclk > 65535) pitchStop();

  if (UNCOIL) {
    CW = !CW; yawTurn(CW); //Stop, drop, and yaw the other way.
    UNCOIL = false;
    noInterrupts();
    delay(1000);
    interrupts();
    while (!UNCOIL); //Wait for switch to return HIGH
    
    CW = !CW; yawStop(); //Wires should be uncoiled.
    digitalWrite(13, LOW); //Make it obvious we're done unwinding.
    UNCOIL = false;
  }

  delay(100);

}
