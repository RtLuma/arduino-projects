#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>		//usleep
#include <stdlib.h>		//srand, rand
#include <time.h>       //time 

using namespace std;

uint16_t PIXELS;
uint8_t R, G, B;

#define ZERO_SYMBOL " "
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

    void print() {
        node *cur = head;
        // uint16_t p=0;
        // for (; cur->next != head;) {
        do {
            // for (; p < cur->pos; p++) printf(ZERO_SYMBOL);
            // p += printGrad(cur, cur->next);
            // printf("[%d, %d] ", cur->pos, cur->mag);
            printf("[%d] ", cur->pos);
            // printSparkle(temp->mag);
            cur=cur->next;
        } while (cur != head);
        printf("\n");
        // for (; p < PIXELS; p++) printf(ZERO_SYMBOL);
    }
    
    // bool emplace(node* pre, node* temp, node* cur) {
    //     if (pre) pre->next = temp;
    //     temp->next = cur;
    //     temp = nullptr;
    //     delete temp;
    //     return true;
    // }

    bool insert(uint16_t pos, uint8_t mag) { 
        if (pos > PIXELS || pos == head->pos) return false;
        mag |= 1; nodes++; node *cur=head;
        
        if (pos < head->pos) { head = new node(mag,pos,head); tail->next=head; return true; }
        
        do { if (cur->next->pos == pos) { nodes--; return false; }
             if (cur->next->pos > pos) { cur->next = new node(mag, pos, cur->next); return true; }
             cur = cur->next;
        } while (cur->next != head);
        
        cur->next = new node(mag, pos, cur->next); tail=cur->next; return true;
    }

    bool cut(uint16_t pos) {  //delete @ position
        node *cur = head;
        node *pre = nullptr;
        while (cur != nullptr) {
            if (cur->pos == pos) {
                if (pre != nullptr) pre->next = cur->next;
                else head = cur->next;
                delete cur;
                nodes--;
                return true;
            }
            if (cur->pos > pos) return false;
            pre = cur;
            cur = cur->next;
        }
        return false;
    }
    
    bool checkOrder() {
        node *cur = head;
        do {
            if (cur->next->pos - cur->pos != 1) return false;
            cur=cur->next;
        } while (cur->next != head);
        return true;
    }
};

struct winsize w;

// #define SPARKLES 50

int main() {

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    PIXELS = w.ws_col-2;
    // uint16_t SCRAMBLE = PIXELS/3-1;
    for (uint32_t T=0; T<1000; T++) {
    
        list sparkles;

        srand(time(NULL)*T*T*T); rand();
    
        uint8_t i_s[SCRAMBLE];
        for (uint8_t i=0; i < SCRAMBLE; i++) i_s[i]=i+1;
        for (uint8_t i=0; i < SCRAMBLE; i++) {
            uint8_t r1 = rand()%SCRAMBLE;
            uint8_t r2 = rand()%SCRAMBLE;
            if (r1 > PIXELS || r2 > PIXELS) {i--; continue;}
            uint8_t t = i_s[r1]; 
            i_s[r1] = i_s[r2];
            i_s[r2] = t;
        }
        // for (uint8_t i=0; i < SCRAMBLE; i++) printf("%d ", i_s[i]); printf("\n");       
        for (uint8_t i=0; i < SCRAMBLE; i++) {
            sparkles.insert(i_s[i], 0);
            printf("%d\t", i_s[i]);
            sparkles.print();
        }
        
        if (!sparkles.checkOrder()) {
            printf("\n\n\n\n\n\nWeak!\n\n\n\n\n");
            exit(1);
        }
        printf("\n");
        // sparkles.print();
    }
    
    // sparkles.print();
    
}