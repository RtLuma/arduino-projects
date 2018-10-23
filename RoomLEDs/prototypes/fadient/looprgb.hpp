#include "ring.hpp"


struct RGBing {
    Ring R[3];
    
    void populate(uint8_t n) {
        n /= 3;
        for (uint8_t c=0; c<3; c++) R[c].populate(n);
    }
    
    void update(void) {
        for (uint8_t c=0; c<3; c++) R[c].updateContinuous();
    }
    
    void display(void) {
        node*    p[3];
        node     o[3];
        uint16_t v[3];
        int16_t  d[3];

        //Wrap tail around to head if needed by creating artificial midpoint for them to gradient to.        
        // for (uint8_t c=0; c<3; c++) {
        //     p[c]->pos += PIXELS;
        //     o[c]=node(Ring::interpolate(R[c].tail, p[c], PIXELS), 0, p[c]);
        //     p[c]->pos -= PIXELS;
        //     if (p[c]->pos) p[c] = &o[c];
        //     v[c] = abs(p[c]->lum) << 7;
        // }        
        
        for (uint16_t i=0; i < PIXELS; i++) {
            
            // uint8_t _r, _g, _b;
            
            for (uint8_t c=0; c<3; c++) {
            
                if (p[c]->pos == i && p[c] != R[c].tail) { 
                    p[c] = p[c]->next;
                    // uint8_t _d = 
                    d[c] = ((abs(p[c]->next->lum) << 7) - v[c]) / (p[c]->next->pos - p[c]->pos);
                }
                
                v[c] += d[c];
            }
            //Print
            printf("\033[48;2;%d;%d;%dm \033[0m", v[0] >> 7, v[1] >> 7, v[2] >> 7);
        }
    }
} rgbing;

void rgbContinuous(void) { 
    rgbing.update();
    rgbing.display();
}
