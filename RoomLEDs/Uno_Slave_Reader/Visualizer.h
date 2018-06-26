#define SECTION 21
#define SECTIONODD 22

extern const uint8_t hues[BANDS];

uint8_t sclera[2][BANDS][3] = {0};
uint8_t iris[3] = { 0 };
uint8_t pupil[3] = { 0 };

#define sendSclera pixelAr = pixelA[0];\
  pixelAg = pixelA[1];\
  pixelAb = pixelA[2];\
  pixelBr = pixelB[0];\
  pixelBg = pixelB[1];\
  pixelBb = pixelB[2];\
  dist = SECTION;\
  for (uint8_t p = 0; p < SECTION; p++) {\
    dist--;\
    sendPixel(\
              ((pixelAr * dist) + (pixelBr * p)) / SECTION,\
              ((pixelAg * dist) + (pixelBg * p)) / SECTION,\
              ((pixelAb * dist) + (pixelBb * p)) / SECTION\
             );\
  }

#define sendScleraOdd pixelAr = pixelA[0];\
  pixelAg = pixelA[1];\
  pixelAb = pixelA[2];\
  pixelBr = pixelB[0];\
  pixelBg = pixelB[1];\
  pixelBb = pixelB[2];\
  dist = SECTIONODD;\
  for (uint8_t p = 0; p < SECTIONODD; p++) {\
    dist--;\
    sendPixel(\
              ((pixelAr * dist) + (pixelBr * p)) / SECTIONODD,\
              ((pixelAg * dist) + (pixelBg * p)) / SECTIONODD,\
              ((pixelAb * dist) + (pixelBb * p)) / SECTIONODD\
             );\
  }


void Chroma(void) {
  FFT.read();

  byte t = millis() >> 9;

  for (uint8_t channel = 0; channel < 2; channel++) {
    for (uint8_t band = 0; band < BANDS; band++) {

      uint8_t hue = hues[band] - t;
      //      uint8_t hue = hues[band];

      uint32_t level = FFT.lvls[channel][band] + 1;

      sclera[channel][band][0] = (rainbow(hue + 170)  * level) >> 10;
      sclera[channel][band][1] = (rainbow(hue + 85)   * level) >> 10;
      sclera[channel][band][2] = (rainbow(hue)        * level) >> 10;
    }
  }

  /////////////////////////////// start computing and broadcasting each pixel on the fly
  uint8_t band, dist;
  uint8_t pixelAr, pixelAg, pixelAb, pixelBr, pixelBg, pixelBb;
  uint8_t *pixelA, *pixelB;

  for (band = 3; band < 6; band++) {
    pixelA = sclera[0][band]; pixelB = sclera[0][band + 1]; sendSclera
  }

  pixelA = sclera[0][6]; pixelB = sclera[1][6]; sendSclera

  for (band; band > 3; band--) {
    pixelA = sclera[1][band]; pixelB = sclera[1][band - 1]; sendSclera
  }
  
  for (band; band > 0; band--) {
    pixelA = sclera[1][band]; pixelB = sclera[1][band - 1]; sendScleraOdd
  }
 
  pixelA = sclera[1][0]; pixelB = sclera[0][0]; sendSclera

  for (band; band < 3; band++) {
    pixelA = sclera[0][band]; pixelB = sclera[0][band + 1]; sendScleraOdd
  }
  

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

  newIris[0] >>= 2; newIris[1] >>= 2; newIris[2] >>= 2;

  iris[0] = (newIris[0] + newIris[0] + newIris[0] + iris[0]) >> 2;
  iris[1] = (newIris[1] + newIris[1] + newIris[1] + iris[1]) >> 2;
  iris[2] = (newIris[2] + newIris[2] + newIris[2] + iris[2]) >> 2;

  newPupil[0] >>= 1; newPupil[1] >>= 1; newPupil[2] >>= 1;

  pupil[0] = (newPupil[0] + newPupil[0] + newPupil[0] + pupil[0]) >> 2;
  pupil[1] = (newPupil[1] + newPupil[1] + newPupil[1] + pupil[1]) >> 2;
  pupil[2] = (newPupil[2] + newPupil[2] + newPupil[2] + pupil[2]) >> 2;

  for (p; p < 194; p++) sendPixel2(iris[0], iris[1], iris[2]);
  for (p; p < 298; p++) sendPixel2(pupil[0], pupil[1], pupil[2]);
}

