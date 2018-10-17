#include <iostream>
#include <string>

extern uint16_t PIXELS;
extern uint8_t R, G, B;

#define RGB_ZERO_SYMBOL " "
#define RGB_GRADIENT_WIDTH 3

uint8_t rainbow(uint8_t hue) { if (hue > 170) return ~(hue + (hue << 1)); if (hue > 85) return hue + (hue << 1); return 0; }

struct rgbnode {
    uint16_t pos;
    uint8_t rgb[3];
    int8_t  hue;
    rgbnode *next;
    rgbnode() {
        this->hue=rand();
        this->pos=0;
        this->next=this;
    }
    rgbnode(int8_t hue, uint16_t pos, rgbnode *next) {
        this->hue=hue;
        this->pos=pos;
        this->next=next;
        rgb[0]=rgb[1]=rgb[2]=0;
    }
    rgbnode(int8_t hue, uint16_t pos, uint8_t rgb[3], rgbnode *next) {
        this->hue=hue;
        this->pos=pos;
        this->next=next;
        this->rgb[0]=rgb[0];
        this->rgb[1]=rgb[1];
        this->rgb[2]=rgb[2];
    }
    uint16_t getPos(void) { return pos & 32767; }
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
    
    void interpolate(uint16_t pos, rgbnode* nA, uint8_t ABrgb[3], rgbnode* nB) {
        uint16_t dist = nB->getPos() - nA->getPos();
        
        uint8_t* Argb = nA->rgb;
        uint8_t* Brgb = nB->rgb;
        
        uint16_t R = Argb[0] << 7;
        uint16_t G = Argb[1] << 7;
        uint16_t B = Argb[2] << 7;
        
        uint16_t dFromA = pos - nA->getPos();
        
        int16_t Rstep = ((((int16_t)Brgb[0])<<7) - R)/dist;
        int16_t Gstep = ((((int16_t)Brgb[1])<<7) - G)/dist;
        int16_t Bstep = ((((int16_t)Brgb[2])<<7) - B)/dist;
        
        R += Rstep * dFromA;
        G += Gstep * dFromA;
        B += Bstep * dFromA;
        
        ABrgb[0] = R>>7;        
        ABrgb[1] = G>>7;        
        ABrgb[2] = B>>7;
    }    
    
    bool comp(uint8_t rgb[3], uint8_t RGB[3]) {
        for (uint8_t i=0; i<3; i++) if (abs(rgb[i] - RGB[i]) > 1) return false;
        return true;
    }
    
