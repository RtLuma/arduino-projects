#include <string>
#include <iostream>
using namespace std;

#define SECTION 50
#define BANDS 7

uint8_t sclera[2][BANDS][3] = {0};

string val2block(uint8_t val) {
  float rat = (float)val/255.0;
  // return to_string(val) + " ";
  if (rat > 0.9)    return "\u2588";
  if (rat > 7.0/8.0) return "\u2587";
  if (rat > 6.0/8.0) return "\u2586";
  if (rat > 5.0/8.0) return "\u2585";
  if (rat > 4.0/8.0) return "\u2584";
  if (rat > 3.0/8.0) return "\u2584";
  if (rat > 2.0/8.0) return "\u2583";
  if (rat > 1.0/8.0) return "\u2582";
    return ".";
}

void printColor(uint8_t c, uint8_t ansi) {
  printf("%c[%dm", 0x1B, ansi);
  cout << val2block(c);
  printf("%c[%dm", 0x1B, 0);
}

#define sendVisualizerPixels(DIST)\
  r = pixelA[0] << 8;\
  g = pixelA[1] << 8;\
  b = pixelA[2] << 8;\
  printf("%d,%d,%d -> %d,%d,%d over %d steps:\n",r>>8,g>>8,b>>8,pixelB[0],pixelB[1],pixelB[2],SECTION);\
  r_step = (((uint16_t)pixelB[0] << 8) - ((uint16_t)pixelA[0] << 8))/DIST;\
  g_step = (((uint16_t)pixelB[1] << 8) - ((uint16_t)pixelA[1] << 8))/DIST;\
  b_step = (((uint16_t)pixelB[2] << 8) - ((uint16_t)pixelA[2] << 8))/DIST;\
  printColor(r>>8, 31);\
  for (uint8_t p = 1; p < DIST; p++) { r += r_step; printColor(r>>8, 31); } printf("\n");\
  printColor(g>>8, 32);\
  for (uint8_t p = 1; p < DIST; p++) { g += g_step; printColor(g>>8, 32); } printf("\n");\
  printColor(b>>8, 34);\
  for (uint8_t p = 1; p < DIST; p++) { b += b_step; printColor(b>>8, 34); } printf("\n");\
  cout << endl<<endl;\
    
//----------------------------------------------------------------------- 

int main() {

  /////////////////////////////// start computing and broadcasting each pixel on the fly
  uint16_t r, g, b;
  int16_t r_step, g_step, b_step;
  uint8_t *pixelA, *pixelB;
  
  sclera[0][1][0] = 255; sclera[0][1][1] = 64; sclera[0][1][2] = 0;
  sclera[0][0][0] = 0; sclera[0][0][1] = 0; sclera[0][0][2] = 0;
  sclera[1][0][0] = 0; sclera[1][0][1] = 255; sclera[1][0][2] = 64;
  sclera[1][1][0] = 64; sclera[1][1][1] = 0; sclera[1][1][2] = 255;

  pixelA = sclera[0][0]; pixelB = sclera[0][1]; sendVisualizerPixels(SECTION)
  pixelA = sclera[0][1]; pixelB = sclera[0][0]; sendVisualizerPixels(SECTION)
  pixelA = sclera[0][0]; pixelB = sclera[1][0]; sendVisualizerPixels(SECTION)
  pixelA = sclera[1][0]; pixelB = sclera[1][1]; sendVisualizerPixels(SECTION)
  pixelA = sclera[1][1]; pixelB = sclera[1][0]; sendVisualizerPixels(SECTION)
  
}


