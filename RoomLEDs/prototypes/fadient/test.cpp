#include <sys/ioctl.h>  //terminal width
#include <unistd.h>		//usleep
#include <time.h>       //time 
#include "ring.hpp"     //the linked loop???? 

using namespace std;
using voidF = void(*)(void) ;

struct winsize w;

uint16_t PIXELS;
uint8_t R, G, B, P;
voidF display;

#define DISCRETE false
#define TRICHROMATIC true
//https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html

int main() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    PIXELS = w.ws_col-2;
    srand(time(NULL));

    // P = PIXELS/10;
    P = 20;
    R = rand(); G = rand(); B = rand();
    Ring ring(P);
    
    // Test node bitfield getters & setters work as expected
    uint32_t n;
    ring.col(n, 197); ring.lum(n, 230); ring.pos(n, 254);
    if (ring.col(n) != 197) { printf("dicks0\n"); exit(1); }
    if (ring.lum(n) != 230) { printf("dicks0\n"); exit(1); }
    if (ring.pos(n) != 254) { printf("dicks0\n"); exit(1); }
        
    // Make sure we can construct the dang thing
    uint8_t coP = P;
    ring.populate(coP);
    if (ring.nodes != P) { printf("dicks1\n"); exit(1); }
    
    // Test that sorting works
    for (uint8_t i=0; i < P; i++) {
        uint32_t n;
        ring.col(n, rand()%3); ring.lum(n, rand()>>24); ring.pos(n, rand()>>16);
        ring.ring[i]=n;
    };
    ring.sort();
    for (uint8_t i=1; i < P; i++)
        if (ring.ring[i] < ring.ring[i-1]) { printf("dicks2\n"); exit(1); }
    
    //Try "resizing"
    uint32_t before = ring.ring[5];
    ring.populate(255);
    if (ring.ring[254] != 0) { printf("dicks3\n"); exit(1); }
    
    ring.pos(ring.ring[254], 200);
    uint32_t after = ring.ring[5];
    if (before != after) { printf("dicks4\n"); exit(1); }
    
    ring.populate(P-1); //Should zero mem
    if (ring.ring[P-1] != 0) { printf("dicks5\n"); exit(1); }
    
    
 
}