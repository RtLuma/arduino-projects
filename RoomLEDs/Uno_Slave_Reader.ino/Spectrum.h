#define RESET 5
#define STROBE 4
#define BANDS 7
#define MAX_CUTOFF 12800
#define MIN_CUTOFF 3200
#define MEMORY 5


//To-do:  Undo high-byte loud decay?


struct spectrumShield {

  uint32_t L[BANDS] = {0}, R[BANDS] = {0};
  uint32_t Lmax[BANDS] = {MAX_CUTOFF}, Rmax[BANDS] = {MAX_CUTOFF};
  uint32_t Llast[BANDS][MEMORY-2] = {{0}}, Rlast[BANDS][MEMORY-2] = {{0}};

  void begin(void) {
    pinMode(RESET, OUTPUT);
    pinMode(STROBE, OUTPUT);
    digitalWrite(STROBE, LOW);    delay(1);
    digitalWrite(RESET, HIGH);    delay(1);
    digitalWrite(STROBE, HIGH);   delay(1);
    digitalWrite(STROBE, LOW);    delay(1);
    digitalWrite(RESET, LOW);     delay(5);
    for (byte band = 0; band < BANDS; band++)  {
      Lmax[band] = MAX_CUTOFF + 1;
      Rmax[band] = MAX_CUTOFF + 1;
    }
  }

  void read(void) {
    // Band 0 = Lowest Frequencies.
    for (byte band = 0; band < BANDS; band++)  {

      if (Lmax[band] > MAX_CUTOFF) Lmax[band] -= 64;
      if (Rmax[band] > MAX_CUTOFF) Rmax[band] -= 64;

      uint16_t val = analogRead(0);
      if (val > 60) {

        // YOU FUCKING DUMB ASS
        
        L[band] = (Llast[band][2] + Llast[band][1] + Llast[band][0] + L[band] + (map(val, 60, 1023, 0, 1023) << 6)) / MEMORY;
        //        L[band] = (Llast[band][0] + L[band] + (map(val, 60, 1023, 0, 1023) << 6)) / 3;
        if (L[band] > Lmax[band]) Lmax[band] = L[band];
      }
      else L[band] = 0;

      for (char m=MEMORY-3; m > 0; m--) Llast[band][m] = Llast[band][m-1];
      
      Llast[band][0] = L[band];

      val = analogRead(1);
      if (val > 60) {
        R[band] = (Rlast[band][2] + Rlast[band][1] + Rlast[band][0] + R[band] + (map(val, 60, 1023, 0, 1023) << 6)) / MEMORY;
        //        R[band] = (Rlast[band][0] + R[band] + (map(val, 60, 1023, 0, 1023) << 6)) / 3;
        if (R[band] > Rmax[band]) Rmax[band] = R[band];
      }
      else R[band] = 0;

      for (char m=MEMORY-3; m > 0; m--) Rlast[band][m] = Rlast[band][m-1];      

      Rlast[band][0] = R[band];

      digitalWrite(STROBE, HIGH);
      digitalWrite(STROBE, LOW);

      //      Serial.print(L[band] >> 6);
      //      Serial.print("/");
      //      Serial.print(Lmax[band] >> 6);
      //      Serial.print("\t");
    }
    //    Serial.println();
  }
} FFT;
