#define RESET 5
#define STROBE 4
#define BANDS 7

struct spectrumShield {

  uint16_t L[BANDS], Lmax[BANDS], R[BANDS], Rmax[BANDS];
  
  void init(void) {
    pinMode(RESET, OUTPUT);
    pinMode(STROBE, OUTPUT);
    digitalWrite(STROBE, LOW);    delay(1);
    digitalWrite(RESET, HIGH);    delay(1);
    digitalWrite(STROBE, HIGH);   delay(1);
    digitalWrite(STROBE, LOW);    delay(1);
    digitalWrite(RESET, LOW);     delay(5);
  }
  
  void read(void) {
    // Band 0 = Lowest Frequencies.
    for (byte band = 0; band < BANDS; band++)  {
      
      //Read twice and take the average by dividing by 2
      L[band] = (analogRead(0) + analogRead(0) ) >> 1;
      if (L[band] > Lmax[band]) Lmax[band] = L[band];

      R[band] = (analogRead(1) + analogRead(1) ) >> 1;
      if (R[band] > Rmax[band]) Rmax[band] = R[band];

      digitalWrite(STROBE, HIGH);
      digitalWrite(STROBE, LOW);
    }
  }
  
} FFT;
