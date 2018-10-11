#include <sys/ioctl.h>  //terminal width
#include <unistd.h>		//usleep
#include <time.h>       //time 
#include "monoring.hpp"     //the linked list


using namespace std;
using voidF = void(*)(void) ;

struct winsize w;

uint16_t SPARKLES = 10;
uint16_t PIXELS;
uint8_t R, G, B;
voidF display;

#define DISCRETE false
#define TRICHROMATIC true

int main() {
    srand(time(NULL));
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    PIXELS = w.ws_col-2;
    srand(time(NULL));
    R = rand(); G = rand(); B = rand();
    
    // MonoRing leds;
    mleds.populate(SPARKLES);
    
    uint8_t mode = ((uint8_t)TRICHROMATIC << 1) + (uint8_t)DISCRETE;
    
    switch (mode) {
        case 0: //0b00
            display = monoContinuous;
            break;
    
        case 1: //0b01
            display = monoDiscrete;
            break;
            
        case 2: //0b10
            display = monoContinuous;
            break;
            
        case 3: //0b11
            display = monoDiscrete;
            break;
    
        default:
            break;
    }
    
    while (true) {
        printf("\r[");
        display();
        printf("]");
        fflush(stdout);
        usleep(20000);
        printf("\r");
    }
}