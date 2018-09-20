#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>		//usleep
#include <stdlib.h>		//srand, rand
#include <time.h>       //time 

using namespace std;

uint16_t PIXELS;
uint8_t R, G, B;

#define ZERO_SYMBOL "."
#define TEST_FAIL_MSG "\n\n\n\n\n\nWeak!\n\n\n\n\n"
#define SCRAMBLE 20
string val2block(uint8_t val) {if (val > 223) return "\u2588";  if (val > 191) return "\u2587";  if (val > 159) return "\u2586";  if (val > 127) return "\u2585";  if (val > 95)  return "\u2584";  if (val > 63)  return "\u2583";  if (val > 31)  return "\u2582";  return " ";} 

void printSparkle(int8_t mag) {
	uint8_t disp = abs(mag); if (disp < 128) disp <<= 1; else disp = 255;
	printf("\033[38;2;%d;%d;%dm", (uint16_t(R * disp) + 1) >> 8, (uint16_t(G * disp) + 1) >> 8, (uint16_t(B * disp) + 1) >> 8);
	cout << val2block(disp);
	printf("\033[0;m");
	
} //]]

void printSpawn(int8_t mag) {
	uint8_t disp = abs(mag); if (disp < 128) disp <<= 1; else disp = 255;
    printf("\033[38;2;255;255;255m");
	cout << val2block(disp);
	printf("\033[0;m");
	
} //]]

struct node {
    int8_t mag; uint16_t pos; node *next;
    node(int8_t mag, uint16_t pos, node *next) { this->mag=mag; this->pos=pos; this->next=next; }
    node() { this->mag=0; this->pos=rand()%SCRAMBLE; this->next=this; }
};

uint8_t printGrad(node* A, node* B) {
  int16_t mag = A->mag << 8; uint16_t dist = B->pos - A->pos;
  int16_t mag_step = int16_t((B->mag<<8) - mag)/dist; uint16_t p = 1;
//   printSpawn(mag>>8); mag += mag_step;
  printSparkle(mag>>8); mag += mag_step;
  for (; p < dist; p++) { printSparkle(mag>>8); mag += mag_step; }
  return dist;
}

int8_t spawnMag(node* A, node* B, uint16_t pos) {
    // return 0;
    int16_t dist =  B->pos - A->pos;
    int16_t distA = pos - A->pos;
    int16_t distB = B->pos - pos;
    if (dist > 0) {
    int16_t magA = (distA * A->mag) ; 
    int16_t magB = (distB * B->mag) ; 
    return -(magA + magB) / dist;
    }
    else return 0;
}    
    

class list {
private:
    node *head; node *tail; uint8_t nodes;

public:
    list() { head = new node; tail = head; nodes = 0; }
    auto getHead()  { return head;  }
    auto getNodes() { return nodes; }
    void populate (uint8_t sparkles) { while (nodes < sparkles) insert(rand()%PIXELS, rand()%256); }
    void terminate(uint8_t sparkles) { while (nodes > sparkles) cut(head->pos); }

    void print() {
		node *temp = head; uint16_t p=0;
		for (; temp->next != head;) {
			for (; p < temp->pos; p++) printf(ZERO_SYMBOL);
			p += printGrad(temp, temp->next);
            temp=temp->next;
		}
		for (; p < PIXELS; p++) printf(ZERO_SYMBOL);
	}
    

    bool insert(uint16_t pos, uint8_t mag) { // bool 'alive' instead of mag arg
        nodes++; node *cur=head;
        if (pos < head->pos) { head = new node(mag,pos,head); tail->next=head;       return true;  }
        do { if (cur->next->pos == pos) {                                   nodes--; return false; }
             if (cur->next->pos >  pos) { cur->next = new node(mag, pos, cur->next); return true;  }
             cur = cur->next; } while (cur->next != head);
        cur->next = new node(mag, pos, cur->next); tail=cur->next;                   return true;
    }
    
    bool insertAlive(uint16_t pos) {
        if (pos > PIXELS || pos == head->pos) return false;
        nodes++; node *pre = tail; node *cur = head;
        if (pos < head->pos) {
            
            head = new node(spawnMag(pre, cur->next, pos),pos,head);
            
            tail->next=head;
            return true;
        }
        do {
            if (cur->next->pos == pos) { nodes--; return false; }
            if (cur->next->pos >  pos) {
            
            
            cur->next = new node(spawnMag(pre, cur->next, pos),pos,cur->next);

            return true;
            }
            pre = cur;
            cur = cur->next;
        } while(cur->next != head);
        cur->next = new node(spawnMag(pre, cur->next, pos), pos, cur->next); tail=cur->next;                   return true;
        return true;
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
    
    void update() {             // Kill on < midpoint
      node *cur = head;
      do {
        int8_t newmag = cur->mag + 1;
        
        if (cur->mag<0 && newmag>-1) {
          uint16_t del_pos = cur->pos; cur = cur->next; cut(del_pos);
          bool reinsert; do { reinsert = !insertAlive((rand()%PIXELS)); } while (reinsert);
          continue;
        }        
        cur->mag = newmag;
        cur = cur->next;
      } while(cur->next != head);
    }   
    
};

struct winsize w;

#define SPARKLES 15

int main() {
    srand(time(NULL));
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	list sparkles;
	PIXELS = w.ws_col-2;
	sparkles.populate(SPARKLES);
    sparkles.insert(PIXELS,1);
    R=rand(); G=rand(); B=rand();
	while (true) {
        sparkles.update();
        cout << "\r[";
        sparkles.print();
        cout << "]";
        fflush(stdout);
        usleep(10000);
        printf("\r");
	}
}