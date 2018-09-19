#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>		//usleep
#include <stdlib.h>		//srand, rand
#include <time.h>       //time 

using namespace std;

uint16_t PIXELS;
uint8_t R, G, B;

#define ZERO_SYMBOL "...."
#define TEST_FAIL_MSG "\n\n\n\n\n\nWeak!\n\n\n\n\n"
#define SCRAMBLE 20

struct node {
    int8_t mag; uint16_t pos; node *next;
    node(int8_t mag, uint16_t pos, node *next) { this->mag=mag; this->pos=pos; this->next=next; }
    node() { this->mag=0; this->pos=rand()%SCRAMBLE; this->next=this; }
};

class list {
private:
    node *head;
    node *tail;
    uint8_t nodes;

public:
    list() {
        head = new node;
        tail = head;
        nodes = 0;
    }
    
    auto getHead()  { return head;  }
    auto getNodes() { return nodes; }

    void print() { node *cur = head; do { printf("[%d] ", cur->pos); cur=cur->next; } while (cur != head); printf("\n"); }
    
    void proxPrint() {
        node *cur = head;
        uint16_t p=0;
        do {
            for (; p < cur->pos; p++) printf(ZERO_SYMBOL);
            // printf("[%d, %d] ", cur->pos, cur->mag);
            printf("[%02d]", cur->pos); p++;
            cur=cur->next;
        } while (cur != head);
        printf("\n");
    }

    bool insert(uint16_t pos, uint8_t mag) { 
        if (pos > PIXELS || pos == head->pos)                                        return false;
        mag |= 1; nodes++; node *cur=head;
        if (pos < head->pos) { head = new node(mag,pos,head); tail->next=head;       return true;  }
        do { if (cur->next->pos == pos) {                                   nodes--; return false; }
             if (cur->next->pos >  pos) { cur->next = new node(mag, pos, cur->next); return true;  }
             cur = cur->next; } while (cur->next != head);
        cur->next = new node(mag, pos, cur->next); tail=cur->next;                   return true;
    }
    
    
    bool cut(uint16_t pos) {  //delete @ position
        if (pos > PIXELS || pos < head->pos)                                        return false;
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
    
    bool checkSequence() {
        if (nodes < 1) return true;
        node *cur = head;
        do {
            if (cur->next->pos - cur->pos != 1) return false;
            cur=cur->next;
        } while (cur->next != head);
        return true;
    }
    
    bool checkSort() {
        if (nodes < 1) return true;
        node *cur = head;
        do {
            if (cur->next->pos - cur->pos < 1) return false;
            cur=cur->next;
        } while (cur->next != head);
        return true;
    }
    
    
};

struct winsize w;

// #define SPARKLES 50

int main() {
    srand(time(NULL));
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    PIXELS = w.ws_col-2;
    // uint16_t SCRAMBLE = PIXELS/3-1;
    
    for (uint32_t T=0; T<1000; T++) { // Run test a bunch of times to avoid serendipity
        printf("Test %d #############################\n", T);
        list sparkles;
        printf("%d\t", (sparkles.getHead())->pos);
        sparkles.proxPrint();
        
        rand();
    
        uint8_t i_s[SCRAMBLE];
        for (uint8_t i=0; i < SCRAMBLE; i++) i_s[i]=i; // Init array in order
        
        for (uint8_t i=0; i < SCRAMBLE; i++) {  //Randomly scramble all indices
            uint8_t r1 = rand()%SCRAMBLE;
            uint8_t r2 = rand()%SCRAMBLE;
            if (r1 > PIXELS || r2 > PIXELS) {i--; continue;}
            uint8_t t = i_s[r1]; 
            i_s[r1] = i_s[r2];
            i_s[r2] = t;
        }

        for (uint8_t i=0; i < SCRAMBLE; i++) {  // Push the random order, should end up sorted
            sparkles.insert(i_s[i], 0);
            printf("%d\t", i_s[i]);
            sparkles.proxPrint();
            if (!sparkles.checkSort()) { printf("sort"); printf(TEST_FAIL_MSG); exit(1); }
        }
        
        // LL should be full, make sure it's in exact sequential order.
        if (!sparkles.checkSequence()) { printf("seq"); printf(TEST_FAIL_MSG); exit(1); }
        
        // Now to empty it out
        
        for (uint8_t i=1; i < SCRAMBLE; i++) {  // Should be fine to cut in the same random order
            sparkles.cut(i_s[i]);
            printf("-%d\t", i_s[i]);
            sparkles.proxPrint();
            if (!sparkles.checkSort()) { printf(TEST_FAIL_MSG); exit(1); }
        }
        
        printf("Sole survivor: ");
        sparkles.print();
        printf("\n");
    }
}