#define RESET 5
#define STROBE 4
#define BANDS 7
#define MAX_CUTOFF 200
#define MIN_CUTOFF 90

struct spectrumShield {

  uint16_t lvls[2][BANDS] = {{0}};
  uint16_t maxs[2][BANDS] = {{MAX_CUTOFF}};

  void begin(void) {
    pinMode(RESET, OUTPUT);
    pinMode(STROBE, OUTPUT);
    digitalWrite(STROBE, LOW);    delay(1);
    digitalWrite(RESET, HIGH);    delay(1);
    digitalWrite(STROBE, HIGH);   delay(1);
    digitalWrite(STROBE, LOW);    delay(1);
    digitalWrite(RESET, LOW);     delay(5);
    for (byte band = 0; band < BANDS; band++)  {
      maxs[0][band] = MAX_CUTOFF + 1;
      maxs[1][band] = MAX_CUTOFF + 1;
    }
  }

  void read(void) {
    for (byte band = 0; band < BANDS; band++)  {
      for (byte channel = 0; channel < 2; channel++) {

        if (maxs[channel][band] > MAX_CUTOFF) maxs[channel][band]--;
        if (lvls[channel][band] > 8) lvls[channel][band] -= lvls[channel][band] >> 3;  // Fade by subtracting a proportion
        else lvls[channel][band] = 0;

        uint16_t level = (analogRead(channel) + analogRead(channel)) >> 1;
        level = level > MIN_CUTOFF ? map(level, MIN_CUTOFF, 1023, 0, 1023) : 0;

        uint32_t ratio = 1023;

        if (level < maxs[channel][band]) {
          for (byte power = 0; power < 5; power++) {
            ratio *= level; // Raise ratio to power without overflow
            ratio /= maxs[channel][band];
          }
        }
        else maxs[channel][band] = level;

        if (ratio > lvls[channel][band]) {
          lvls[channel][band] = ratio;
          //          lvls[channel][band] >>= 1;
        }

      }

      digitalWrite(STROBE, HIGH);
      digitalWrite(STROBE, LOW);


      //      Serial.print((1023 * (lvls[0][band] + 1)) >> 10);
      //      Serial.print("/");
      //      Serial.print(maxs[0][band]);
      //      Serial.print("\t");
    }
    //    Serial.println();
  }
} FFT;

