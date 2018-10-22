#include <iostream>
#include <string>

extern uint16_t PIXELS;
extern uint8_t R, G, B, P;

#define ZERO_SYMBOL " "
#define GRADIENT_WIDTH 3

/*  32-bit node protohue:
  |Position.........| hue   |  Lum   |
  |######## ########|########|########| */

struct Ring {
    uint32_t* ring; uint8_t nodes;
    
    Ring() { nodes = 0; }
    
    //node ops [
        static uint16_t pos(uint32_t n) { return  (n &  0xffff0000) >> 16; }
        static uint16_t hue(uint32_t n) { return  (n &  0x0000ff00) >> 8;  }
        static uint16_t lum(uint32_t n) { return   n &  0x000000ff;        }
        static void pos(uint32_t& n, uint16_t p) { n &= 0x0000ffff; n |= (uint32_t)p << 16; }
        static void hue(uint32_t& n, uint8_t  l) { n &= 0xffff00ff; n |= (uint32_t)l << 8;  }
        static void lum(uint32_t& n, uint8_t  h) { n &= 0xffffff00; n |= h;                 }
    //]
    
    void sort() { // Insertion sort
        uint8_t i, j; uint32_t k; 
        for (i = 1; i < nodes; i++) { 
            j = i-1; k = ring[i];
            while (j >= 0 && pos(ring[j]) > pos(k)) { ring[j+1] = ring[j]; j--; } 
            ring[j+1] = k; 
        } 
    }
    
    void init(uint8_t desiredNodes) { 
        ring = new uint32_t[desiredNodes];
        if (desiredNodes > nodes) {
            for (uint8_t i=nodes; i < desiredNodes; i++) ring[i]=initNode();
        }
        nodes = desiredNodes;
        sort();
    } 
        
    void populate (uint8_t desiredNodes) { 
        delete[]ring;
        uint32_t n0=ring[0],n1=ring[1];
        ring = new uint32_t[desiredNodes];
        if (desiredNodes > nodes) {
            for (uint8_t i=nodes; i < desiredNodes; i++) ring[i]=initNode();
        }
        nodes = desiredNodes;
        ring[0]=n0;ring[1]=n1;
        sort();
    }
    
    uint32_t initNode() {
        uint32_t n;
        n =  (rand()%PIXELS) << 16;
        n += (rand() >> 24) << 8;
        n +=  rand() >> 24;
        // pos(n, rand()%PIXELS);
        // hue(n, rand());
        // lum(n, rand());
        printf("init: %03d, %03d @ %03d\t%08x\n", lum(n), hue(n), pos(n), n);
        return n;
    }
    
    void printNode(uint32_t n) { 
        printf("[%03d,%03d@%03d:%08x]", lum(n), hue(n), pos(n), n);
        // printf("[%03d]", (n)); 
    }
    // void printNode(uint32_t n) { printf("[%03d,%03d@%03d]", lum(n),hue(n),pos(n)); }
    
    void print() {
        for (uint8_t i=0; i < nodes; i++) printNode(ring[i]);
        printf("\n");
    }
    
    std::string to_string() {
        std::string s;
        for (uint8_t i=0; i < nodes; i++) {
            s += "[";
            s += std::to_string(lum(ring[i]));
            s += ",";
            s += std::to_string(hue(ring[i]));
            s += "@";
            s += std::to_string(pos(ring[i]));
            s += ":";
            s += std::to_string(ring[i]);
            s += "]";
        }
        return s;
        
    }
    
