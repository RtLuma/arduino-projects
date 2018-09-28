#include <string>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>		//usleep
#include <stdlib.h>		//srand, rand
#include <time.h>     //time 
using namespace std;

#define SECTION 83
#define BANDS 7

uint8_t rainbow(uint8_t hue) { if (hue > 170) return ~(hue + (hue << 1)); if (hue > 85) return hue + (hue << 1); return 0; }
void rainbowRGB(uint8_t rgb[3], uint8_t hue) { rgb[0] = rainbow(hue+170); rgb[1] = rainbow(hue+85); rgb[2] = rainbow(hue); }
string val2block(uint8_t val) {  if (val > 223) return "\u2588";  if (val > 191) return "\u2587";  if (val > 159) return "\u2586";  if (val > 127) return "\u2585";  if (val > 95)  return "\u2584";  if (val > 63)  return "\u2583";  if (val > 31)  return "\u2582";  return " ";} 

void printColor(uint8_t r, uint8_t g, uint8_t b) { printf("\033[38;2;%d;%d;%dm█\033[0;m", r, g, b); fflush(stdout); usleep(1000); } //]]

void printLine(uint8_t* A, uint8_t* B, uint16_t DIM) {
  uint16_t r, g, b;
  int16_t r_step, g_step, b_step;
  r = A[0] << 8; g = A[1] << 8; b = A[2] << 8;
  r_step = (((uint16_t)B[0] << 8) - r)/DIM;
  g_step = (((uint16_t)B[1] << 8) - g)/DIM;
  b_step = (((uint16_t)B[2] << 8) - b)/DIM;
 for (uint16_t p = 0; p < DIM; p++) { printColor(r>>8, g>>8, b>>8); r += r_step; g += g_step; b += b_step; }
 printf("\n");
}

void printSquare(uint8_t* A, uint8_t* B, uint16_t DIM) {
  uint16_t r, g, b;
  int16_t r_step, g_step, b_step;
  r = A[0] << 8; g = A[1] << 8; b = A[2] << 8;
  r_step = (((uint16_t)B[0] << 8) - r)/DIM;
  g_step = (((uint16_t)B[1] << 8) - g)/DIM;
  b_step = (((uint16_t)B[2] << 8) - b)/DIM;
  for (uint16_t i=0; i < DIM>>1; i++) { 
    A[0] = r >> 8; A[1] = g >> 8; A[2] = b >> 8;
    B[0] = ~(r >> 8); B[1] = ~(g >> 8); B[2] = ~(b >> 8);
    printLine(A, B, DIM);
    r += r_step; g += g_step; b += b_step;
    r += r_step; g += g_step; b += b_step;
  }
 printf("\n");
}
  
//----------------------------------------------------------------------- 

struct winsize w;

int main() {  
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  srand(time(NULL)); rand();
  uint8_t A[3], B[3], hue;
  
  while(true) {
    A[0]=rand(); A[1]=rand(); A[2]=rand(); 
    B[0]=rand(); B[1]=rand(); B[2]=rand(); 
    uint16_t DIM = min(w.ws_row, w.ws_col);
    printColor(A[0],A[1],A[2]); printf(" -> "); printColor(B[0],B[1],B[2]);  printf(" over %d steps:\n",DIM);
    printSquare(A, B, DIM);    
    
    
    usleep(200000);
 }

    return 0;
  
}


