#include <iostream>

using namespace std;

int main() {
    uint16_t A = 0;
    uint16_t B = 255;
    uint8_t dist = 10;
    
    double mag = A;
    double mag_step = B;
    mag_step -= mag;
    mag_step /= dist;
    
    printf("%f\n", mag_step);

    for (uint8_t p = 0; p <= dist; p++) {
        
        double delta = mag;
        delta *= delta;
        delta /= 255;
        
        printf("%2f ", delta);
        mag += mag_step;   
        
    }   
    
    printf("\n");
}
