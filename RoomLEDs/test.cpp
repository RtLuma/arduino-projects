// There's also this too https://github.com/ivanseidel/LinkedList

#include<iostream>
#include <string>
#include <unistd.h>		//usleep
#include <stdlib.h>		//srand, rand
#include <time.h>       //time 
#include <memory>		//unique_ptr
#include <assert.h>

using namespace std;

#define ZERO_SYMBOL " "
#define FORE(START, END) for (int i=START; i < END; i++)

string val2block(uint8_t val) {
  // return to_string(val) + " ";
  if (val > 226) return "\u2588";
  if (val > 198) return "\u2587";
  if (val > 169) return "\u2586";
  if (val > 141) return "\u2585";
  if (val > 113) return "\u2584";
  if (val > 84)  return "\u2583";
  if (val > 56)  return "\u2582";
  if (val > 28)  return "\u2581";
  				 return ZERO_SYMBOL;
} 

void printColor(uint8_t r, uint8_t g, uint8_t b) {
  printf("\r%c[%dm", 0x1B, 31); cout << val2block(r);
  printf("%c[%dm", 0x1B, 32); cout << val2block(g);
  printf("%c[%dm", 0x1B, 34); cout << val2block(b);
  printf("%c[%dm", 0x1B, 0);
  fflush(stdout);
  usleep(20000);
}
//]]]]

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void insertionSort(int arr[], int length) {
      int i, j, tmp;
      for (i = 1; i < length; i++) {
            j = i;
            while (j > 0 && arr[j - 1] > arr[j]) {
                  tmp = arr[j];
                  arr[j] = arr[j - 1];
                  arr[j - 1] = tmp;
                  j--;
            }
      }
}

int lsr(int x, int n) {  return (int)((unsigned int)x >> n); }


void printArray(uint32_t arr[], int length) {
    // cout << "{ ";
    FORE(0, length) {
		uint16_t pos = arr[i] >> 16;
		int16_t mag = arr[i] & 0xFFFF;
		cout << "[" << to_string(pos) << ":" << to_string(mag) << "], ";
	}
    // cout << " }\n";
	cout << endl;
}

//Store whether fading in or out in high bit of pos

#define SCLERA 300
#define SPARKLES 25

int main() {
	srand(time(NULL)); rand();
	uint8_t DYNAMIC_SIZE;
	uint32_t *leds;

	do { DYNAMIC_SIZE =  rand()%SPARKLES; } while (DYNAMIC_SIZE < 5);
	leds = new uint32_t [DYNAMIC_SIZE];
	
	FORE(0,DYNAMIC_SIZE) {
		uint32_t pos = (rand()%SCLERA);
		uint16_t mag = rand();
		cout << "[" << to_string(pos) << ":" << to_string((int16_t)mag) << "], ";
		pos <<= 16;
		leds[i]= pos | mag;
	}
	cout << endl;
	
	printArray(leds, DYNAMIC_SIZE);
}


