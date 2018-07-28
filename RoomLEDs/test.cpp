// There's also this too https://github.com/ivanseidel/LinkedList

#include<iostream>
#include <string>
#include <unistd.h>		//usleep
#include <stdlib.h>		//srand, rand
#include <time.h>       //time 

using namespace std;

#define ZERO_SYMBOL " " 

string val2block(uint8_t val) {
  // return to_string(val) + " ";
  if (val > 226) return "\u2588";
  if (val > 198) return "\u2587";
  if (val > 169) return "\u2586";
  if (val > 141) return "\u2585";
  if (val > 113) return "\u2584";
  if (val > 84)  return "\u2583";
  if (val > 56)  return "\u2582";
  if (val > 28)  return "\u2581";
  				 return ZERO_SYMBOL;
} 

void printColor(uint8_t r, uint8_t g, uint8_t b) {
  printf("\r%c[%dm", 0x1B, 31); cout << val2block(r);
  printf("%c[%dm", 0x1B, 32); cout << val2block(g);
  printf("%c[%dm", 0x1B, 34); cout << val2block(b);
  printf("%c[%dm", 0x1B, 0);
  fflush(stdout);
  usleep(20000);
}
//]]]]

//Store whether fading in or out in high bit of pos
struct node { uint8_t mag; uint16_t pos; node *next; };

#define SCLERA 75
#define SPARKLES 50


class list { 
  private:
  	node *head, *tail;
	uint8_t nodes;
  public:  
    list() { head=NULL; tail=NULL; nodes=0; }
	
	uint8_t length(void) { return nodes; }
	
	void print() {
		node *temp = new node; temp=head; string chain="";
		uint16_t p;
		for (p=0; temp != NULL;) {
			for (p; p < (0x7fff & temp->pos); p++) chain += ZERO_SYMBOL;
			chain += val2block(temp->mag); p++;
			temp=temp->next;
		}
		for (p; p < SCLERA; p++) chain += ZERO_SYMBOL;
		// while (temp!=NULL) { chain += "[" + to_string(temp->pos) + val2block(temp->mag) + "]"; temp=temp->next; }
		cout << chain;
		// cout << chain.substr(0,chain.length()-4) << endl;
	}
	
	bool insert(uint16_t pos, uint8_t mag) { //at position
		pos &= 0x7fff;
		node *pre=new node;
		node *cur=new node;
		node *temp=new node; temp->mag=mag; temp->pos=pos; temp->next=NULL;
		cur=head;
		nodes++;
		if (head==NULL)	{ head=temp; tail=temp; temp=NULL; return true; }
		if ((0x7fff & head->pos) > pos) { temp->next=head; head=temp; temp=NULL; return true; }
		while (cur->next!=NULL) {
			pre=cur;
			cur=cur->next;
			if ((0x7fff & pre->pos) == pos || (0x7fff & cur->pos) == pos) { 
				temp=NULL;
				nodes--;
				return false; 
				}
			if ((0x7fff & cur->pos) > pos) {
				pre->next=temp;
				temp->next=cur;
				temp=NULL;
				return true;
			}
			
		}
		cur->next=temp; temp->next=NULL;
		return true;
	}
	
	bool cut(uint16_t pos) {	//delete @ position
		node *cur=new node;
		node *pre=new node;
		cur=head;
		while (cur->next != NULL) {
			if (cur->pos == pos) {
				pre->next=cur->next;
				delete cur;
				nodes--;
				return true;
			}
			if (cur->pos > pos) return false;
			pre=cur;
			cur=cur->next;
		}
		return false;
	}
	
	void update() {
		node *cur=head;
		while (cur->next!=NULL) {
			if (cur->pos & 0x8000) {
				if (!--(cur->mag)) {
					// cut(cur->pos);
					cur->pos &= 0x7fff;
				}
			}
			else if (!++(cur->mag)) { cur->mag = 255; cur->pos |= 0x8000; }
			cur=cur->next;
		}
	}
};


int main() {
	list leds;
	srand (time(NULL)<<10); rand();
	for (uint8_t r=(rand()>>23); r>0; r--) rand();
	cout << endl;
	while (leds.length() < SPARKLES) {
		uint16_t pos = (rand()%SCLERA) | ((rand() & 0x80000000) ? 0x8000 : 0);
		uint8_t mag = rand() >> 23; 
		leds.insert(pos, mag);
	}
	
	while (true) {
		leds.update();
		cout << "\r";
		leds.print();
		fflush(stdout);
		usleep(2000);
	}
}





// uint8_t main() {
// 	list obj;
// 	obj.push(25);
// 	obj.push(50);
// 	obj.push(90);
// 	obj.push(40);
// 	cout<<"--------printing All nodes--------\n";
// 	obj.print();
// 	cout << "(" << to_string(obj.length()) << ")\n";
// 	cout<<"\n----------Inserting At End----------\n";
// 	obj.push(55);
// 	obj.print();
// 	cout << "(" << to_string(obj.length()) << ")\n";
// 	cout<<"\n---------Inserting At Start---------\n";
// 	obj.unshift(50);
// 	obj.print();
// 	cout << "(" << to_string(obj.length()) << ")\n";
// 	cout<<"\n---------Inserting At Particular-------\n";
// 	obj.insert(5,60);
// 	obj.print();
// 	cout << "(" << to_string(obj.length()) << ")\n";
// 	cout<<"\n---------Deleting At Start----------\n";
// 	obj.shift();
// 	obj.print();
// 	cout << "(" << to_string(obj.length()) << ")\n";
// 	cout<<"\n----------Deleing At End------------\n";
// 	obj.pop();
// 	obj.print();
// 	cout << "(" << to_string(obj.length()) << ")\n";
// 	cout<<"\n-------Deleting At Particular-------\n";
// 	obj.cut(2);
// 	obj.print();
// 	cout << "(" << to_string(obj.length()) << ")\n";
// 	return 0;
// }