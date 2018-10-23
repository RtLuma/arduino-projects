#include <sys/ioctl.h>  //terminal width
#include <unistd.h>		//usleep
#include <time.h>       //time 
#include "monoring.hpp"     //the linked list
#include "rgbring.hpp"     //the linked list


using namespace std;
using voidF = void(*)(void) ;

struct winsize w;

uint16_t SPARKLES = 20;
uint16_t PIXELS;
uint8_t R, G, B;
voidF display;

#define DISCRETE false
#define TRICHROMATIC true

//https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html

int main() {
    srand(time(NULL));
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    PIXELS = w.ws_col-2;
    SPARKLES = PIXELS/10;
    srand(time(NULL));
    R = rand(); G = rand(); B = rand();
    
    uint8_t mode = ((uint8_t)TRICHROMATIC << 1) + (uint8_t)DISCRETE;
    
    switch (mode) {
        case 0: //0b00
            display = monoContinuous;
            mleds.populate(SPARKLES);
            break;
    
        case 1: //0b01
            display = monoDiscrete;
            mleds.populate(SPARKLES);
            break;
            
        case 2: //0b10
            display = rgbContinuous;
            cleds.populate(SPARKLES);
            cleds.scramble();
            break;
            
        case 3: //0b11
            display = rgbDiscrete;
            cleds.populate(SPARKLES);
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