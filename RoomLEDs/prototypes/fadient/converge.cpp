#include <iostream>
#include <string>
#include <unistd.h>		//usleep
#include <math.h>

uint8_t rgb[3]={0,0,0}, RGB[3]={255,128,0}, _RGB[3]={0,128,255};

#define CONVERGE 10

bool comp(uint8_t rgb[3], uint8_t RGB[3]) {
    for (uint8_t i=0; i<3; i++) if (rgb[i] != RGB[i]) return false;
    return true;
}

uint16_t mag(uint8_t rgb[3]) {
    uint16_t a1=pow(rgb[0],2), 
             a2=pow(rgb[1],2),
             a3=pow(rgb[2],2);
    return sqrt(a1+a2+a3);
}

uint16_t dist(uint8_t rgb[3], uint8_t RGB[3]) {
    int16_t a1=pow(RGB[0] - rgb[0],2), 
            a2=pow(RGB[1] - rgb[1],2),
            a3=pow(RGB[2] - rgb[2],2);
    return sqrt(a1+a2+a3);
}


// void converge(uint8_t rgb[3], uint8_t RGB[3]) {
//     uint16_t d = abs(mag(RGB)-mag(rgb));
//     // printf("%d ", d);
//     for (uint8_t i=0; i<3; i++) {
//             int16_t delta = RGB[i]-rgb[i];
//             if (!delta) continue;
//             int16_t delta2 = delta;
//             delta /= d;
//             if (!delta) delta = delta2 > 0 ? 1 : -1;
//             rgb[i] += delta;
//     }
// }

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
    // for (uint32_t T=0; T<1000; T++) {
        for (uint8_t i=0; i<3; i++) { rgb[i]=rand(); RGB[i]=rand(); _RGB[i]=rand();}
        while(!comp(rgb,RGB)) {
            converge(rgb, RGB);
            converge(RGB, _RGB);
            printf("\033[38;2;%d;%d;%dm█\033[0;m", rgb[0], rgb[1], rgb[2]);
            printf("\033[38;2;%d;%d;%dm█\033[0;m", RGB[0], RGB[1], RGB[2]);
            printf("\n");
        }
    // }
}