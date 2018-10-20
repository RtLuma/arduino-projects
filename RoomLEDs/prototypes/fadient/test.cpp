#include <sys/ioctl.h>  //terminal width
#include <unistd.h>		//usleep
#include <time.h>       //time 
#include "ring.hpp"     //the linked loop???? 


using namespace std;
using voidF = void(*)(void) ;

struct winsize w;

uint8_t P = 20;
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
    // P = PIXELS/10;
    P = 20;
    srand(time(NULL));
    R = rand(); G = rand(); B = rand();
    Ring ring(P);
    ring.populate(P+2);
    printf("%d\n", ring.nodes);
    ring.populate(P-2);
    printf("%d\n", ring.nodes);
    printf("%d\n", ring.ring[2]->pos);
    
}