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
    P = PIXELS/10;
    // P = 20;
    R = rand(); G = rand(); B = rand();
    Ring ring;
    ring.init(P);
    ring.print();
    
    // Test node bitfield getters & setters work as expected
    // These should be idempotent
    uint32_t n;
    uint16_t pos = rand()%PIXELS;
    uint8_t  hue = rand()>>24;
    uint8_t  lum = rand()>>24;
    Ring::hue(n, hue); Ring::lum(n, lum); Ring::pos(n, pos);
    if (Ring::hue(n) != hue) { printf("Node ops\n"); exit(1); }
    if (Ring::lum(n) != lum) { printf("Node ops\n"); exit(1); }
    if (Ring::pos(n) != pos) { printf("Node ops\n"); exit(1); }
        
    // Make sure we can construct the dang thing
    ring.populate(P);
    Ring::hue(ring.ring[0], hue);
    Ring::lum(ring.ring[0], lum);
    Ring::pos(ring.ring[0], pos);
    if (ring.nodes != P) { printf("Population\n"); exit(1); }
    // This should also sort the nodes on position
    ring.print();
    
    printf("%d\n", pos);
    for (uint8_t i=1; i < P; i++)
        if (Ring::pos(ring.ring[i]) < Ring::pos(ring.ring[i-1])) {
            printf("Sorting: %d < %d\n", ring.ring[i], ring.ring[i-1]);
            exit(1);
        }
    
    //Try "resizing". These will just segfault if things go wrong.
    uint32_t before = ring.ring[5];
    ring.populate(255);
    
    ring.pos(ring.ring[254], 200);
    uint32_t after = ring.ring[5];
    if (before != after) { printf("Resize stability\n"); exit(1); }
    
}