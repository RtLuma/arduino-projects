include <string>
using namespace std;

uint8_t rainbow(uint8_t hue) {
  if (hue > 170) return ~(hue + (hue << 1));
  if (hue > 85) return hue + (hue << 1);
  return 0;
}

#define SECTION 21
#define SECTIONODD 22
#define BANDS 7
#define SECTION 22

const uint8_t hues[BANDS] = { 0, 37, 74, 111, 148, 185, 222 };   // Even

void tick() { for}

uint8_t sclera[2][BANDS][3] = {0};

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

void Chroma(void) {

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
    newIris[1] += (sclera[0][band][1] + sclera[1][band][1]) >> 1;
    newIris[2] += (sclera[0][band][2] + sclera[1][band][2]) >> 1;
  }
  band++;
  for (band; band < 7; band++) {
    newPupil[0] += (sclera[0][band][0] + sclera[1][band][0]) >> 1;
    newPupil[1] += (sclera[0][band][1] + sclera[1][band][1]) >> 1;
    newPupil[2] += (sclera[0][band][2] + sclera[1][band][2]) >> 1;
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



int main() {
    
}