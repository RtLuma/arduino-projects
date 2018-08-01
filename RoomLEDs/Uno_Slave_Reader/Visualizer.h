#define SECTION_SCLERA 25
#define SECTION_IRIS 16
#define SECTION_PUPIL 8

//const byte hues[BANDS] = { 2, 21, 45, 115, 150, 200, 225 };   // Pretty
//const byte hues[BANDS] = { 2, 21, 35, 185, 200, 22a0, 240 };   // Sunset
//const uint8_t hues[BANDS] = { 0, 37, 74, 111, 148, 185, 222 };   // Even
uint8_t hues[BANDS] = { random(256), random(256), random(256), random(256), random(256), random(256), random(256) };   // Random

uint8_t sclera[2][BANDS][3] = {0};

#define visualizerBlock(DIST, PIXEL_FUNCTION)\
  r = (uint16_t)pixelA[0] << 7; r_step = pixelB[0] << 7; r_step -= r; r_step /= DIST;\
  g = (uint16_t)pixelA[1] << 7; g_step = pixelB[1] << 7; g_step -= g; g_step /= DIST;\
  b = (uint16_t)pixelA[2] << 7; b_step = pixelB[2] << 7; b_step -= b; b_step /= DIST;\
  PIXEL_FUNCTION(r>>7,g>>7,b>>7);\
  for (uint8_t p = 1; p < DIST; p++) {\
    PIXEL_FUNCTION(r>>7,g>>7,b>>7);\
    r += r_step; g += g_step; b += b_step;\
  } delayMicroseconds(10);
#define sendVisualizerPixels(WIDTH) visualizerBlock(WIDTH, sendPixel)
#define sendVisualizerPixels2(WIDTH) visualizerBlock(WIDTH, sendPixel2)

void chromaVisualizer() {
  FFT.read();
  for (uint8_t channel = 0; channel < 2; channel++) {
    for (uint8_t band = 0; band < BANDS; band++) {
      uint32_t level = FFT.lvls[channel][band] + 1;
      randomSeed(micros()); uint8_t hue = hues[band]; if (level < 50) hues[band] += random(-3, 3);
      sclera[channel][band][0] = (rainbow(hue + 170)  * level) >> 10;
      sclera[channel][band][1] = (rainbow(hue + 85)   * level) >> 10;
      sclera[channel][band][2] = (rainbow(hue)        * level) >> 10;
    }
  }
}

void solidVisualizer() {
  FFT.read();
  for (uint8_t channel = 0; channel < 2; channel++) {
    for (uint8_t band = 0; band < BANDS; band++) {
      uint32_t level = FFT.lvls[channel][band] + 1;
      sclera[channel][band][0] = (R * level) >> 10;
      sclera[channel][band][1] = (G * level) >> 10;
      sclera[channel][band][2] = (B  * level) >> 10;
    }
  }
}

