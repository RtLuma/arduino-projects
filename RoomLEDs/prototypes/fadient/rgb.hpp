#include "ring.hpp"


struct RGBing {
    Ring R; Ring G; Ring B;
    
    void populate(uint8_t n) {
        n /= 3;
        R.populate(n);
        G.populate(n);
        B.populate(n);
    }
    
    void update(void) {
        R.updateContinuous();
        G.updateContinuous();
        B.updateContinuous();
    }
    
    void display(void) {
        node *Rp=R.head, *Gp=G.head, *Bp=B.head;
        
        //Wrap tail around to head if needed by creating artificial midpoint for them to gradient to.        
        Rp->pos += PIXELS; node Ro(Ring::interpolate(R.tail, Rp, PIXELS), 0, Rp); Rp->pos -= PIXELS; if (Rp->pos) Rp = &Ro;
        Gp->pos += PIXELS; node Go(Ring::interpolate(G.tail, Gp, PIXELS), 0, Gp); Gp->pos -= PIXELS; if (Gp->pos) Gp = &Go;
        Bp->pos += PIXELS; node Bo(Ring::interpolate(B.tail, Bp, PIXELS), 0, Bp); Bp->pos -= PIXELS; if (Bp->pos) Bp = &Bo;
        
        uint16_t r; uint16_t g; uint16_t b;

        // r = abs(Rp->lum) << 7;
        // g = abs(Gp->lum) << 7;
        // b = abs(Bp->lum) << 7;
        
        int16_t dR; int16_t dG; int16_t dB;
        
        // int16_t dR = ((Rp->next->lum << 7) - r) / (Rp->next->pos - Rp->pos);
        // int16_t dG = ((Gp->next->lum << 7) - g) / (Gp->next->pos - Gp->pos);
        // int16_t dB = ((Bp->next->lum << 7) - b) / (Bp->next->pos - Bp->pos);
        
        // dR = ((abs(Rp->next->lum) << 7) - r) / (Rp->next->pos - Rp->pos);
        // dG = ((abs(Gp->next->lum) << 7) - r) / (Gp->next->pos - Gp->pos);
        // dB = ((abs(Bp->next->lum) << 7) - r) / (Bp->next->pos - Bp->pos);
        
        for (uint16_t p=0; p < PIXELS; p++) {
            
            // if (Rp->pos == p && Rp != R.tail) { Rp=Rp->next; dR = ((abs(Rp->next->lum) << 7) - r) / (Rp->next->pos - Rp->pos); }
            // if (Gp->pos == p && Gp != G.tail) { Gp=Gp->next; dG = ((abs(Gp->next->lum) << 7) - g) / (Gp->next->pos - Gp->pos); }
            // if (Bp->pos == p && Bp != B.tail) { Bp=Bp->next; dB = ((abs(Bp->next->lum) << 7) - b) / (Bp->next->pos - Bp->pos); }

            r += dB;
            g += dG;
            b += dB;            
            
            uint8_t _r = r>>7;
            uint8_t _g = g>>7;
            uint8_t _b = b>>7;
            
            if (Rp->pos == p && Rp != R.tail) { _r=255;Rp=Rp->next; r=abs(Rp->lum)<<7; dR = ((abs(Rp->next->lum) << 7) - r) / (Rp->next->pos - Rp->pos); }
            if (Gp->pos == p && Gp != G.tail) { _g=255;Gp=Gp->next; g=abs(Gp->lum)<<7; dG = ((abs(Gp->next->lum) << 7) - g) / (Gp->next->pos - Gp->pos); }
            if (Bp->pos == p && Bp != B.tail) { _b=255;Bp=Bp->next; b=abs(Bp->lum)<<7; dB = ((abs(Bp->next->lum) << 7) - b) / (Bp->next->pos - Bp->pos); }
            
            //Print
            printf("\033[48;2;%d;%d;%dm \033[0m", _r, _g, _b);
            // printf("\033[48;2;%d;%d;%dm \033[0m", r >> 7, g >> 7, b >> 7);
        }
    }
} rgbing;

void rgbContinuous(void) { 
    rgbing.update();
    rgbing.display();
}
