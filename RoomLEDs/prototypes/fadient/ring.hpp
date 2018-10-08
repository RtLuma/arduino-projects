#include <iostream>
#include <string>

#include "node.hpp"

extern uint16_t PIXELS;
extern uint8_t R, G, B;

#define ZERO_SYMBOL " "

// int8_t intersat(node* A, node* B, uint16_t pos) {
//     uint16_t sat = abs(A->sat) << 8;
//     uint16_t dist = B->pos - A->pos;
//     if (!dist) return 0;
//     uint16_t dFromA = pos - A->pos;
//     int16_t sat_step = int16_t(abs(B->sat<<8) - sat)/dist;
//     sat += sat_step * dFromA;
//     sat >>= 8;
//     return rand() & 1 ? -sat : sat;
// }    

struct node {
    uint16_t pos;
    int8_t sat;
    node *next;
    node() {
        this->sat=sat;
        this->pos=0;
        this->next=this;
    }

    virtual void print() = 0;
};

struct monode : public node {

    monode(int8_t sat, uint16_t pos, node* next) {
        this->pos=pos;
        this->next=next;
    }

    monode() {
        this->sat=0;
        this->pos=0;
        this->next=this;
    }

     void print() {
        uint8_t disp = abs(sat);
        if (disp < 128) disp <<= 1;
        else disp = 255;
        printf("\033[38;2;%d;%d;%dm█\033[0;m", (uint16_t(R * disp) + 1) >> 8, (uint16_t(G * disp) + 1) >> 8, (uint16_t(B * disp) + 1) >> 8);
    } //]]
};

class ring {
private:
    node *head; node *tail; uint8_t nodes;
    
public:
    ring() { head = new node; tail = head; nodes = 0; }
    auto getHead()  { return head;  }
    auto getTail()  { return tail;  }
    auto getNodes() { return nodes; }
    void populate (uint8_t desiredNodes) { while (nodes < desiredNodes) insert(rand()%PIXELS, rand()); }
    void terminate(uint8_t desiredNodes) { while (nodes > desiredNodes) remove(head->pos); }
    
    void printDiscrete() {
		node *n = head;
		uint16_t p=0;
		for (; n->next != head;) {
			for (; p < n->pos; p++) printf(ZERO_SYMBOL);
			n->print();
            p++;
            n=n->next;
		}
		for (; p < PIXELS; p++) printf(ZERO_SYMBOL);
	}
    
    // void printContinuous() {
	// 	node *n = head;
    //     uint16_t p=0;
    //     if (head->pos > 0) {
    //         head->pos += PIXELS;
    //         node first(intersat(tail, head, PIXELS), 0, head);
    //         temp=&first;
    //         head->pos -= PIXELS;
    //         p=printGrad(temp, head);
    //         temp=head;
    //     }        
	// 	for (; temp->next != head;) {
	// 		for (; p < temp->pos; p++) printf(ZERO_SYMBOL);
	// 		p += printGrad(temp, temp->next);
    //         temp=temp->next;
	// 	}
	// 	for (; p < PIXELS; p++) printf(ZERO_SYMBOL);
	// }
    
    // uint8_t printGrad(node* A, node* B) {
    //     uint16_t sat = abs(A->sat) << 8; uint16_t dist = B->pos - A->pos;
    //     if (!dist) return 0;
    //     int16_t sat_step = int16_t(abs(B->sat<<8) - sat)/dist;
    //     uint16_t p = 1;
    //     //   printSparkle(sat>>8);
    //     //   printSpawn(sat>>8);
    //     nodePrint(sat>>8);
    //     sat += sat_step;
    //     for (; p < dist; p++) { printSparkle(sat>>8); sat += sat_step; }
    //     return dist;
    // }
    
    bool insert(uint16_t pos, int8_t sat) { // bool 'alive' instead of sat arg
        nodes++; node *cur=head;
        if (pos < head->pos) { head = new node(sat,pos,head); tail->next=head;       return true;  }
        do { if (cur->next->pos == pos) {                                   nodes--; return false; }
             if (cur->next->pos >  pos) { cur->next = new node(sat, pos, cur->next); return true;  }
             cur = cur->next; } while (cur->next != head);
        cur->next = new node(sat, pos, cur->next); tail=cur->next;                   return true;
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