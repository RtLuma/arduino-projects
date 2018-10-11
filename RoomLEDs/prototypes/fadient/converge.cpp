#include <iostream>
#include <string>
#include <unistd.h>		//usleep

uint8_t rgb[3]={0,0,0}, RGB[3]={255,128,0};

#define CONVERGE 20

bool comp(uint8_t rgb[3], uint8_t RGB[3]) {
    for (uint8_t i=0; i<3; i++) if (rgb[i] != RGB[i]) return false;
    return true;
}

void converge(uint8_t rgb[3], uint8_t RGB[3]) {
    for (uint8_t i=0; i<3; i++) {
            int16_t delta = RGB[i]-rgb[i];
            if (!delta) continue;
            int16_t delta2 = delta;
            delta /= CONVERGE;
            if (!delta) delta = delta2 > 0 ? 1 : -1;
            rgb[i] += delta;
    }
}

int main() {
    uint32_t frames=0;
    srand(time(NULL));
    for (uint8_t i=0; i<3; i++) {
        rgb[i]=rand();
        RGB[i]=rand();
    }
    while(!comp(rgb,RGB)) {
        frames++;
        // printf("%d: ", frames);
        converge(rgb, RGB);
        printf("\033[38;2;%d;%d;%dm█\033[0;m", rgb[0], rgb[1], rgb[2]);        
        // usleep(100000);
    }
        printf("\n");
}