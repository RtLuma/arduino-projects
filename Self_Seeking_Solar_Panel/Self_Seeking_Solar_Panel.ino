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

#define HISTORY 20

uint16_t history[HISTORY] = {0};
uint32_t sum = 0;

void setup(void) {
  Serial.begin(9600);
  interrupts();
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

int16_t U = 0, D = 0, L = 0, R = 0;

void loop() {

  for (uint8_t i = 0; i < 4; i++) {
    uint16_t val = analogRead(PHOTO_PINS[i]);
    if (val < PHOTO[i][1]) PHOTO[i][1] = val;
    if (val > PHOTO[i][2]) PHOTO[i][2] = val;
    PHOTO[i][0] = map(val, PHOTO[i][1], PHOTO[i][2], 0, 255);
  }

  int16_t u = (PHOTO[2][0] + PHOTO[3][0]) >> 1,
          d = (PHOTO[0][0] + PHOTO[1][0]) >> 1,
          r = (PHOTO[1][0] + PHOTO[3][0]) >> 1,
          l = (PHOTO[0][0] + PHOTO[2][0]) >> 1;

  uint32_t avg = u + d + l + r;
  avg >>= 2;

  sum -= history[0];
  for (uint8_t j = 1; j < HISTORY; j++) history[j - 1] = history[j];
  history[HISTORY - 1] = avg;
  sum += avg;


  uint32_t stdsum = 0;
  for (int i = 0; i < HISTORY; i++) stdsum += sq(avg - history[i]);

  float stdev = sqrt(stdsum / float(HISTORY));

  Serial.println(stdev);

  //  Serial.print(u);
  //  Serial.print(",");
  //  Serial.print(d);
  //  Serial.print(",");
  //  Serial.print(l);
  //  Serial.print(",");
  //  Serial.println(r);

  pitchTurn(u - d > 0);
  yawTurn(r - l > 0);

  U = u;
  D = d;
  L = l;
  R = r;


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
    delay(300);
  }

  delay(100);
}