    void blend(uint8_t rgb[3], uint8_t RGB[3]) {
        for (uint8_t i=0; i<3; i++) {
            int16_t delta = RGB[i]-rgb[i];
            if (!delta) continue;
            bool delta2 = delta < 0;
            delta >>= 5;
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
    
    void updateContinuous() {
      rgbnode *pre = tail, *cur = head;
      uint8_t newrgb[3];
      do {
        if (cur->pos & 32768) { // Fade out
            interpolate(cur->getPos(), pre, newrgb, cur->next);
            blend(cur->rgb, newrgb);
            if (comp(cur->rgb, newrgb)) {
                uint16_t del_pos = cur->getPos();
                cur = cur->next;
                remove(del_pos);
                bool reinject; do { reinject = !inject(rand()%PIXELS); } while (reinject);
                continue;
            }
        }                
        else {  // Converge from birthcolors to target hue
            newrgb[0]=rainbow(cur->hue+170);
            newrgb[1]=rainbow(cur->hue+85);
            newrgb[2]=rainbow(cur->hue);
            blend(cur->rgb, newrgb);
            if (comp(cur->rgb, newrgb)) cur->pos |= 32768; // Set decay flag
        }
        
        pre = cur; cur = cur->next;
      } while(cur!= head);
    }
    
    void scramble() {
        rgbnode *n = head;
        do {
            
        
        } while(n->next != head);
    }
    
    void printContinuous() {
        rgbnode *n = head;
        
        head->pos &= 32767;
        head->pos += PIXELS;
        
        rgbnode origin(0, 0, head);
        interpolate(PIXELS, tail, origin.rgb, head);

        head->pos -= PIXELS;
        head->pos |= 32768;
        
        uint16_t p=0;
        
        n=&origin;
        p=printGradient(n, head);
        n=head;
                
        for (; n->next != head;) {
            p += printGradient(n, n->next);
            n=n->next;
        }

        origin.pos=PIXELS;
        n=&origin;
        p=printGradient(tail, n);
	}
    
    void printNode(rgbnode* n) {
        uint8_t disp = abs(n->hue);
        if (disp < 128) disp <<= 1;
        else disp = 255;
        uint8_t* rgb=n->rgb;
        printf("\033[38;2;%d;%d;%dm█\033[0;m", (uint16_t(rgb[0] * disp) + 1) >> 8, (uint16_t(rgb[1] * disp) + 1) >> 8, (uint16_t(rgb[2] * disp) + 1) >> 8);
    } //]]
    
    uint8_t printGradient(rgbnode* nA, rgbnode* nB) {
        uint16_t dist = nB->getPos() - nA->getPos();
        if (!dist) return 0;
        
        uint8_t* Argb = nA->rgb;
        uint8_t* Brgb = nB->rgb;
        
        uint16_t R = Argb[0] << 7;
        uint16_t G = Argb[1] << 7;
        uint16_t B = Argb[2] << 7;
        
        int16_t Rstep = ((((int16_t)Brgb[0])<<7) - R)/dist;
        int16_t Gstep = ((((int16_t)Brgb[1])<<7) - G)/dist;
        int16_t Bstep = ((((int16_t)Brgb[2])<<7) - B)/dist;
        
        R += Rstep;
        G += Gstep;
        B += Bstep;

        uint16_t p = 0;
        // uint16_t p = 1;if (nA->pos & 32768) printf("\033[38;2;%d;%d;%dmX\033[0;m", R>>7, G>>7, B>>7); else printf("\033[38;2;%d;%d;%dm@\033[0;m", R>>7, G>>7, B>>7);
        
        for (; p < dist; p++) { 
            printf("\033[38;2;%d;%d;%dm█\033[0;m", R>>7, G>>7, B>>7);
            R += Rstep;
            G += Gstep;
            B += Bstep;
        }
        return dist;
    }
    
    bool insert(uint16_t pos, int8_t hue) {
        nodes++; rgbnode *cur=head;
        if (pos < head->getPos()) { head = new rgbnode(hue,pos,head); tail->next=head;       return true;  }
        do { if (cur->next->getPos() == pos) {                                   nodes--;    return false; }
             if (cur->next->getPos() >  pos) { cur->next = new rgbnode(hue, pos, cur->next); return true;  }
             cur = cur->next; } while (cur->next != head);
        cur->next = new rgbnode(hue, pos, cur->next); tail=cur->next;                   return true;
    }
    
    bool inject(uint16_t pos) {
        if (pos > PIXELS || pos == head->getPos()) return false;
        rgbnode *cur = head;
        nodes++; 
        if (pos < head->getPos()) {
            // head->pos += PIXELS:                 //Fix this shit nigga
            uint8_t rgb[3];
            interpolate(head->getPos(), cur, rgb, cur->next);
            head = new rgbnode(rand(), pos, rgb, head);
            // head->pos -= PIXELS:
            tail->next=head;
            return true;
        }
        do {
            if (cur->next->getPos() > pos ) {
                if (cur->next->getPos() - pos < RGB_GRADIENT_WIDTH) { nodes--; return false; }
                if (pos - cur->getPos()       < RGB_GRADIENT_WIDTH) { nodes--; return false; }
                uint8_t rgb[3];
                interpolate(cur->getPos(), cur, rgb, cur->next);
                pos = (cur->getPos() + cur->next->getPos())>>1;
                cur->next = new rgbnode(rand(), pos, rgb, cur->next);
                return true;
            }
            if (cur->next->getPos() == pos) { nodes--; return false; }
            cur = cur->next;
        } while(cur->next != head);
        uint8_t rgb[3];
        interpolate(cur->getPos(), cur, rgb, cur->next);
        cur->next = new rgbnode(rand(), pos, rgb, cur->next);
        tail=cur->next;
        return true;
    }
    
    bool remove(uint16_t pos) {  //delete @ position
        if (pos > PIXELS || pos < head->getPos()) return false;
        rgbnode *cur = head;
        if (pos == head->getPos()) {
            nodes--;
            rgbnode* temp=head->next;
            delete head; head=temp;
            tail->next=head;
            return true;
        }
        do {
            if (cur->next->getPos() >  pos) return false;
            if (cur->next->getPos() == pos) {
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
void rgbContinuous(void) { cleds.updateContinuous(); cleds.printContinuous(); }