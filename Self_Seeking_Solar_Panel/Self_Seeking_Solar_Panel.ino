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
  pinMode(YAW_INTERUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(YAW_INTERUPT_PIN), YAW_ISR, FALLING);

  int INPUTS[] = {
    PHOTO_BR,
    PHOTO_TR,
    PHOTO_TL,
    PHOTO_BL
  };

  for (auto p : INPUTS) pinMode(p, INPUT);

  int OUTPUTS[] = {
    YAW_MOTOR_ENABLE,
    YAW_MOTOR_PIN_1,
    YAW_MOTOR_PIN_2,
    PITCH_MOTOR_ENABLE,
    PITCH_MOTOR_PIN_1,
    PITCH_MOTOR_PIN_2
  };

  for (auto p : OUTPUTS) pinMode(p, OUTPUT);

  yawStop();
  pitchStop();

  //  yawTurn(true);
  //  pitchTurn(true);

}

int PHOTO_PINS[] = { PHOTO_BL, PHOTO_BR, PHOTO_TL, PHOTO_TR };

uint16_t PHOTO[4][3] = {
  {0, 1000, 0},
  {0, 1000, 0},
  {0, 1000, 0},
  {0, 1000, 0}
};

uint16_t U, D, L, R;

void loop() {

  //  for (uint8_t i = 0; i < 4; i++) {
  //    uint16_t val = analogRead(PHOTO_PINS[i]);
  //    i f (val < PHOTO[i][1]) PHOTO[i][1] = val;
  //    if (val > PHOTO[i][2]) PHOTO[i][2] = val;
  //    PHOTO[i][0] = map(val, PHOTO[i][1], PHOTO[i][2], 0, 255);
  //    //    Serial.print(val);
  //    //    Serial.print(",");
  //  }

  if (UNCOIL) {
    pitchStop();
    noInterrupts();
    CW = !CW;
    yawTurn(CW); //Stop, drop, and yaw the other way.
    UNCOIL = false;
    delay(1000);
    interrupts();
    while (!UNCOIL); //Wait for switch to return HIGH

    CW = !CW;
    digitalWrite(13, LOW); //Make it obvious we're done unwinding.
    UNCOIL = false;
    yawTurn(CW);
    delay(100);
  }

  delay(200);

    uint16_t BL = analogRead(PHOTO_BL),
           TL = analogRead(PHOTO_TL),
           TR = analogRead(PHOTO_TR),
           BR = analogRead(PHOTO_BR);

  uint16_t u = (TR + TL) >> 1,
           d = (BL + BR) >> 1,
           r = (BR + TR) >> 1,
           l = (BL + TL) >> 1;

  bool bU = U - u > 0,
       bD = D - d > 0,
       bL = L - l > 0,
       bR = R - r > 0;

  if (bU && bD) pitchStop();
  else pitchTurn(bU);

  if (bL && bR) yawStop();
  else yawTurn(bR);

  U = u;
  D = d;
  L = l;
  R = r;


  //  if (!pitclk) pitchStop();
  //  else pitclk += CW ? -1 : 1;
  //  if (pitclk > 65535) pitchStop();



  //  Serial.println(digitalRead(YAW_INTERUPT_PIN));

  //  if (millis() > 4000) {
  //    Serial.print(up);
  //    Serial.print(",");
  //    Serial.print(down);
  //    Serial.print(",");
  //    Serial.print(left);
  //    Serial.print(",");
  //    Serial.println(right);
  //  }

}
