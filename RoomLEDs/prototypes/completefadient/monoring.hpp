#include <iostream>
#include <string>

extern uint16_t PIXELS;
extern uint8_t R, G, B;

#define ZERO_SYMBOL " "
#define GRADIENT_WIDTH 3

struct monode {
    uint16_t pos;
    int8_t sat;
    monode *next;
    monode() {
        this->sat=0;
        this->pos=0;
        this->next=this;
    }
    
    monode(int8_t sat, uint16_t pos, monode *next) {
        this->sat=sat;
        this->pos=pos;
        this->next=next;
    }
};

class MonoRing {
// using voidF = void(*)(void) ;
    
    
private:
    monode *head; monode *tail; uint8_t nodes;
    
public:
    // void (MonoRing::update)(void);
    // void (MonoRing::print)(void);
    
    MonoRing() {
        head = new monode;
        tail = head;
        nodes = 1;
    }
        
    void populate (uint8_t desiredNodes) { while (nodes < desiredNodes) insert(rand()%PIXELS, rand()); }
    void terminate(uint8_t desiredNodes) { while (nodes > desiredNodes) remove(head->pos); }
    
    int8_t interpolate(monode* A, monode* B, uint16_t pos) {
        uint16_t dist = B->pos - A->pos;
        if (!dist) return 0;
        uint16_t sat = abs(A->sat) << 8;
        uint16_t dFromA = pos - A->pos;
        int16_t sat_step = int16_t(abs(B->sat<<8) - sat)/dist;
        sat += sat_step * dFromA;
        sat >>= 8;
        return rand() & 1 ? -sat : sat;
    }    
    
    void updateDiscrete(void) {
      monode *n = head;
      do {
        int8_t newsat = n->sat + 1;
        
        if (n->sat<0 && newsat>-1) {
          uint16_t del_pos = n->pos; n = n->next; remove(del_pos);
          bool reinject; do { reinject = !insert(rand()%PIXELS, 0); } while (reinject);
          continue;
        }        
        n->sat = newsat;
        n = n->next;
      } while(n->next != head);
    }
        
    void printDiscrete() {
		monode *n = head;
		uint16_t p=0;
		for (; n->next != head;) {
			for (; p < n->pos; p++) printf(ZERO_SYMBOL);
			printNode(n->sat);
            p++;
            n=n->next;
		}
		for (; p < PIXELS; p++) printf(ZERO_SYMBOL);
	}
    
    void updateContinuous() {
      monode *pre = tail, *cur = head;
      do {
        int8_t newsat = cur->sat + 1;
        
        if (!(abs(newsat) - abs(interpolate(pre, cur->next, cur->pos)))) {
          uint16_t del_pos = cur->pos; cur = cur->next; remove(del_pos);
          bool reinject; do { del_pos = rand()%PIXELS; reinject = !inject(del_pos); } while (reinject);
          continue;
        }        
        cur->sat = newsat;
        pre = cur; cur = cur->next;
      } while(cur->next != head);
    }
    
    void printContinuous() {
        monode *n = head;
        head->pos += PIXELS;
        monode origin(interpolate(tail, head, PIXELS), 0, head);
        head->pos -= PIXELS;
        
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
    
    void printNode(int8_t sat) {
        uint8_t disp = abs(sat);
        if (disp < 128) disp <<= 1;
        else disp = 255;
        printf("\033[38;2;%d;%d;%dm█\033[0;m", (uint16_t(R * disp) + 1) >> 8, (uint16_t(G * disp) + 1) >> 8, (uint16_t(B * disp) + 1) >> 8);
    } //]]
    
    uint8_t printGradient(monode* A, monode* B) {
        uint16_t sat = abs(A->sat) << 8; uint16_t dist = B->pos - A->pos;
        if (!dist) return 0;
        int16_t sat_step = int16_t(abs(B->sat<<8) - sat)/dist;
        uint16_t p = 1;
        // printNode(sat>>8);
        printf("\033[38;2;255;255;255m█\033[0;m");
        sat += sat_step;
        for (; p < dist; p++) { printNode(sat>>8); sat += sat_step; }
        return dist;
    }
    
    bool insert(uint16_t pos, int8_t sat) { // bool 'alive' instead of sat arg
        nodes++; monode *cur=head;
        if (pos < head->pos) { head = new monode(sat,pos,head); tail->next=head;       return true;  }
        do { if (cur->next->pos == pos) {                                   nodes--; return false; }
             if (cur->next->pos >  pos) { cur->next = new monode(sat, pos, cur->next); return true;  }
             cur = cur->next; } while (cur->next != head);
        cur->next = new monode(sat, pos, cur->next); tail=cur->next;                   return true;
    }
    
    bool inject(uint16_t pos) {
        if (pos > PIXELS || pos == head->pos) return false;
        monode *cur = head;
        nodes++; 
        if (pos < head->pos) {
            head = new monode(interpolate(cur, cur->next, pos),pos,head);
            tail->next=head;
            return true;
        }
        do {
            if (cur->next->pos > pos ) {
                if (cur->next->pos - pos < GRADIENT_WIDTH) { nodes--; return false; }
                if (pos - cur->pos       < GRADIENT_WIDTH) { nodes--; return false; }
                cur->next = new monode(interpolate(cur, cur->next, pos),pos,cur->next);
                return true;
            }
            if (cur->next->pos == pos) { nodes--; return false; }
            cur = cur->next;
        } while(cur->next != head);
        cur->next = new monode(interpolate(cur, cur->next, pos), pos, cur->next);
        tail=cur->next;
        return true;
    }
    
    bool remove(uint16_t pos) {  //delete @ position
        if (pos > PIXELS || pos < head->pos) return false;
        monode *cur = head;
        if (pos == head->pos) {
            nodes--;
            monode* temp=head->next;
            delete head; head=temp;
            tail->next=head;
            return true;
        }
        do {
            if (cur->next->pos >  pos) return false;
            if (cur->next->pos == pos) {
                monode *temp = cur->next->next; 
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
} mleds;

void monoDiscrete(void) { mleds.updateDiscrete(); mleds.printDiscrete(); }
void monoContinuous(void) { mleds.updateContinuous(); mleds.printContinuous(); }
