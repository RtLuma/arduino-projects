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

string val2block(uint8_t val) {  if (val > 223) return "\u2588";  if (val > 191) return "\u2587";  if (val > 159) return "\u2586";  if (val > 127) return "\u2585";  if (val > 95)  return "\u2584";  if (val > 63)  return "\u2583";  if (val > 31)  return "\u2582";  return " ";} 

void printSparkle(int8_t mag) {
	uint8_t disp = abs(mag); if (disp < 128) disp <<= 1; else disp = 255;\
	printf("\033[38;2;%d;%d;%dm", (uint16_t(R * disp) + 1) >> 8, (uint16_t(G * disp) + 1) >> 8, (uint16_t(B * disp) + 1) >> 8);
	cout << val2block(disp);
	printf("\033[0;m");
	
} //]]

struct node { int8_t mag; uint16_t pos; node *next; };

class list { 
  private:
  	node *head, *tail;
	uint8_t nodes;
  public:  
	list() { head = nullptr; tail = nullptr; nodes = 0; }
    
    void populate(uint8_t sparkles) {      
      while (nodes < sparkles) {
        uint8_t m = (rand()%256);
        uint16_t p = (rand()%PIXELS);
        insert(p, m);
      }
    }

    void terminate(uint8_t sparkles) { while (nodes > sparkles) cut(head->pos); }
	
	void print() {
		node *temp = head;
		uint16_t p;
		for (p=0; temp != nullptr;) {
			for (; p < temp->pos; p++) printf(ZERO_SYMBOL);
			printSparkle(temp->mag); p++; temp=temp->next;
		}
		for (; p < PIXELS; p++) printf(ZERO_SYMBOL);
	}
	
	   bool insert(uint16_t pos, uint8_t mag) { //at position
      node *pre;
      node *cur = head;
      mag |= 1;
      node *temp = new node; temp->mag = mag; temp->pos = pos; temp->next = nullptr;
      nodes++;
      if (head == nullptr)  {
        head = temp;
        tail = temp;
        temp = nullptr;
        delete temp;
        return true;
      }
      if (head->pos > pos) {
        temp->next = head;
        head = temp;
        temp = nullptr;
        delete temp;
        return true;
      }
      while (cur->next != nullptr) {
        pre = cur;
        cur = cur->next;
        if (pre->pos == pos || cur->pos == pos) {
          delete temp;
          nodes--;
          return false;
        }
        if (cur->pos > pos) {
          if (pre != nullptr) pre->next = temp;
          temp->next = cur;
          temp = nullptr;
          delete temp;
          return true;
        }

      }
      cur->next = temp; temp->next = nullptr;
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

    void update() {
      node *cur = head;
      while (cur != nullptr) {

        int8_t newmag = cur->mag + 1;
        
        if (cur->mag<0 && newmag>-1) {
          if ((rand()%5)!=4) { cur->mag=-1; cur=cur->next; continue; }
          uint16_t del_pos = cur->pos;
          cur = cur->next;
          cut(del_pos);
          bool reinsert;
          do { reinsert = !insert((rand()%PIXELS), 0); } while (reinsert);
          continue;
        }        
        cur->mag = newmag;
        cur = cur->next;
      }
    }
};

struct winsize w;

#define SPARKLES 20

int main() {
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	list sparkles;
	PIXELS = w.ws_col-2;
	sparkles.populate(SPARKLES);
	srand(time(NULL)); rand();
	for (uint8_t r=rand(); r>0; r--) rand();
	
	while (true) {
		R=rand(); G=rand(); B=rand();
		for (uint8_t i = 1; i; i++) {
			sparkles.update();
			cout << "\r[";
			sparkles.print();
			cout << "]";
			fflush(stdout);
			usleep(10000);
		}
		printf("\n");
	}
}