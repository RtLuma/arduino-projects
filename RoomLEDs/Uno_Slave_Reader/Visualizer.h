#define SECTION 25

//const byte hues[BANDS] = { 2, 21, 45, 115, 150, 200, 225 };   // Pretty
//const byte hues[BANDS] = { 2, 21, 35, 185, 200, 22a0, 240 };   // Sunset
const uint8_t hues[BANDS] = { 0, 37, 74, 111, 148, 185, 222 };   // Even

extern volatile uint8_t RGB[3];

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
  sendPixel(pixelAr, pixelAg, pixelAb);\
  for (uint8_t p = 1; p < SECTION; p++) {\
    dist--;\
    sendPixel(\
              ((pixelAr * dist) + (pixelBr * p)) / SECTION,\
              ((pixelAg * dist) + (pixelBg * p)) / SECTION,\
              ((pixelAb * dist) + (pixelBb * p)) / SECTION\
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
      //
      sclera[channel][band][0] = (rainbow(hue + 170)  * level) >> 10;
      sclera[channel][band][1] = (rainbow(hue + 85)   * level) >> 10;
      sclera[channel][band][2] = (rainbow(hue)        * level) >> 10;


      //      sclera[channel][band][0] = (RGB[0]  * level) >> 10;
      //      sclera[channel][band][1] = (RGB[1]   * level) >> 10;
      //      sclera[channel][band][2] = (RGB[2]        * level) >> 10;
    }
  }

  /////////////////////////////// start computing and broadcasting each pixel on the fly
  uint8_t band, dist;
  uint8_t pixelAr, pixelAg, pixelAb, pixelBr, pixelBg, pixelBb;
  uint8_t *pixelA, *pixelB;

  //  auto _t = millis();

  pixelA = sclera[0][3]; pixelB = sclera[0][2]; sendSclera
  pixelA = sclera[0][2]; pixelB = sclera[0][1]; sendSclera
  pixelA = sclera[0][1]; pixelB = sclera[0][0]; sendSclera

  pixelA = sclera[1][0]; pixelB = sclera[1][1]; sendSclera
  pixelA = sclera[1][1]; pixelB = sclera[1][2]; sendSclera
  pixelA = sclera[1][2]; pixelB = sclera[1][3]; sendSclera
  pixelA = sclera[1][3]; pixelB = sclera[1][4]; sendSclera
  pixelA = sclera[1][4]; pixelB = sclera[1][5]; sendSclera
  pixelA = sclera[1][5]; pixelB = sclera[1][6]; sendSclera

  pixelA = sclera[0][6]; pixelB = sclera[0][5]; sendSclera
  pixelA = sclera[0][5]; pixelB = sclera[0][4]; sendSclera
  pixelA = sclera[0][4]; pixelB = sclera[0][3]; sendSclera

  //  Serial.println(millis()-_t);

  uint16_t newIris[3] = { 0 };
  uint32_t irisMag = 3;

  uint16_t newPupil[3] = { 0 };
  uint32_t pupilMag = 3;


  for (band = 0; band < 2; band++) {
    newIris[0] += (sclera[0][band][0] + sclera[1][band][0]) >> 1;
    newIris[1] += (sclera[0][band][1] + sclera[1][band][1]) >> 1;
    newIris[2] += (sclera[0][band][2] + sclera[1][band][2]) >> 1;
    irisMag += (FFT.lvls[0][band] + FFT.lvls[1][band]) >> 1;
  }
  band+=3;
  for (band; band < 7; band++) {
    newPupil[0] += (sclera[0][band][0] + sclera[1][band][0]) >> 1;
    newPupil[1] += (sclera[0][band][1] + sclera[1][band][1]) >> 1;
    newPupil[2] += (sclera[0][band][2] + sclera[1][band][2]) >> 1;
    pupilMag += (FFT.lvls[0][band] + FFT.lvls[1][band]) >> 1;
  }

  newIris[0] >>= 2; newIris[1] >>= 2; newIris[2] >>= 2; //irisMag /= 3;
  irisMag = (IRIS * irisMag) >> 12;


  iris[0] = (newIris[0] + newIris[0] + newIris[0] + iris[0]) >> 2;
  iris[1] = (newIris[1] + newIris[1] + newIris[1] + iris[1]) >> 2;
  iris[2] = (newIris[2] + newIris[2] + newIris[2] + iris[2]) >> 2;

  newPupil[0] >>= 1; newPupil[1] >>= 1; newPupil[2] >>= 1; //pupilMag /= 3;
  pupilMag = (PUPIL * pupilMag) >> 12;

  pupil[0] = (newPupil[0] + newPupil[0] + newPupil[0] + pupil[0]) >> 2;
  pupil[1] = (newPupil[1] + newPupil[1] + newPupil[1] + pupil[1]) >> 2;
  pupil[2] = (newPupil[2] + newPupil[2] + newPupil[2] + pupil[2]) >> 2;

  uint8_t p = IRIS >> 2;
  for (p; p > irisMag;      p--) sendPixel2(0, 0, 0);
  for (p; p > 0;            p--) sendPixel2(iris[0], iris[1], iris[2]);
  for (p; p < irisMag;      p++) sendPixel2(iris[0], iris[1], iris[2]);
  for (p; p < (IRIS >> 1);  p++) sendPixel2(0, 0, 0);
  for (p = (IRIS >> 1); (p > (IRIS >> 2)) && (p > irisMag)  ; p--) sendPixel2(0, 0, 0);
  for (p; p > (IRIS >> 2);  p--) sendPixel2(iris[0], iris[1], iris[2]);

  //  for (p = 0; p < PUPIL; p++) sendPixel2(pupil[0], pupil[1], pupil[2]);

  p = PUPIL >> 2;
  for (p; p < pupilMag;      p++) sendPixel2(pupil[0], pupil[1], pupil[2]);
  for (p; p < (PUPIL >> 1);  p++) sendPixel2(0, 0, 0);  
  for (p; p > pupilMag;      p--) sendPixel2(0, 0, 0);
  for (p; p > 0;             p--) sendPixel2(pupil[0], pupil[1], pupil[2]);
  for (p; p < min(PUPIL >> 2, pupilMag); p++) sendPixel2(pupil[0], pupil[1], pupil[2]);
  for (p; p < (PUPIL >> 2);  p++) sendPixel2(0,0,0);
}

