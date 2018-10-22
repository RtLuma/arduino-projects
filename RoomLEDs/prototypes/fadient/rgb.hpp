#include "ring.hpp"


struct RGBing {
    Ring R; Ring G; Ring B;
    
    void populate(uint8_t n) {
        n /= 3;
        R.populate(n);
        G.populate(n);
        B.populate(n);
    }
    
    void display() {
        node *Rp=R.head, *Gp=G.head, *Bp=B.head;
        
        //3 origins here cuz dicks
        
        for (uint16_t p=0; p < PIXELS; p++) {
            
            uint16_t r = Rp->lum;
            uint16_t g = Gp->lum;
            uint16_t b = Bp->lum;
            
            int16_t dR = ((Rp->next->lum << 7) - r) / (Rp->next->pos - Rp->pos);
            int16_t dG = ((Gp->next->lum << 7) - g) / (Gp->next->pos - Gp->pos);
            int16_t dB = ((Bp->next->lum << 7) - b) / (Bp->next->pos - Bp->pos);
            
            r += dR
            g += dG;
            b += dB;

        }
    }
};