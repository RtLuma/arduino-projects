#include <sys/ioctl.h>
#include <unistd.h>		//usleep
#include <stdlib.h>		//srand, rand
#include <time.h>       //time 
#include "cxxopts.hpp"
#include "linked_loop.hpp"


using namespace std;

#define TEST_FAIL_MSG "\n\n\n\n\n\nWeak!\n\n\n\n\n"
#define SCRAMBLE 20

struct winsize w;

// #define SPARKLES 50

int main() {
    srand(time(NULL));
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    PIXELS = w.ws_col-2;
    // uint16_t SCRAMBLE = PIXELS/3-1;
    
    for (uint32_t T=0; T<1000; T++) { // Run test a bunch of times to avoid serendipity
        printf("Test %d #############################\n\n", T);
        list sparkles;
        printf("%d\t", (sparkles.getHead())->pos);
        sparkles.proxPrint();
        
        rand();
    
        uint8_t i_s[SCRAMBLE];
        for (uint8_t i=0; i < SCRAMBLE; i++) i_s[i]=i; // Init array in order
        
        for (uint8_t i=0; i < SCRAMBLE; i++) {  //Randomly scramble all indices
            uint8_t r1 = rand()%SCRAMBLE;
            uint8_t r2 = rand()%SCRAMBLE;
            if (r1 > PIXELS || r2 > PIXELS) {i--; continue;}
            uint8_t t = i_s[r1]; 
            i_s[r1] = i_s[r2];
            i_s[r2] = t;
        }

        for (uint8_t i=0; i < SCRAMBLE; i++) {  // Push the random order, should end up sorted
            sparkles.insert(i_s[i], 0);
            printf("%d\t", i_s[i]);
            sparkles.proxPrint();
            if (!sparkles.checkSort()) { printf("sort"); printf(TEST_FAIL_MSG); exit(1); }
        }
        
        // LL should be full, make sure it's in exact sequential order.
        if (!sparkles.checkSequence()) { printf("seq"); printf(TEST_FAIL_MSG); exit(1); }
        
        // #################################################################################### Now to empty it out
        
        for (uint8_t i=0; i < SCRAMBLE; i++) {  //Randomly scramble all indices
            uint8_t r1 = rand()%SCRAMBLE;
            uint8_t r2 = rand()%SCRAMBLE;
            if (r1 > PIXELS || r2 > PIXELS) {i--; continue;}
            uint8_t t = i_s[r1]; 
            i_s[r1] = i_s[r2];
            i_s[r2] = t;
        }
        
        for (uint8_t i=1; i < SCRAMBLE; i++) {  // Cut the random order, should still preserve consistency
            sparkles.cut(i_s[i]);
            printf("-%d\t", i_s[i]);
            sparkles.proxPrint();
            if (!sparkles.checkSort()) { printf(TEST_FAIL_MSG); exit(1); }
        }
        
        printf("Sole survivor: ");
        sparkles.print();
        printf("\n");
        
            // sparkles.insert(rand()%PIXELS, rand());
    // sparkles.insert(rand()%PIXELS, rand());
    // sparkles.insert(rand()%PIXELS, rand());
    // cout << "\r["; sparkles.print(); cout << "]\n";
    // sparkles.insertAlive(rand()%PIXELS);
    // sparkles.insertAlive(rand()%PIXELS);
    // sparkles.insertAlive(rand()%PIXELS);
    // sparkles.insertAlive(rand()%PIXELS);
    // sparkles.insertAlive(rand()%PIXELS);
    // cout << "\r["; sparkles.print(); cout << "]\n";

    }
}