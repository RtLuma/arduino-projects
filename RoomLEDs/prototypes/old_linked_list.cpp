// There's also this too https://github.com/ivanseidel/LinkedList

#include<iostream>
#include <string>
using namespace std;

struct node {	int8_t data;	node *next;	};

class list { 
  private:
  	node *head, *tail;
	uint8_t nodes;
  public:  
    list() { head=NULL; tail=NULL; nodes=0; }
	
	uint8_t length(void) { return nodes; }
	
	void print() {
		node *temp = new node; temp=head; string chain="";
		while (temp!=NULL) { chain += to_string(temp->data) + " → "; temp=temp->next; }
		cout << chain.substr(0,chain.length()-4) << endl;
	}
	
	/*            ↓ insert
	 unshift → [chain] ← push
	   shift ← [chain] → pop
		          ↓ cut
	*/
		
	void push(int value) {
		node *temp=new node; temp->data=value; temp->next=NULL;
		if (head==NULL)	{	head=temp; tail=temp;	temp=NULL; }
		else { tail->next=temp; tail=temp;	} nodes++;
	}	
		
	void unshift(int value) { //at beginning
		node *temp=new node; temp->data=value; temp->next=head; head=temp; nodes++;
	}
	
	void insert(int pos, int value) { //at position
		node *pre=new node; node *cur=new node; node *temp=new node; cur=head;
		for(int i=1;i<pos;i++) { pre=cur; cur=cur->next; }
		temp->data=value; pre->next=temp; temp->next=cur; nodes++;
	}
	
	void shift() {		//delete first
		node *temp=new node; temp=head; head=head->next; delete temp; nodes--;
	}
	
	void pop() {		//delete last
		node *current=new node; node *previous=new node; current=head;
		while(current->next!=NULL)  { previous=current;	current=current->next; }
		tail=previous; previous->next=NULL; delete current; nodes--;
	}
	
	void cut(int pos) {	//delete @ position
		node *current=new node; node *previous=new node; current=head;
		for(int i=1;i<pos;i++) { previous=current; current=current->next; }
		previous->next=current->next; nodes--;
	}
};


int main() {
	list obj;
	obj.push(25);
	obj.push(50);
	obj.push(90);
	obj.push(40);
	cout<<"--------printing All nodes--------\n";
	obj.print();
	cout << "(" << to_string(obj.length()) << ")\n";
	cout<<"\n----------Inserting At End----------\n";
	obj.push(55);
	obj.print();
	cout << "(" << to_string(obj.length()) << ")\n";
	cout<<"\n---------Inserting At Start---------\n";
	obj.unshift(50);
	obj.print();
	cout << "(" << to_string(obj.length()) << ")\n";
	cout<<"\n---------Inserting At Particular-------\n";
	obj.insert(5,60);
	obj.print();
	cout << "(" << to_string(obj.length()) << ")\n";
	cout<<"\n---------Deleting At Start----------\n";
	obj.shift();
	obj.print();
	cout << "(" << to_string(obj.length()) << ")\n";
	cout<<"\n----------Deleing At End------------\n";
	obj.pop();
	obj.print();
	cout << "(" << to_string(obj.length()) << ")\n";
	cout<<"\n-------Deleting At Particular-------\n";
	obj.cut(2);
	obj.print();
	cout << "(" << to_string(obj.length()) << ")\n";
	return 0;
}