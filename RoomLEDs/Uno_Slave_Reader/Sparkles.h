#define MAX_SPARKLES 180 //heap overflow n such

struct node {
    uint16_t pos;
    int8_t lum;
    node *next;
    node() {
        this->lum=0;
        this->pos=0;
        this->next=this;
    }
    
    node(int8_t lum, uint16_t pos, node *next) {
        this->lum=lum;
        this->pos=pos;
        this->next=next;
    }
};

struct Ring {
    
    node *head; node *tail; uint8_t nodes;
    
    Ring() {
        head = new node;
        tail = head;
        nodes = 1;
    }
    
    void populate (uint8_t desiredNodes) { while (nodes < desiredNodes) insert(random(PIXELS), random(255)); }
//    void populate (uint8_t desiredNodes) { while (nodes < desiredNodes) insert(random(PIXELS), random(255)); }
    void terminate(uint8_t desiredNodes) { while (nodes > desiredNodes) remove(head->pos); }
    
    static int8_t interpolate(node* A, node* B, uint16_t pos) {
        int16_t dist = (int16_t)B->pos - (int16_t)A->pos;
        if (!dist) return 0;
        uint16_t lum = abs(A->lum) << 8;
        uint16_t dFromA = pos - A->pos;
        int16_t lum_step = int16_t(abs(B->lum<<8) - lum)/dist;
        lum += lum_step * dFromA;
        lum >>= 8;
        return random(2) & 1 ? -lum : lum;
    }

    static uint8_t getLum(int16_t lum) {
        uint8_t disp = abs(lum);
        if (disp < 128) disp <<= 1;
        else disp = 255;
        return disp;
    }
    
    static void printNode(int8_t lum) {
        uint32_t disp = abs(lum);
        if (disp < 128) disp <<= 1;
        else disp = 255;
        disp = sq(disp);
        send2Pixels(
            (uint32_t(disp) * R) >> 16,
            (uint32_t(disp) * G) >> 16,
            (uint32_t(disp) * B) >> 16
        );
    }

    static void printNode(uint8_t lum) {
      uint16_t plz = lum;
        send2Pixels(
            (plz * R) >> 8,
            (plz * G) >> 8,
            (plz * B) >> 8
        );
    }
    
    
    void updateDiscrete(void) {
      node *n = head;
      do {
        int8_t newlum = n->lum + 1;
        
        if (n->lum<0 && newlum>-1) {
          uint16_t del_pos = n->pos;
          n = n->next;
          remove(del_pos);
          bool reinject;
          do { reinject = !insert(random(PIXELS), 0); } while (reinject);
          continue;
        }        
        n->lum = newlum;
        n = n->next;
      } while(n->next != head);
    }
        
    void printDiscrete() {
        node *n = head;
        uint16_t p=0;
        for (; n->next != head;) {
        for (; p < n->pos; p++) send2Pixels(0, 0, 0);
        printNode(n->lum);
                p++;
                n=n->next;
        }
        for (; p < PIXELS; p++) send2Pixels(0, 0, 0);
    }
    
    void updateContinuous() {
      node _tail(*tail); _tail.pos -= PIXELS;
//      node _head(*head); _head.pos += PIXELS;
      node *pre = &_tail, *cur = head;
      
//       tail->next=&_head;
      
      do {
        int8_t newlum = cur->lum + 1;
        
        if (!(abs(newlum) - abs(interpolate(pre, cur->next, cur->pos)))) {
          uint16_t del_pos = cur->pos; cur = cur->next; remove(del_pos);
          bool reinject; do { reinject = !inject(random(PIXELS)); } while (reinject);
          continue;
        }        
        cur->lum = newlum;
        pre = cur; cur = cur->next;
      } while(cur != tail);
      
//      if (tail->next == &_head) tail->next=head;
    }
    
    void printContinuous() {
        node *n = head;
        head->pos += PIXELS;
        node origin(interpolate(tail, head, PIXELS), 0, head);
        head->pos -= PIXELS;
        
        uint16_t p=0;
        
        if (head->pos) n=&origin;
//        p=printGradient(n, head);
//        n=head;
                
        for (; n != tail;) {
            p += printGradient(n, n->next);
            n=n->next;
        }

        origin.pos=PIXELS;
        n=&origin;
        p=printGradient(tail, n);
  }
    
