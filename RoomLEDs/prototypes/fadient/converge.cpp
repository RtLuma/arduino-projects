#include <iostream>
#include <string>
#include <unistd.h>		//usleep

uint8_t rgb[3]={0}, RGB[3]={128,128,0};

#define CONVERGE 2

int main() {
    uint32_t frames=0;
    while(true) {
        frames++;
        printf("%d: ", frames);
        for (uint8_t i=0; i<3; i++) {
            printf("%d ", rgb[i]);
            uint8_t delta = RGB[i]-rgb[i];
            delta /= CONVERGE;
            rgb[i] += delta + 1;
        }
        printf("\n");
        usleep(1000000);
    }
}