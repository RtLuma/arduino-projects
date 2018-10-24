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
        
        Rp->pos += PIXELS; node Ro(Ring::interpolate(R.tail, R.head, PIXELS), 0, R.head); node Rf(Ro.lum,PIXELS-1,&Ro); Rp->pos -= PIXELS;
        Gp->pos += PIXELS; node Go(Ring::interpolate(G.tail, G.head, PIXELS), 0, G.head); node Gf(Go.lum,PIXELS-1,&Go); Gp->pos -= PIXELS;
        Bp->pos += PIXELS; node Bo(Ring::interpolate(B.tail, B.head, PIXELS), 0, B.head); node Bf(Bo.lum,PIXELS-1,&Bo); Bp->pos -= PIXELS;
        
        if (Rp->pos) Rp = &Ro; 
        if (Gp->pos) Gp = &Go; 
        if (Bp->pos) Bp = &Bo;
        
        if (R.tail->pos != PIXELS-1) R.tail->next=&Rf;
        if (G.tail->pos != PIXELS-1) G.tail->next=&Gf;
        if (B.tail->pos != PIXELS-1) B.tail->next=&Bf;
        
        uint16_t r, g, b, Rd, Gd, Bd; int16_t Rs, Gs, Bs;
        
        r=abs(Rp->lum)<<8;
        g=abs(Gp->lum)<<8;
        b=abs(Bp->lum)<<8;
        
        Rd=abs(Rp->next->pos - Rp->pos);
        Gd=abs(Gp->next->pos - Gp->pos);
        Bd=abs(Bp->next->pos - Bp->pos);
        
        Rs=((abs(Rp->next->lum)<<8) - r)/Rd;
        Gs=((abs(Gp->next->lum)<<8) - g)/Gd;
        Bs=((abs(Bp->next->lum)<<8) - b)/Bd;

        for (uint16_t p = 0; p < PIXELS; p++) { 
            if (!(--Rd)) { Rp=Rp->next;r=abs(Rp->lum)<<8;Rd=Rp->next->pos - Rp->pos;Rs=((abs(Rp->next->lum)<<8)-r)/Rd;}
            if (!(--Gd)) { Gp=Gp->next;g=abs(Gp->lum)<<8;Gd=Gp->next->pos - Gp->pos;Gs=((abs(Gp->next->lum)<<8)-g)/Gd;}
            if (!(--Bd)) { Bp=Bp->next;b=abs(Bp->lum)<<8;Bd=Bp->next->pos - Bp->pos;Bs=((abs(Bp->next->lum)<<8)-b)/Bd;}
            printf("\033[48;2;%d;%d;%dm \033[0m", getLum(r>>8), getLum(g>>8), getLum(b>>8));
            r += Rs;
            g += Gs;
            b += Bs;
        }
        
        R.tail->next=R.head;
        G.tail->next=G.head;
        B.tail->next=B.head;
        
    }
} rgbing;

void rgbContinuous(void) { 
    rgbing.update();
    rgbing.display();
}
