#include <iostream>
#include <string>

extern uint16_t PIXELS;
extern uint8_t R, G, B;

#define ZERO_SYMBOL " "
#define GRADIENT_WIDTH 3

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
        
    void populate (uint8_t desiredNodes) { while (nodes < desiredNodes) insert(rand()%PIXELS, rand()); }
    void terminate(uint8_t desiredNodes) { while (nodes > desiredNodes) remove(head->pos); }
    
    static int8_t interpolate(node* A, node* B, uint16_t pos) {
        uint16_t dist = B->pos - A->pos;
        if (!dist) return 0;
        uint16_t lum = abs(A->lum) << 8;
        uint16_t dFromA = pos - A->pos;
        int16_t lum_step = int16_t(abs(B->lum<<8) - lum)/dist;
        lum += lum_step * dFromA;
        lum >>= 8;
        return rand() & 1 ? -lum : lum;
    }    
    
    void updateDiscrete(void) {
      node *n = head;
      do {
        int8_t newlum = n->lum + 1;
        
        if (n->lum<0 && newlum>-1) {
          uint16_t del_pos = n->pos; n = n->next; remove(del_pos);
          bool reinject; do { reinject = !insert(rand()%PIXELS, 0); } while (reinject);
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
			for (; p < n->pos; p++) printf(ZERO_SYMBOL);
			printNode(n->lum);
            p++;
            n=n->next;
		}
		for (; p < PIXELS; p++) printf(ZERO_SYMBOL);
	}
    
    void updateContinuous() {
      node _tail(*tail); _tail.pos -= PIXELS;
      node _head(*head); _head.pos += PIXELS;
      node *pre = &_tail, *cur = head;
      
      tail->next=&_head;
      
      do {
        int8_t newlum = cur->lum + 1;
        
        if (!(abs(newlum) - abs(interpolate(pre, cur->next, cur->pos)))) {
          uint16_t del_pos = cur->pos; cur = cur->next; remove(del_pos);
          bool reinject; do { del_pos = rand()%PIXELS; reinject = !inject(del_pos); } while (reinject);
          continue;
        }        
        cur->lum = newlum;
        pre = cur; cur = cur->next;
      } while(cur != tail);
      
      if (tail->next == &_head) tail->next=head;
    }
    
    void printContinuous() {
        node *n = head;
        head->pos += PIXELS;
        node origin(interpolate(tail, head, PIXELS), 0, head);
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
    
    void printNode(int8_t lum) {
        uint8_t disp = abs(lum);
        if (disp < 128) disp <<= 1;
        else disp = 255;
        // printf("\033[38;2;%d;%d;%dm", uint16_t(R * disp) >> 8, uint16_t(G * disp) >> 8, uint16_t(B * disp) >> 8);
        // std::cout << val2block(disp);
        // printf("\033[0m");
        printf("\033[48;2;%d;%d;%dm \033[0m", uint16_t(R * disp) >> 8, uint16_t(G * disp) >> 8, uint16_t(B * disp) >> 8);
    } //]]
    
    uint8_t printGradient(node* A, node* B) {
        uint16_t lum = abs(A->lum) << 8; uint16_t dist = B->pos - A->pos;
        if (!dist) return 0;
        int16_t lum_step = int16_t(abs(B->lum<<8) - lum)/dist;
        uint16_t p = 1;
        printNode(127);
        for (; p < dist; p++) { printNode(lum>>8); lum += lum_step; }
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
                if (cur->next->pos - pos < GRADIENT_WIDTH) { nodes--; return false; }
                if (pos - cur->pos       < GRADIENT_WIDTH) { nodes--; return false; }
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
} mleds;

void monoDiscrete(void) { mleds.updateDiscrete(); mleds.printDiscrete(); }
void monoContinuous(void) { mleds.updateContinuous(); mleds.printContinuous();}
