#include "ring.hpp"

extern uint8_t HEIGHT;

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
    
     static void printNode(uint16_t r, uint16_t g, uint16_t b) {
        uint32_t _r = abs(r>>8); if (_r < 128) _r <<= 1; else _r = 255;
        uint32_t _g = abs(g>>8); if (_g < 128) _g <<= 1; else _g = 255;
        uint32_t _b = abs(b>>8); if (_b < 128) _b <<= 1; else _b = 255;
        printf("\033[48;2;%d;%d;%dm \033[0m", _r,_g,_b);
    }
    
    void display(void) {
        node    *Rp=R.head,
                *Gp=G.head,
                *Bp=B.head;
        
        uint16_t r,  g,  b,
                 rB, gB, bB;
                
        int16_t  Rs, Gs, Bs,
                 Rd, Gd, Bd;
        
        #define ORIGIN(C, Cp, Co, Cf)\
        Cp->pos += PIXELS;\
        node Co(Ring::interpolate(C.tail, C.head, PIXELS), 0, C.head);\
        node Cf(Co.lum,PIXELS-1,&Co);\
        Cp->pos -= PIXELS;\
        if (Cp->pos) Cp = &Co;\
        if (C.tail->pos != PIXELS-1) C.tail->next=&Cf;\

        ORIGIN(R, Rp, Ro, Rf)
        ORIGIN(G, Gp, Go, Gf)
        ORIGIN(B, Bp, Bo, Bf)

        #define DERIVE(CP, CD, CS, c, cb)\
        CD=CP->next->pos - CP->pos;\
        c=abs(CP->lum)<<8;\
        cb=abs(CP->next->lum)<<8;\
        CS=(int16_t)cb - (int16_t)c;\
        CS/=CD;\
        CP=CP->next;\

        DERIVE(Rp, Rd, Rs, r, rB)
        DERIVE(Gp, Gd, Gs, g, gB)
        DERIVE(Bp, Bd, Bs, b, bB)

        for (uint16_t p = 0; p < PIXELS; p++) { 

            bool Rb = !(--Rd);
            bool Gb = !(--Gd);
            bool Bb = !(--Bd);
            
            if (Rb) { DERIVE(Rp, Rd, Rs, r, rB) }
            if (Gb) { DERIVE(Gp, Gd, Gs, g, gB) }
            if (Bb) { DERIVE(Bp, Bd, Bs, b, bB) }
            
            printNode(r, g, b);
            
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
    for (uint8_t i = 0; i < HEIGHT; i++) {
        rgbing.display();
        printf("\n");
    }
    printf("\033[%dA", HEIGHT);
    
}
