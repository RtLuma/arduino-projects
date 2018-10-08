#include <iostream>
#include <string>

#define ZERO_SYMBOL " "

uint16_t PIXELS;
uint8_t R, G, B;

using voidF = void(*)(int8_t) ;


std::string val2block(uint8_t val) { return "\u2588"; } 
// std::string val2block(uint8_t val) {if (val > 223) return "\u2588";if (val > 191) return "\u2587";if (val > 159) return "\u2586";if (val > 127) return "\u2585";if (val > 95)  return "\u2584";if (val > 63)  return "\u2583";if (val > 31)  return "\u2582";return ZERO_SYMBOL;} 

void printSparkle(int8_t mag) {
	uint8_t disp = abs(mag); if (disp < 128) disp <<= 1; else disp = 255;
	printf("\033[38;2;%d;%d;%dm", (uint16_t(R * disp) + 1) >> 8, (uint16_t(G * disp) + 1) >> 8, (uint16_t(B * disp) + 1) >> 8);
	std::cout << val2block(disp);
	printf("\033[0;m");
	
} //]]

void printSpawn(int8_t mag) {
	uint8_t disp = abs(mag); if (disp < 128) disp <<= 1; else disp = 255;
    printf("\033[38;2;255;255;255m");
	std::cout << val2block(disp);
	printf("\033[0;m");
	
} //]]

struct node {
    int8_t mag; uint16_t pos; node *next;
    node(int8_t mag, uint16_t pos, node *next) { this->mag=mag; this->pos=pos; this->next=next; }
    node() { this->mag=0; this->pos=rand()%PIXELS; this->next=this; }
    voidF nodePrint;
};

int8_t interMag(node* A, node* B, uint16_t pos) {
    uint16_t mag = abs(A->mag) << 8;
    uint16_t dist = B->pos - A->pos;
    if (!dist) return 0;
    uint16_t dFromA = pos - A->pos;
    int16_t mag_step = int16_t(abs(B->mag<<8) - mag)/dist;
    mag += mag_step * dFromA;
    mag >>= 8;
    return rand() & 1 ? -mag : mag;
}    
    

class list {
private:
    node *head; node *tail; uint8_t nodes;
    voidF nodePrint;
    
public:
    list() { head = new node; tail = head; nodes = 0;
        nodePrint = printSparkle;
        // nodePrint = printSpawn;
    }
    auto getHead()  { return head;  }
    auto getTail()  { return tail;  }
    auto getNodes() { return nodes; }
    void populate (uint8_t sparkles) { while (nodes < sparkles) insert(rand()%PIXELS, rand()); }
    void terminate(uint8_t sparkles) { while (nodes > sparkles) cut(head->pos); }
    
    void print() {
		node *temp = head; uint16_t p=0;
        if (head->pos > 0) {
            head->pos += PIXELS;
            node first(interMag(tail, head, PIXELS), 0, head);
            temp=&first;
            head->pos -= PIXELS;
            p=printGrad(temp, head);
            temp=head;
        }        
		for (; temp->next != head;) {
			for (; p < temp->pos; p++) printf(ZERO_SYMBOL);
			p += printGrad(temp, temp->next);
            temp=temp->next;
		}
		for (; p < PIXELS; p++) printf(ZERO_SYMBOL);
	}
    
    uint8_t printGrad(node* A, node* B) {
        uint16_t mag = abs(A->mag) << 8; uint16_t dist = B->pos - A->pos;
        if (!dist) return 0;
        int16_t mag_step = int16_t(abs(B->mag<<8) - mag)/dist;
        uint16_t p = 1;
        //   printSparkle(mag>>8);
        //   printSpawn(mag>>8);
        nodePrint(mag>>8);
        mag += mag_step;
        for (; p < dist; p++) { printSparkle(mag>>8); mag += mag_step; }
        return dist;
    }
    
    void printProx() { node *cur = head; do { printf("[%d,%d] ", cur->pos, cur->mag); cur=cur->next; } while (cur != head); printf("\n"); }
    

    bool insert(uint16_t pos, int8_t mag) { // bool 'alive' instead of mag arg
        nodes++; node *cur=head;
        if (pos < head->pos) { head = new node(mag,pos,head); tail->next=head;       return true;  }
        do { if (cur->next->pos == pos) {                                   nodes--; return false; }
             if (cur->next->pos >  pos) { cur->next = new node(mag, pos, cur->next); return true;  }
             cur = cur->next; } while (cur->next != head);
        cur->next = new node(mag, pos, cur->next); tail=cur->next;                   return true;
    }
    
    bool insertAlive(uint16_t pos) {
        if (pos > PIXELS || pos == head->pos) return false;
        nodes++; node *cur = head;
        if (pos < head->pos) {
            
            head = new node(interMag(cur, cur->next, pos),pos,head);
            
            tail->next=head;
            return true;
        }
        #define WIDTH 3
        do {
            if (cur->next->pos > pos ) {
                if (cur->next->pos - pos < WIDTH) { nodes--; return false; }
                if (pos - cur->pos < WIDTH) { nodes--; return false; }
                cur->next = new node(interMag(cur, cur->next, pos),pos,cur->next);
                return true;
            }
            if (cur->next->pos == pos) { nodes--; return false; }
            cur = cur->next;
        } while(cur->next != head);
        cur->next = new node(interMag(cur, cur->next, pos), pos, cur->next); tail=cur->next;
        return true;
    }

    bool cut(uint16_t pos) {  //delete @ position
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
    
    void update() {
      node *pre = tail;
      node *cur = head;
      do {
        int8_t newmag = cur->mag + 1;
        
        if (!(abs(newmag) - abs(interMag(pre, cur->next, cur->pos)))) {
          uint16_t del_pos = cur->pos; cur = cur->next; cut(del_pos);
          bool reinsert; do { del_pos = rand()%PIXELS; reinsert = !insertAlive(del_pos); } while (reinsert);
          continue;
        }        
        cur->mag = newmag;
        pre = cur; cur = cur->next;
      } while(cur->next != head);
    }   
    
};