    uint8_t printGradient(node* nA, node* nB) {
        int16_t dist = nB->pos - nA->pos;
//        if (!dist) return 0;
        uint16_t lum = abs(nA->lum) << 8;
        uint16_t Blum = abs(nB->lum) << 8;
        int16_t lum_step = (int16_t)Blum - (int16_t)lum;
        
        lum_step /= dist;

//        uint16_t p = 1;
//        send2Pixels(255,255,255);
//        lum += lum_step;
        uint16_t p = 0;
        for (; p < dist; p++) {
          printNode(int8_t(lum>>8));
          lum += lum_step;
         }
        return dist;
    }
    
    bool insert(uint16_t pos, int8_t lum) { // bool 'alive' instead of lum arg
        nodes++; node *cur=head;
        if (pos < head->pos) { head = new node(lum,pos,head); tail->next=head;       return true;  }
        do { if (cur->next->pos == pos) {                                   nodes--; return false; }
             if (cur->next->pos >  pos) { cur->next = new node(lum, (cur->pos + cur->next->pos)>>1, cur->next); return true;  }
             cur = cur->next; } while (cur->next != head);
        cur->next = new node(lum, pos, cur->next); tail=cur->next;                   return true;
    }
    
    bool inject(uint16_t pos) {
        if (pos > PIXELS || pos == head->pos) return false;
        node *cur = head;
        nodes++; 
        if (pos < head->pos) {
            head->pos += PIXELS;
            head = new node(interpolate(tail, head, pos+PIXELS),pos,head);
            head->next->pos -= PIXELS;
            tail->next=head;
            return true;
        }
        do {
            if (cur->next->pos > pos ) {
                pos = (cur->pos + cur->next->pos)>>1;
                cur->next = new node(interpolate(cur, cur->next, pos),pos,cur->next);
                return true;
            }
            if (cur->next->pos == pos) { nodes--; return false; }
            cur = cur->next;
        } while(cur->next != head);
        cur->next = new node(interpolate(cur, cur->next, pos), pos, cur->next);
        tail=cur->next;
        return true;
    }
    
    bool remove(uint16_t pos) {  //delete @ position
        if (pos > PIXELS || pos < head->pos) return false;
        node *cur = head;
        if (pos == head->pos) {
            nodes--;
            node* temp=head->next;
            delete head; head=temp;
            tail->next=head;
            return true;
        }
        do {
            if (cur->next->pos >  pos) return false;
            if (cur->next->pos == pos) {
                node *temp = cur->next->next; 
                delete cur->next;
                cur->next = temp;
                nodes--;
                if (cur->next == head) tail = cur;
                return true;
            }
            cur = cur->next;
        } while (cur->next != head);
        return false;
    }
};

struct RGBing {
    Ring R; Ring G; Ring B;
    
    void populate(uint8_t n) {
        n /= 3;
        R.populate(n);
        G.populate(n);
        B.populate(n);
    }

    void terminate(uint8_t n) {
      n /= 3;
      R.terminate(n);
      G.terminate(n);
      B.terminate(n);
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
    
    static void printNode(uint16_t r, uint16_t g, uint16_t b) {
        uint32_t _r = abs(r>>8); if (_r < 128) _r <<= 1; else _r = 255; _r = sq(_r);
        uint32_t _g = abs(g>>8); if (_g < 128) _g <<= 1; else _g = 255; _g = sq(_g);
        uint32_t _b = abs(b>>8); if (_b < 128) _b <<= 1; else _b = 255; _b = sq(_b);
//        send2Pixels(_r,_g,_b);
        send2Pixels(_r>>8,_g>>8,_b>>8);
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
            
//            send2Pixels(
//                getLum(r>>8),
//                getLum(g>>8),
//                getLum(b>>8)
//            );
            printNode(r, g, b);
//            printNode(Rb ? 255:r, Gb ? 255:g, Bb ? 255:b);
            
            r += Rs;
            g += Gs;
            b += Bs;
        }
        
        R.tail->next=R.head;
        G.tail->next=G.head;
        B.tail->next=B.head;
        
    }
}; 


Ring mleds;
RGBing rgbing;

void monoDiscrete  (void) { mleds.updateDiscrete();   mleds.printDiscrete();   delayMicroseconds(MAX_SPARKLES - P); }
void monoContinuous(void) { mleds.updateContinuous(); mleds.printContinuous();  }
void rgbContinuous (void) { rgbing.update();          rgbing.display();        delayMicroseconds(MAX_SPARKLES - P); }
