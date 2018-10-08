#include <sys/ioctl.h>  //terminal width
#include <unistd.h>		//usleep
#include <stdlib.h>		//srand, rand
#include <time.h>       //time 
#include "ring.hpp"
// #include "cxxopts.hpp"

using namespace std;

struct winsize w;

#define SPARKLES 20

int main() {
    srand(time(NULL));
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	PIXELS = w.ws_col-2;
	list sparkles;
	sparkles.populate(SPARKLES);
    R=rand(); G=rand(); B=rand();
	while (true) {
            
            sparkles.update();
        
        cout << "\r[";
        
            sparkles.print();
        
        cout << "]";
        
        fflush(stdout);
        usleep(20000);
        printf("\r");
	}
    
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
