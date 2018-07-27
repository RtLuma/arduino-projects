// There's also this too https://github.com/ivanseidel/LinkedList

#include<iostream>
#include <string>
#include <unistd.h>		//usleep
#include <stdlib.h>		//srand, rand
#include <time.h>       //time 

using namespace std;

string val2block(uint8_t val) {
  // return to_string(val) + " ";
  if (val > 223) return "\u2588";
  if (val > 191) return "\u2587";
  if (val > 159) return "\u2586";
  if (val > 127) return "\u2585";
  if (val > 95)  return "\u2584";
  if (val > 63)  return "\u2583";
  if (val > 31)  return "\u2582";
  return ".";
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


class list { 
  private:
  	node *head, *tail;
	uint8_t nodes;
  public:  
    list() { head=NULL; tail=NULL; nodes=0; }
	
	uint8_t length(void) { return nodes; }
	
	void print() {
		node *temp = new node; temp=head; string chain="";
		while (temp!=NULL) { chain += "[" + to_string(temp->pos) + " " + val2block(temp->mag) + "]"; temp=temp->next; }
		cout << chain << endl;
		// cout << chain.substr(0,chain.length()-4) << endl;
	}
	
	/*            ↓ insert
	 unshift → [chain] ← push
	   shift ← [chain] → pop
		          ↓ cut
	*/
	
	void unshift(uint8_t mag) { //at beginning
		node *temp=new node; temp-> mag=mag; temp->pos=0; temp->next=head; head=temp; nodes++;
	}
	
	bool insert(uint16_t pos, uint8_t mag) { //at position
		node *pre=new node;
		node *cur=new node;
		node *temp=new node; temp->mag=mag; temp->pos=pos; temp->next=NULL;
		cur=head;
		nodes++;
		if (head==NULL)	{ head=temp; tail=temp; temp=NULL; return true; }
		if (head->pos > pos) { temp->next=head; head=temp; temp=NULL; return true; }
		while (cur->next!=NULL) {
			pre=cur;
			cur=cur->next;
			if (pre->pos == pos || cur->pos == pos) { 
				temp=NULL;
				nodes--;
				return false; 
				}
			if (cur->pos > pos) {
				pre->next=temp;
				temp->next=cur;
				temp=NULL;
				return true;
			}
			
		}
		cur->next=temp; temp->next=NULL;
		return true;
	}
	
	void shift() {		//delete first
		node *temp=new node; temp=head; head=head->next; delete temp; nodes--;
	}
	
	void pop() {		//delete last
		node *current=new node; node *previous=new node; current=head;
		while(current->next!=NULL)  { previous=current;	current=current->next; }
		tail=previous; previous->next=NULL; delete current; nodes--;
	}
	
	void cut(uint8_t pos) {	//delete @ position
		node *current=new node; node *previous=new node; current=head;
		for(uint8_t i=1;i<pos;i++) { previous=current; current=current->next; }
		previous->next=current->next; nodes--;
	}
};


#define SCLERA 10
#define SPARKLES 10

int main() {
	list leds;
	srand (time(NULL)*SCLERA); rand();
	for (uint8_t r=(rand()>>23); r>0;r--) rand();
	
	while (leds.length() < SPARKLES) {
		uint16_t pos = rand()%SCLERA;
		uint8_t mag = rand() >> 23; 
		if (leds.insert(pos, mag)) {
			cout << to_string(pos)  << " → ";
			// cout << to_string((uint8_t8_t)) << endl;
			leds.print();
			usleep(1000);
		}
	}
	// cout << endl << to_string(leds.length()) << endl;
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