void visualizer(void) {
  uint8_t *pixelA, *pixelB;
  //////// start computing and broadcasting each pixel on the fly
  uint16_t r, g, b;
  int16_t r_step, g_step, b_step;
  //  pixelA = sclera[1][1]; ]; pixelB = sclera[0][1]; sendVisualizerPixels(149)

  //  pixelA = sclera[1][3]; pixelB = sclera[1][2]; sendVisualizerPixels2(48)
  //  pixelA = sclera[0][pixelB = sclera[1][0]; sendVisualizerPixels(149)
  //  pixelA = sclera[0][02]; pixelB = sclera[0][3]; sendVisualizerPixels2(49)
  //  pixelA = sclera[0][3]; pixelB = sclera[0][4]; sendVisualizerPixels2(48)
  //  pixelA = sclera[1][4]; pixelB = sclera[1][3]; sendVisualizerPixels2(49)

  //  pixelA = sclera[1][6]; pixelB = sclera[1][5]; sendVisualizerPixels2(52)
  //  pixelA = sclera[0][5]; pixelB = sclera[0][6]; sendVisualizerPixels2(52)


  pixelA = sclera[0][3]; pixelB = sclera[0][2]; sendVisualizerPixels(SECTION_SCLERA)
  pixelA = sclera[0][2]; pixelB = sclera[0][1]; sendVisualizerPixels(SECTION_SCLERA)
  pixelA = sclera[0][1]; pixelB = sclera[1][0]; sendVisualizerPixels(SECTION_SCLERA)

  pixelA = sclera[1][0]; pixelB = sclera[1][1]; sendVisualizerPixels(SECTION_SCLERA)
  pixelA = sclera[1][1]; pixelB = sclera[1][2]; sendVisualizerPixels(SECTION_SCLERA)
  pixelA = sclera[1][2]; pixelB = sclera[1][3]; sendVisualizerPixels(SECTION_SCLERA)
  pixelA = sclera[1][3]; pixelB = sclera[1][4]; sendVisualizerPixels(SECTION_SCLERA)
  pixelA = sclera[1][4]; pixelB = sclera[1][5]; sendVisualizerPixels(SECTION_SCLERA)
  pixelA = sclera[1][5]; pixelB = sclera[1][6]; sendVisualizerPixels((SECTION_SCLERA + 1))
  //pixelA = sclera[1][5]; pixelB = sclera[1][6]; visualizerDebug((SECTION_SCLERA + 1), sendPixel)

  pixelA = sclera[0][6]; pixelB = sclera[0][5]; sendVisualizerPixels((SECTION_SCLERA - 1))
  pixelA = sclera[0][5]; pixelB = sclera[0][4]; sendVisualizerPixels(SECTION_SCLERA)
  pixelA = sclera[0][4]; pixelB = sclera[0][3]; sendVisualizerPixels(SECTION_SCLERA)

  pixelA = sclera[0][3]; pixelB = sclera[0][2]; sendVisualizerPixels2((SECTION_IRIS - 1))
  pixelA = sclera[0][2]; pixelB = sclera[0][1]; sendVisualizerPixels2(SECTION_IRIS)
  pixelA = sclera[0][1]; pixelB = sclera[0][0]; sendVisualizerPixels2(SECTION_IRIS)

  pixelA = sclera[1][0]; pixelB = sclera[1][1]; sendVisualizerPixels2((SECTION_IRIS + 1))
  pixelA = sclera[1][1]; pixelB = sclera[1][2]; sendVisualizerPixels2(SECTION_IRIS)
  pixelA = sclera[1][2]; pixelB = sclera[1][3]; sendVisualizerPixels2(SECTION_IRIS)
  pixelA = sclera[1][3]; pixelB = sclera[1][4]; sendVisualizerPixels2(SECTION_IRIS)
  pixelA = sclera[1][4]; pixelB = sclera[1][5]; sendVisualizerPixels2(SECTION_IRIS)
  pixelA = sclera[1][5]; pixelB = sclera[1][6]; sendVisualizerPixels2((SECTION_IRIS + 1))

  pixelA = sclera[0][6]; pixelB = sclera[0][5]; sendVisualizerPixels2(SECTION_IRIS)
  pixelA = sclera[0][5]; pixelB = sclera[0][4]; sendVisualizerPixels2((SECTION_IRIS + 0))
  pixelA = sclera[0][4]; pixelB = sclera[0][3]; sendVisualizerPixels2((SECTION_IRIS + 1))

  pixelA = sclera[0][3]; pixelB = sclera[0][2]; sendVisualizerPixels2(SECTION_PUPIL)
  pixelA = sclera[0][2]; pixelB = sclera[0][1]; sendVisualizerPixels2((SECTION_PUPIL + 1))
  pixelA = sclera[0][1]; pixelB = sclera[0][0]; sendVisualizerPixels2((SECTION_PUPIL + 2))

  pixelA = sclera[1][0]; pixelB = sclera[1][1]; sendVisualizerPixels2((SECTION_PUPIL + 1))
  pixelA = sclera[1][1]; pixelB = sclera[1][2]; sendVisualizerPixels2(SECTION_PUPIL)
  pixelA = sclera[1][2]; pixelB = sclera[1][3]; sendVisualizerPixels2(SECTION_PUPIL)
  pixelA = sclera[1][3]; pixelB = sclera[1][4]; sendVisualizerPixels2(SECTION_PUPIL)
  pixelA = sclera[1][4]; pixelB = sclera[1][5]; sendVisualizerPixels2(SECTION_PUPIL)
  pixelA = sclera[1][5]; pixelB = sclera[1][6]; sendVisualizerPixels2((SECTION_PUPIL + 2))

  pixelA = sclera[0][6]; pixelB = sclera[0][5]; sendVisualizerPixels2(SECTION_PUPIL)
  pixelA = sclera[0][5]; pixelB = sclera[0][4]; sendVisualizerPixels2((SECTION_PUPIL + 1))
  pixelA = sclera[0][4]; pixelB = sclera[0][3]; sendVisualizerPixels2(SECTION_PUPIL + 1)

}

