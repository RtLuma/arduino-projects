#include <string>
#include <iostream>
#include <unistd.h>
using namespace std;

#define SECTION 100
#define BANDS 7

uint8_t sclera[2][BANDS][3] = {0};

string val2block(uint8_t val) {
  // return to_string(val) + " ";
  if (val > 223) return "\u2588";
  if (val > 191) return "\u2587";
  if (val > 159) return "\u2586";
  if (val > 127) return "\u2585";
  if (val > 95)  return "\u2584";
  if (val > 63)  return "\u2583";
  if (val > 31)  return "\u2582";
  return "_";
} 

void printColor(uint8_t r, uint8_t g, uint8_t b) {
  printf("\r%c[%dm", 0x1B, 31); cout << val2block(r);
  printf("%c[%dm", 0x1B, 32); cout << val2block(g);
  printf("%c[%dm", 0x1B, 34); cout << val2block(b);
  printf("%c[%dm", 0x1B, 0);
  fflush(stdout);
  usleep(10000);

}

#define sendVisualizerPixels(DIST)\
  r = pixelA[0]; g = pixelA[1]; b = pixelA[2];\
  printf("%d,%d,%d -> %d,%d,%d over %d steps\n",r,g,b,pixelB[0],pixelB[1],pixelB[2],SECTION);\
  cin.ignore();\
  r_step = ((int16_t)(pixelB[0] - pixelA[0]) << 8)/DIST;\
  g_step = ((int16_t)(pixelB[1] - pixelA[1]) << 8)/DIST;\
  b_step = ((int16_t)(pixelB[2] - pixelA[2]) << 8)/DIST;\
  for (uint8_t p = 0; p < DIST; p++) {\
    printColor(r>>8, g>>8, b>>8);\
    r += r_step; g += g_step; b += b_step;\
  }\
printf("\r");\
    
//----------------------------------------------------------------------- 

int main() {

  /////////////////////////////// start computing and broadcasting each pixel on the fly
  uint16_t r, g, b;
  int16_t r_step, g_step, b_step;
  uint8_t *pixelA, *pixelB;
  
  sclera[0][1][0] = 255; sclera[0][1][1] = 255; sclera[0][1][2] = 255;
  sclera[0][0][0] = 0; sclera[0][0][1] = 0; sclera[0][0][2] = 0;
  sclera[1][0][0] = 0; sclera[1][0][1] = 255; sclera[1][0][2] = 64;
  sclera[1][1][0] = 64; sclera[1][1][1] = 0; sclera[1][1][2] = 255;

  pixelA = sclera[0][0]; pixelB = sclera[0][1]; sendVisualizerPixels(SECTION)
  pixelA = sclera[0][1]; pixelB = sclera[0][0]; sendVisualizerPixels(SECTION)
  pixelA = sclera[0][0]; pixelB = sclera[1][0]; sendVisualizerPixels(SECTION)
  pixelA = sclera[1][0]; pixelB = sclera[1][1]; sendVisualizerPixels(SECTION)
  pixelA = sclera[1][1]; pixelB = sclera[1][0]; sendVisualizerPixels(SECTION)
  
}


