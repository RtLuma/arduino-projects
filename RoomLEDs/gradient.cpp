#include <string>
#include <iostream>
#include <unistd.h>		//usleep
#include <stdlib.h>		//srand, rand
#include <time.h>     //time 
using namespace std;

#define SECTION 100
#define BANDS 7

uint8_t rainbow(uint8_t hue) { if (hue > 170) return ~(hue + (hue << 1)); if (hue > 85) return hue + (hue << 1); return 0; }
void rainbowRGB(uint8_t rgb[3], uint8_t hue) { rgb[0] = rainbow(hue+170); rgb[1] = rainbow(hue+85); rgb[2] = rainbow(hue); }
string val2block(uint8_t val) {  if (val > 223) return "\u2588";  if (val > 191) return "\u2587";  if (val > 159) return "\u2586";  if (val > 127) return "\u2585";  if (val > 95)  return "\u2584";  if (val > 63)  return "\u2583";  if (val > 31)  return "\u2582";  return " ";} 

void printColor(uint8_t r, uint8_t g, uint8_t b) { printf("\033[38;2;%d;%d;%dm█\033[0;m", r, g, b); fflush(stdout); } //]]

void gradient(uint8_t* pixelA, uint8_t* pixelB) {
  uint16_t r, g, b;
  int16_t r_step, g_step, b_step;
  printColor(pixelA[0],pixelA[1],pixelA[2]); printf(" -> "); printColor(pixelB[0],pixelB[1],pixelB[2]);  printf(" over %d steps:\n",SECTION);
  r = pixelA[0] << 8; g = pixelA[1] << 8; b = pixelA[2] << 8;
  r_step = (((uint16_t)pixelB[0] << 8) - ((uint16_t)pixelA[0] << 8))/SECTION;
  g_step = (((uint16_t)pixelB[1] << 8) - ((uint16_t)pixelA[1] << 8))/SECTION;
  b_step = (((uint16_t)pixelB[2] << 8) - ((uint16_t)pixelA[2] << 8))/SECTION;
 for (uint8_t p = 0; p < SECTION; p++) { printColor(r>>8, g>>8, b>>8); r += r_step; g += g_step; b += b_step; }
 printf("\n");
}
    
//----------------------------------------------------------------------- 

int main() {  
  srand(time(NULL)); rand();
  uint8_t A[3], B[3], hue;
  while(true) {
    hue = rand();
    rainbowRGB(A, hue);
    hue = rand();
    rainbowRGB(B, hue);
    gradient(A, B);
    usleep(1000000);
 }
  
}


