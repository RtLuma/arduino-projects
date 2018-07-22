#define RESET 5
#define STROBE 4
#define BANDS 7
#define MAX_CUTOFF 200
#define MIN_CUTOFF 50

struct spectrumShield {

  uint16_t lvls[2][BANDS] = {{0}, {0}};
  uint16_t maxs[2][BANDS] = {{MAX_CUTOFF + 1}, {MAX_CUTOFF + 1}};

  void begin(void) {
    pinMode(RESET,  OUTPUT);
    pinMode(STROBE, OUTPUT);
    digitalWrite(STROBE, LOW);  delay(1);
    digitalWrite(RESET,  HIGH); delay(1);
    digitalWrite(STROBE, HIGH); delay(1);
    digitalWrite(STROBE, LOW);  delay(1);
    digitalWrite(RESET,  LOW);  delay(5);

    for (byte band = 0; band < BANDS; band++)  {
      maxs[0][band] = MAX_CUTOFF + 1;
      maxs[1][band] = MAX_CUTOFF + 1;
    }
  }

  void read(void) {
    for (byte band = 0; band < BANDS; band++)  {
      for (byte channel = 0; channel < 2; channel++) {

        if (maxs[channel][band] > MAX_CUTOFF) maxs[channel][band] -= 2;
        if (lvls[channel][band] > 8) { // Fade by subtracting a proportion
          lvls[channel][band] -= (lvls[channel][band] / 6);
        }
        else lvls[channel][band] = 0;

        uint16_t level = (analogRead(channel) + analogRead(channel)) >> 1;
        //        level = 1023;
        //        level = level > MIN_CUTOFF ? level - MIN_CUTOFF : 0;
        level = level > MIN_CUTOFF ? map(level, MIN_CUTOFF, 1023, 0, 1023) : 0;
        //level = 1023;

        uint32_t ratio = 1023;

        if (level < maxs[channel][band]) {
          for (byte power = 0; power < 7; power++) {
            ratio *= level; // Raise ratio to power without overflow
            ratio /= maxs[channel][band];
          }
        }
        else maxs[channel][band] = level;

        if (ratio > lvls[channel][band]) {
          lvls[channel][band] = ratio;
          //lvls[channel][band] += ratio + (ratio << 1); lvls[channel][band] >>= 2;
          //lvls[channel][band] += ratio; lvls[channel][band] >>= 1;
        }

      }

      digitalWrite(STROBE, HIGH);
      digitalWrite(STROBE, LOW);
    }
    //Average top and bottom bands for visual consistency
    lvls[0][0] = lvls[1][0] = (lvls[0][0] + lvls[1][0]) >> 1;
    lvls[0][BANDS - 1] = lvls[1][BANDS - 1] = (lvls[0][BANDS - 1] + lvls[1][BANDS - 1]) >> 1;
  }
} FFT;

