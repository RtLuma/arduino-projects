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
    P = 12;
    R = rand(); G = rand(); B = rand();
    Ring ring;
    
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
    ring.init(P);
    // if (ring.nodes != P) { printf("Population\n"); exit(1); }
    // // This should also sort the nodes on position
    
    // for (uint8_t i=1; i < P; i++) {
    //     uint16_t large = Ring::pos(ring.ring[i]);
    //     uint16_t small = Ring::pos(ring.ring[i-1]);
    //     if (large < small) {
    //         printf("Sorting: %d < %d\n", large, small);
    //         exit(1);
    //     }
    // }
    
    // Test resize stability (should not modify nodes in the list)
    string b4 = ring.to_string();
    
    ring.populate(P);
    ring.populate(P);
    ring.populate(P);
    ring.populate(P);
    ring.populate(P);
    
    string af = ring.to_string();
    if (b4 != af) {
        printf("Stability\n");
        cout << b4 << endl << af << endl;
        exit(1);
    }
    
    ring.print();
    
    printf("All tests passed!\n");
    exit(0);
}