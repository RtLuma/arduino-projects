#include <iostream>
#include <string>

extern uint16_t PIXELS;
extern uint8_t R, G, B;

#define RGB_ZERO_SYMBOL " "
#define RGB_GRADIENT_WIDTH 3

uint8_t rainbow(uint8_t hue) { if (hue > 170) return ~(hue + (hue << 1)); if (hue > 85) return hue + (hue << 1); return 0; }

struct rgbnode {
    uint16_t pos;
    uint8_t rgb[3]={0};
    int8_t  hue;
    rgbnode *next;
    rgbnode() {
        this->hue=0;
        this->pos=0;
        this->next=this;
    }
    rgbnode(int8_t hue, uint16_t pos, rgbnode *next) {
        this->hue=hue;
        this->pos=pos;
        this->next=next;
        rgb[0]=rainbow(hue+170);
        rgb[1]=rainbow(hue+85);
        rgb[2]=rainbow(hue);
    }
};

class TriRing {
    
private:
    rgbnode *head; rgbnode *tail; uint8_t nodes;
    
public:
    TriRing() {
        head = new rgbnode;
        tail = head;
        nodes = 1;
    }
        
    void populate (uint8_t desiredNodes) { while (nodes < desiredNodes) insert(rand()%PIXELS, rand()); }
    void terminate(uint8_t desiredNodes) { while (nodes > desiredNodes) remove(head->pos); }
    
    void interpolate(rgbnode* nA, rgbnode* nAB, rgbnode* nB) {
        uint16_t dist = nB->pos - nA->pos;
        if (!dist) return;
        
        uint8_t* Argb = nA->rgb;
        uint8_t* Brgb = nB->rgb;
        uint8_t* ABrgb = nAB->rgb;
        
        uint16_t R = Argb[0] << 8;
        uint16_t G = Argb[1] << 8;
        uint16_t B = Argb[2] << 8;
        
        uint16_t dFromA = nAB->pos - nA->pos;
        
        int16_t Rstep = int16_t((Brgb[0]<<8) - R)/dist;
        int16_t Gstep = int16_t((Brgb[1]<<8) - G)/dist;
        int16_t Bstep = int16_t((Brgb[2]<<8) - B)/dist;
        
        R += Rstep * dFromA;
        G += Gstep * dFromA;
        B += Bstep * dFromA;
        
        ABrgb[0] = R>>8;        
        ABrgb[1] = G>>8;        
        ABrgb[2] = B>>8;
    }    
    
    void blend(uint8_t rgb[3], uint8_t RGB[3]) {
        for (uint8_t i=0; i<3; i++) {
            int16_t delta = RGB[i]-rgb[i];
            if (!delta) continue;
            bool delta2 = (1<<16) & delta;
            delta >>= 4;
            if (!delta) delta = delta2 ? -1 : 1;
            rgb[i] += delta;
        }
    }
    
    void updateDiscrete(void) {
      rgbnode *n = head;
      do {
        int8_t newhue = n->hue + 1;
        
        if (n->hue<0 && newhue>-1) {
          uint16_t del_pos = n->pos; n = n->next; remove(del_pos);
          bool reinject; do { reinject = !insert(rand()%PIXELS, rand()); } while (reinject);
          continue;
        }        
        n->hue = newhue;
        n = n->next;
      } while(n->next != head);
    }
        
    void printDiscrete() {  //This should handle hue scaling u nig
		rgbnode *n = head;
		uint16_t p=0;
		for (; n->next != head;) {
			for (; p < n->pos; p++) printf(RGB_ZERO_SYMBOL);
			printNode(n);
            p++;
            n=n->next;
		}
		for (; p < PIXELS; p++) printf(RGB_ZERO_SYMBOL);
	}
    
    // void updateContinuous() {
    //   rgbnode *pre = tail, *cur = head;
    //   do {
    //     int8_t newhue = cur->hue + 1;
        
    //     if (!(abs(newhue) - abs(interpolate(pre, cur->next, cur->pos)))) {
    //       uint16_t del_pos = cur->pos; cur = cur->next; remove(del_pos);
    //       bool reinject; do { del_pos = rand()%PIXELS; reinject = !inject(del_pos); } while (reinject);
    //       continue;
    //     }        
    //     cur->hue = newhue;
    //     pre = cur; cur = cur->next;
    //   } while(cur->next != head);
    // }
    
