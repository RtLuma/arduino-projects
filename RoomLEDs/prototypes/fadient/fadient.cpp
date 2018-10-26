#include <sys/ioctl.h>  //terminal width
#include <unistd.h>		//usleep
#include <time.h>       //time 
#include "rgb.hpp"     //the linked list


using namespace std;
using voidF = void(*)(void) ;

struct winsize w;

uint16_t SPARKLES;
uint16_t PIXELS;
uint8_t R, G, B;
voidF display;

#define DISCRETE false
#define TRICHROMATIC true

//https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html

int main() {
    srand(time(NULL));
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
<<<<<<< HEAD
    PIXELS = w.ws_col-2;
    SPARKLES = PIXELS/10;
    // SPARKLES = 21;
=======
    PIXELS = w.ws_col;
    // SPARKLES = PIXELS/10;
    SPARKLES = 21;
>>>>>>> b511d680a7ae3708a3fc1b8a8fda904f7543ed18
    srand(time(NULL));
    R = 255; G = 64; B = 32;
    // R = rand(); G = rand(); B = rand();
    
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
            rgbing.populate(SPARKLES);
            break;
            
        case 3: //0b11
            display = rgbContinuous;
            rgbing.populate(SPARKLES);
            break;
    
        default:
            break;
    }
    
    while (true) {
        display();
        fflush(stdout);
<<<<<<< HEAD
        usleep(50000);
=======
        usleep(25000);
>>>>>>> b511d680a7ae3708a3fc1b8a8fda904f7543ed18
        printf("\n");
    }
}