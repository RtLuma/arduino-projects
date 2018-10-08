#include <iostream>
#include <string>

uint8_t rainbow(uint8_t hue) { if (hue > 170) return ~(hue + (hue << 1)); if (hue > 85) return hue + (hue << 1); return 0; }

extern uint8_t R, G, B;

struct node {
    uint16_t pos;   node *next;
    node() {
        this->pos=0;
        this->next=this;
    }

    virtual void print() = 0;
};

struct monode : public node {
    int8_t mag;

    monode(int8_t mag, uint16_t pos, node* next) {
        this->mag=mag;
        this->pos=pos;
        this->next=next;
    }

    monode() {
        this->mag=0;
        this->pos=0;
        this->next=this;
    }

     void print() {
        uint8_t disp = abs(mag);
        if (disp < 128) disp <<= 1;
        else disp = 255;
        printf("\033[38;2;%d;%d;%dm█\033[0;m", (uint16_t(R * disp) + 1) >> 8, (uint16_t(G * disp) + 1) >> 8, (uint16_t(B * disp) + 1) >> 8);
    } //]]
};

struct rgbnode : public node {
    uint8_t hue;    // target  RGB
    uint8_t rgb[3]; // current rgb

    rgbnode(uint8_t hue, uint16_t pos, node* next) {
        this->hue=hue;
        this->pos=pos;
        this->next=next;
    }

    rgbnode() {
        this->hue=0;
        this->pos=0;
        this->next=this;
    }

    void print() {
        printf("\033[38;2;%d;%d;%dm█\033[0;m", rgb[0], rgb[1], rgb[2]);
    } //]]
};

