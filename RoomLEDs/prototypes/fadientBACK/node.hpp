#include <iostream>
#include <string>

uint16_t PIXELS; //Total number of leds
uint8_t R, G, B;

struct node {
    uint16_t pos;
    node *next;
    node() {
        this->pos=rand()%PIXELS;
        this->next=this;
    }
};

struct monode : public node {
    int8_t mag;
    node(int8_t mag, uint16_t pos, node* next) { this->mag=mag; this->pos=pos; this->next=next; }
    node() { this->mag=0; this->pos=rand()%PIXELS; this->next=this; }
};

struct rgbnode : public node {
    int8_t rgb[3];
    node() {
        this->pos=rand()%PIXELS;
        this->next=this;
        
                
        
    }
};

