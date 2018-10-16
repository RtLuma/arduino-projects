#include <iostream>
#include <string>
#include <unistd.h>		//usleep
#include <math.h>

uint8_t rgb[3]={0,0,0}, RGB[3]={255,128,0},_RGB[3]={0,128,255};
uint16_t RGB_[3] = {0};

// #define CONVERGE 10


bool comp(uint8_t rgb[3], uint8_t RGB[3]) {
    for (uint8_t i=0; i<3; i++) if (rgb[i] != RGB[i]) return false;
    return true;
}

void converge(uint8_t rgb[3], uint8_t RGB[3]) {
    for (uint8_t i=0; i<3; i++) {
            int16_t delta = RGB[i]-rgb[i];
            if (!delta) continue;
            bool delta2 = (1<<16) & delta;
            delta >>= 4;
            if (!delta) delta = delta2 ? -1 : 1;
            rgb[i] += delta;
    }
}


int main() {
    uint32_t frames=0;
    srand(time(NULL));
    for (uint32_t T=0; T<1000; T++) {
        for (uint8_t i=0; i<3; i++) { rgb[i]=rand(); RGB[i]=rand(); _RGB[i]=rand(); RGB_[i]=rand();}
        while((!comp(rgb,RGB)) && (!comp(RGB,_RGB))) {
            converge(rgb, RGB);
            converge(RGB, _RGB);
            for (uint8_t i=0; i<3; i++) {
                RGB_[i]+=int16_t(rand()%512) * (rand() &  1 ? 1 : -1);
                _RGB[i] = RGB_[i] >> 8;
            }
            printf("\033[38;2;%d;%d;%dm█\033[0;m", rgb[0], rgb[1], rgb[2]);
            printf("\033[38;2;%d;%d;%dm█\033[0;m", RGB[0], RGB[1], RGB[2]);
            printf("\t\033[38;2;%d;%d;%dm█\033[0;m", _RGB[0], _RGB[1], _RGB[2]);
            printf("\n");
        }
    }
}