    void displayNode(uint32_t n) {}
    void display() {}
    
    
    // void insert() {
    //     uint8_t i = rand()%P;
    //     uint32_t n = pos(ring[i]);
    //     n += pos(ring[i+1]);
    //     n >>= 1;
    //     lum(n, rand());
    //     nodes++;
    // }
    
//     int8_t interpolate(node* A, node* B, uint16_t pos) {
//         uint16_t dist = B->pos - A->pos;
//         if (!dist) return 0;
//         uint16_t lum = abs(A->lum) << 8;
//         uint16_t dFromA = pos - A->pos;
//         int16_t lum_step = int16_t(abs(B->lum<<8) - lum)/dist;
//         lum += lum_step * dFromA;
//         lum >>= 8;
//         return rand() & 1 ? -lum : lum;
//     }    
    
//     void updateDiscrete(void) {
//       node *n = head;
//       do {
//         int8_t newlum = n->lum + 1;
        
//         if (n->lum<0 && newlum>-1) {
//           uint16_t del_pos = n->pos; n = n->next; remove(del_pos);
//           bool reinject; do { reinject = !insert(rand()%PIXELS, 0); } while (reinject);
//           continue;
//         }        
//         n->lum = newlum;
//         n = n->next;
//       } while(n->next != head);
//     }
        
//     void printDiscrete() {
// 		node *n = head;
// 		uint16_t p=0;
// 		for (; n->next != head;) {
// 			for (; p < n->pos; p++) printf(ZERO_SYMBOL);
// 			printNode(n->lum);
//             p++;
//             n=n->next;
// 		}
// 		for (; p < PIXELS; p++) printf(ZERO_SYMBOL);
// 	}
    
//     void updateContinuous() {
//       node *pre = tail, *cur = head;
//       do {
//         int8_t newlum = cur->lum + 1;
        
//         if (!(abs(newlum) - abs(interpolate(pre, cur->next, cur->pos)))) {
//           uint16_t del_pos = cur->pos; cur = cur->next; remove(del_pos);
//           bool reinject; do { del_pos = rand()%PIXELS; reinject = !inject(del_pos); } while (reinject);
//           continue;
//         }        
//         cur->lum = newlum;
//         pre = cur; cur = cur->next;
//       } while(cur->next != head);
//     }
    
//     void printContinuous() {
//         node *n = head;
//         head->pos += PIXELS;
//         node origin(interpolate(tail, head, PIXELS), 0, head);
//         head->pos -= PIXELS;
        
//         uint16_t p=0;
        
//         n=&origin;
//         p=printGradient(n, head);
//         n=head;
                
//         for (; n->next != head;) {
//             p += printGradient(n, n->next);
//             n=n->next;
//         }

//         origin.pos=PIXELS;
//         n=&origin;
//         p=printGradient(tail, n);
// 	}
    
//     void printNode(int8_t lum) {
//         uint8_t disp = abs(lum);
//         if (disp < 128) disp <<= 1;
//         else disp = 255;
//         printf("\033[38;2;%d;%d;%dm█\033[0;m", (uint16_t(R * disp) + 1) >> 8, (uint16_t(G * disp) + 1) >> 8, (uint16_t(B * disp) + 1) >> 8);
//     } //]]
    
//     uint8_t printGradient(node* A, node* B) {
//         uint16_t lum = abs(A->lum) << 8; uint16_t dist = B->pos - A->pos;
//         if (!dist) return 0;
//         int16_t lum_step = int16_t(abs(B->lum<<8) - lum)/dist;
//         uint16_t p = 1;
//         // printNode(lum>>8);
//         printf("\033[38;2;255;255;255m█\033[0;m");
//         lum += lum_step;
//         for (; p < dist; p++) { printNode(lum>>8); lum += lum_step; }
//         return dist;
//     }
    
//     bool insert(uint16_t pos, int8_t lum) { // bool 'alive' instead of lum arg
//         nodes++; node *cur=head;
//         if (pos < head->pos) { head = new node(lum,pos,head); tail->next=head;       return true;  }
//         do { if (cur->next->pos == pos) {                                   nodes--; return false; }
//              if (cur->next->pos >  pos) { cur->next = new node(lum, pos, cur->next); return true;  }
//              cur = cur->next; } while (cur->next != head);
//         cur->next = new node(lum, pos, cur->next); tail=cur->next;                   return true;
//     }
    
//     bool inject(uint16_t pos) {
//         if (pos > PIXELS || pos == head->pos) return false;
//         node *cur = head;
//         nodes++; 
//         if (pos < head->pos) {
//             head = new node(interpolate(cur, cur->next, pos),pos,head);
//             tail->next=head;
//             return true;
//         }
//         do {
//             if (cur->next->pos > pos ) {
//                 if (cur->next->pos - pos < GRADIENT_WIDTH) { nodes--; return false; }
//                 if (pos - cur->pos       < GRADIENT_WIDTH) { nodes--; return false; }
//                 cur->next = new node(interpolate(cur, cur->next, pos),pos,cur->next);
//                 return true;
//             }
//             if (cur->next->pos == pos) { nodes--; return false; }
//             cur = cur->next;
//         } while(cur->next != head);
//         cur->next = new node(interpolate(cur, cur->next, pos), pos, cur->next);
//         tail=cur->next;
//         return true;
//     }
    
//     bool remove(uint16_t pos) {  //delete @ position
//         if (pos > PIXELS || pos < head->pos) return false;
//         node *cur = head;
//         if (pos == head->pos) {
//             nodes--;
//             node* temp=head->next;
//             delete head; head=temp;
//             tail->next=head;
//             return true;
//         }
//         do {
//             if (cur->next->pos >  pos) return false;
//             if (cur->next->pos == pos) {
//                 node *temp = cur->next->next; 
//                 delete cur->next;
//                 cur->next = temp;
//                 nodes--;
//                 if (cur->next == head) tail = cur;
//                 return true;
//             }
//             cur = cur->next;
//         } while (cur->next != head);
//         return false;
//     }
};

// void monoDiscrete(void) { mleds.updateDiscrete(); mleds.printDiscrete(); }
// void monoContinuous(void) { mleds.updateContinuous(); mleds.printContinuous(); }
