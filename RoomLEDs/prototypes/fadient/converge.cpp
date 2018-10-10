#include <iostream>
#include <string>
#include <unistd.h>		//usleep

uint8_t rgb[3]={255,255,255}, RGB[3]={128,128,0};

#define CONVERGE 2

bool comp(uint8_t rgb[3], uint8_t RGB[3]) {
    for (uint8_t i=0; i<3; i++) if (rgb[i] != RGB[i]) return false;
    return true;
}

int main() {
    uint32_t frames=0;
    while(!comp(rgb,RGB)) {
        frames++;
        printf("%d: ", frames);
        for (uint8_t i=0; i<3; i++) {
            printf("%d ", rgb[i]);
            int8_t delta = RGB[i]-rgb[i];
            int8_t delta2 = delta;
            if (delta) delta /= CONVERGE;
            if (!delta) delta = delta2 > 0 ? 1 : -1;
            rgb[i] += delta;
        }
        printf("\n");
        // usleep(100000);
    }
}