    // void printContinuous() {
    //     rgbnode *n = head;
    //     head->pos += PIXELS;
    //     rgbnode origin(interpolate(tail, head, PIXELS), 0, head);
    //     head->pos -= PIXELS;
        
    //     uint16_t p=0;
        
    //     n=&origin;
    //     p=printGradient(n, head);
    //     n=head;
                
    //     for (; n->next != head;) {
    //         p += printGradient(n, n->next);
    //         n=n->next;
    //     }

    //     origin.pos=PIXELS;
    //     n=&origin;
    //     p=printGradient(tail, n);
	// }
    
    void printNode(rgbnode* n) {
        uint8_t disp = abs(n->hue);
        if (disp < 128) disp <<= 1;
        else disp = 255;
        uint8_t* rgb=n->rgb;
        printf("\033[38;2;%d;%d;%dm█\033[0;m", (uint16_t(rgb[0] * disp) + 1) >> 8, (uint16_t(rgb[1] * disp) + 1) >> 8, (uint16_t(rgb[2] * disp) + 1) >> 8);
    } //]]
    
    // uint8_t printGradient(rgbnode* A, rgbnode* B) {
    //     uint16_t hue = abs(A->hue) << 8; uint16_t dist = B->pos - A->pos;
    //     if (!dist) return 0;
    //     int16_t hue_step = int16_t(abs(B->hue<<8) - hue)/dist;
    //     uint16_t p = 1;
    //     // printNode(hue>>8);
    //     printf("\033[38;2;255;255;255m█\033[0;m");
    //     hue += hue_step;
    //     for (; p < dist; p++) { printNode(hue>>8); hue += hue_step; }
    //     return dist;
    // }
    
    bool insert(uint16_t pos, int8_t hue) { // bool 'alive' instead of hue arg
        nodes++; rgbnode *cur=head;
        if (pos < head->pos) { head = new rgbnode(hue,pos,head); tail->next=head;       return true;  }
        do { if (cur->next->pos == pos) {                                   nodes--; return false; }
             if (cur->next->pos >  pos) { cur->next = new rgbnode(hue, pos, cur->next); return true;  }
             cur = cur->next; } while (cur->next != head);
        cur->next = new rgbnode(hue, pos, cur->next); tail=cur->next;                   return true;
    }
    
    // bool inject(uint16_t pos) {
    //     if (pos > PIXELS || pos == head->pos) return false;
    //     rgbnode *cur = head;
    //     nodes++; 
    //     if (pos < head->pos) {
    //         head = new rgbnode(interpolate(cur, cur->next, pos),pos,head);
    //         tail->next=head;
    //         return true;
    //     }
    //     do {
    //         if (cur->next->pos > pos ) {
    //             if (cur->next->pos - pos < RGB_GRADIENT_WIDTH) { nodes--; return false; }
    //             if (pos - cur->pos       < RGB_GRADIENT_WIDTH) { nodes--; return false; }
    //             cur->next = new rgbnode(interpolate(cur, cur->next, pos),pos,cur->next);
    //             return true;
    //         }
    //         if (cur->next->pos == pos) { nodes--; return false; }
    //         cur = cur->next;
    //     } while(cur->next != head);
    //     cur->next = new rgbnode(interpolate(cur, cur->next, pos), pos, cur->next);
    //     tail=cur->next;
    //     return true;
    // }
    
    bool remove(uint16_t pos) {  //delete @ position
        if (pos > PIXELS || pos < head->pos) return false;
        rgbnode *cur = head;
        if (pos == head->pos) {
            nodes--;
            rgbnode* temp=head->next;
            delete head; head=temp;
            tail->next=head;
            return true;
        }
        do {
            if (cur->next->pos >  pos) return false;
            if (cur->next->pos == pos) {
                rgbnode *temp = cur->next->next; 
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
} cleds;

void rgbDiscrete(void) { cleds.updateDiscrete(); cleds.printDiscrete(); }
// void rgbContinuous(void) { leds.updateContinuous(); leds.printContinuous(); }
