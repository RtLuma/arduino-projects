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
    
    static uint8_t getLum(int16_t lum) {
        uint8_t disp = abs(lum);
        if (disp < 128) disp <<= 1;
        else disp = 255;
        return disp;
    }
    
    void display(void) {
        node *Rp=R.head, *Gp=G.head, *Bp=B.head;
        
        Rp->pos += PIXELS; node Ro(Ring::interpolate(R.tail, Rp, PIXELS), 0, Rp); Rp->pos -= PIXELS; if (Rp->pos) Rp = &Ro;
        Gp->pos += PIXELS; node Go(Ring::interpolate(G.tail, Gp, PIXELS), 0, Gp); Gp->pos -= PIXELS; if (Gp->pos) Gp = &Go;
        Bp->pos += PIXELS; node Bo(Ring::interpolate(B.tail, Bp, PIXELS), 0, Bp); Bp->pos -= PIXELS; if (Bp->pos) Bp = &Bo;
        
        uint16_t r, g, b,
                Rd=1, Gd=1, Bd=1;
        int16_t  Rs, Gs, Bs;

        for (uint16_t p=0; p < PIXELS; p++) {
            
            if (!(--Rd)) {
                Rp=Rp->next;
                r=abs(Rp->lum)<<7;
                Rd = Rp->next->pos - Rp->pos;
                Rs = ((abs(Rp->next->lum)<<7) - r) / Rd;
            }
            if (Gp->pos == p && Gp != G.tail) { Gp=Gp->next; g=abs(Gp->lum)<<7; Gs = ((abs(Gp->next->lum)<<7) - g) / (Gp->next->pos - Gp->pos); }
            if (Bp->pos == p && Bp != B.tail) { Bp=Bp->next; b=abs(Bp->lum)<<7; Bs = ((abs(Bp->next->lum)<<7) - b) / (Bp->next->pos - Bp->pos); }
            
            r += Rs;
            g += Gs;
            b += Bs;            
            
            //Print
            printf("\033[48;2;%d;%d;%dm \033[0m", getLum(r>>7), 0,0);
            // printf("\033[48;2;%d;%d;%dm \033[0m", getLum(r>>7), getLum(g>>7), getLum(b>>7));
        }
    }
} rgbing;

void rgbContinuous(void) { 
    rgbing.update();
    rgbing.display();
}
