#define SECTION 25

extern const uint8_t hues[BANDS];

uint8_t sclera[2][BANDS + 1][3] = {0};
uint8_t iris[3] = { 0 };
uint8_t pupil[3] = { 0 };


void Chroma(void) {
  FFT.read();

  byte t = millis() >> 9;

  for (uint8_t channel = 0; channel < 2; channel++) {
    for (uint8_t band = 0; band < BANDS; band++) {

      uint8_t hue = hues[band] - t;

      uint32_t level = FFT.lvls[channel][band] + 1;

      sclera[channel][band][0] = (rainbow(hue + 170)  * level) >> 10;
      sclera[channel][band][1] = (rainbow(hue + 85)   * level) >> 10;
      sclera[channel][band][2] = (rainbow(hue)        * level) >> 10;
    }

    //Prevent segfault
    sclera[channel][BANDS][0] = sclera[channel][BANDS - 1][0];
    sclera[channel][BANDS][1] = sclera[channel][BANDS - 1][1];
    sclera[channel][BANDS][2] = sclera[channel][BANDS - 1][2];
  }

  /////////////////////////////// start computing and broadcasting each pixel on the fly

  int8_t nextband;
  uint8_t dist, band;
  uint8_t pixelAr, pixelAg, pixelAb, pixelBr, pixelBg, pixelBb;
  uint8_t *pixelA, *pixelB;


  for (band = 0; band < BANDS; band++) {

    dist = SECTION;

    pixelA = sclera[0][band];
    pixelB = sclera[0][band + 1];

    pixelAr = pixelA[0];
    pixelAg = pixelA[1];
    pixelAb = pixelA[2];

    pixelBr = pixelB[0];
    pixelBg = pixelB[1];
    pixelBb = pixelB[2];

    for (uint8_t p = 0; p < SECTION; p++) {
      dist--;
      sendPixel(
        ((pixelAr * dist) + (pixelBr * p)) / SECTION,
        ((pixelAg * dist) + (pixelBg * p)) / SECTION,
        ((pixelAb * dist) + (pixelBb * p)) / SECTION
      );
    }
  }

  for (band = BANDS - 1; band > 0; band--) {
    dist = SECTION;

    pixelA = sclera[1][band];
    pixelB = sclera[1][band - 1];

    pixelAr = pixelA[0];
    pixelAg = pixelA[1];
    pixelAb = pixelA[2];

    pixelBr = pixelB[0];
    pixelBg = pixelB[1];
    pixelBb = pixelB[2];

    for (uint8_t p = 0; p < SECTION; p++) {
      dist--;
      sendPixel(
        ((pixelAr * dist) + (pixelBr * p)) / SECTION,
        ((pixelAg * dist) + (pixelBg * p)) / SECTION,
        ((pixelAb * dist) + (pixelBb * p)) / SECTION
      );
    }
  }
  
  
  
  //sendPixel(sclera[1][0][0], sclera[1][0][1], sclera[1][0][2]);



  uint16_t newIris[3] = { 0 };
  uint16_t newPupil[3] = { 0 };
  band = 0;
  uint16_t p = 0;

  for (band; band < 3; band++) {
    newIris[0] += (sclera[0][band][0] + sclera[1][band][0]) >> 1;
    newIris[1] += (sclera[1][band][1] + sclera[1][band][1]) >> 1;
    newIris[2] += (sclera[2][band][2] + sclera[1][band][2]) >> 1;
  }
  for (band; band < 7; band++) {
    newPupil[0] += (sclera[0][band][0] + sclera[1][band][0]) >> 1;
    newPupil[1] += (sclera[1][band][1] + sclera[1][band][1]) >> 1;
    newPupil[2] += (sclera[2][band][2] + sclera[1][band][2]) >> 1;
  }

  iris[0] = ((newIris[0] >> 2) + iris[0]) >> 1;
  iris[1] = ((newIris[1] >> 2) + iris[1]) >> 1;
  iris[2] = ((newIris[2] >> 2) + iris[2]) >> 1;

  pupil[0] = ((newPupil[0] >> 2) + pupil[0]) >> 1;
  pupil[1] = ((newPupil[1] >> 2) + pupil[1]) >> 1;
  pupil[2] = ((newPupil[2] >> 2) + pupil[2]) >> 1;

  for (p; p < 200; p++) sendPixel2(iris[0], iris[1], iris[2]);
  for (p; p < 300; p++) sendPixel2(pupil[0], pupil[1], pupil[2]);
}

