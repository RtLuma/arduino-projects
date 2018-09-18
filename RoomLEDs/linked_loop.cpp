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

struct node {
    int8_t mag;
    uint16_t pos;
    node *next;
};

class list {
private:
    node *head;
    uint8_t nodes;

public:
    list() {
        head = new node;
        head->pos=0;
        head->mag=1;
        head->next=head;
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

    bool insert(uint16_t pos, uint8_t mag) { //at position
        printf("Inserting @ %d\n",pos);
        if (pos > PIXELS || pos < 0) return false;
        node *pre = nullptr;
        node *cur = head;
        mag |= 1;
        node *temp = new node;
        temp->mag = mag;
        temp->pos = pos;
        temp->next = nullptr;
        nodes++;
        do {
            if (cur->pos == pos) { delete temp; nodes--; printf("\t%d invalid\n", pos); return false; }
            if (cur->pos > pos) {
                if (pre) pre->next = temp;
                temp->next = cur;
                // temp = nullptr;
                // delete temp;
                
                if (pre) printf("\t%d between %d and %d\n",pos, pre->pos, cur->pos);
                else printf("\t%d before %d\n",pos, cur->pos);
                
                return true;
            }
            pre = cur;
            cur=cur->next;
        } while (cur != head);
        pre->next = temp;
        temp->next = cur;
        
        // temp = nullptr;
        // delete temp;
        
        printf("\t%d after %d\n", pos, pre->pos);
        
        return true;
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
    
    // bool checkOrder() {
    //     node *cur = head;
    //     do {
    //         if (cur->pos == pos) { delete temp; nodes--; printf("\t%d invalid\n", pos); return false; }
    //         if (cur->pos > pos) {
    //             if (pre) pre->next = temp;
    //             temp->next = cur;
    //             // temp = nullptr;
    //             // delete temp;
                
    //             if (pre) printf("\t%d between %d and %d\n",pos, pre->pos, cur->pos);
    //             else printf("\t%d before %d\n",pos, cur->pos);
                
    //             return true;
    //         }
    //         pre = cur;
    //         cur=cur->next;
    //     } while (cur != head);
    // }
};

struct winsize w;

#define SPARKLES 100
#define SCRAMBLE SPARKLES-1

int main() {

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    list sparkles;
    PIXELS = SPARKLES*2;
    // PIXELS = w.ws_col-2;
    
    srand(time(NULL)); rand();
    
    uint8_t i_s[SCRAMBLE];
    for (uint8_t i=0; i < SCRAMBLE; i++) i_s[i]=i+1;
    for (uint8_t i=0; i < SCRAMBLE; i++) printf("%d ", i_s[i]); printf("\n");
    for (uint8_t i=0; i < SCRAMBLE; i++) {
        uint8_t r1 = rand()%SCRAMBLE;
        uint8_t r2 = rand()%SCRAMBLE;
        if (r1 > PIXELS || r2 > PIXELS) {i--; continue;}
        printf("[%d %d]",r1,r2);
        uint8_t t = i_s[r1]; 
        i_s[r1] = i_s[r2];
        i_s[r2] = t;
    }printf("\n");
    for (uint8_t i=0; i < SCRAMBLE; i++) printf("%d ", i_s[i]); printf("\n");
    
    for (uint8_t i=0; i < SCRAMBLE; i++) {
        sparkles.insert(i_s[i], 0);
    }
    sparkles.print();
    
}