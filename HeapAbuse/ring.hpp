extern uint16_t PIXELS;
extern uint8_t R, G, B, P;

#define ZERO_SYMBOL " "
#define GRADIENT_WIDTH 3

/*  32-bit node protohue:
  |Position.........| hue   |  Lum   |
  |######## ########|########|########| */

struct Ring {
    uint32_t* ring; uint8_t nodes;
    
    Ring() { nodes = 0; }
    
    //node ops [
        static uint16_t pos(uint32_t n) { return  (n &  0xffff0000) >> 16; }
        static uint16_t hue(uint32_t n) { return  (n &  0x0000ff00) >> 8;  }
        static uint16_t lum(uint32_t n) { return   n &  0x000000ff;        }
        static void pos(uint32_t& n, uint16_t p) { n &= 0x0000ffff; n |= (uint32_t)p << 16; }
        static void hue(uint32_t& n, uint8_t  l) { n &= 0xffff00ff; n |= (uint32_t)l << 8;  }
        static void lum(uint32_t& n, uint8_t  h) { n &= 0xffffff00; n |= h;                 }
    //]
    
    void sort() { // Insertion sort
        uint8_t i, j; uint32_t k; 
        for (i = 1; i < nodes; i++) { 
            j = i-1; k = ring[i];
            while (j >= 0 && pos(ring[j]) > pos(k)) { ring[j+1] = ring[j]; j--; } 
            ring[j+1] = k; 
        } 
    }

     void init (uint8_t desiredNodes) { 
        ring = new uint32_t[desiredNodes];
        if (desiredNodes > nodes) for (uint8_t i=nodes; i < desiredNodes; i++) ring[i]=initNode();
        nodes = desiredNodes;
        sort();
    }
        
    void populate (uint8_t desiredNodes) { 
        uint32_t n0=ring[0];
        delete[]ring;
        ring = new uint32_t[desiredNodes];
        if (desiredNodes > nodes) for (uint8_t i=nodes; i < desiredNodes; i++) ring[i]=initNode();
        nodes = desiredNodes;
        ring[0]=n0;
        sort();
    }
    
    uint32_t initNode() {
        uint32_t n;
        n =  random(PIXELS) << 16;
        n += random(256) << 8;
        n += random(256);
        // pos(n, rand()%PIXELS);
        // hue(n, rand());
        // lum(n, rand());
//        Serial.print("init: ");
//        printNode(n);
        return n;
    }
    
    void printNode(uint32_t n) { 
        Serial.print(lum(n));
        Serial.print(",");
        Serial.print(hue(n));
        Serial.print("@");
        Serial.print(pos(n));
        Serial.print(":");
        Serial.println(n);
    }
    // void printNode(uint32_t n) { Serial.println("[%03d,%03d@%03d]", lum(n),hue(n),pos(n)); }
    
    void print() {
        for (uint8_t i=0; i < nodes; i++) printNode(ring[i]);
        Serial.println("\n");
    }
    
    String to_string() {
        String s;
        for (uint8_t i=0; i < nodes; i++) {
            s += "[";
            s += (lum(ring[i]));
            s += ",";
            s += (hue(ring[i]));
            s += "@";
            s += (pos(ring[i]));
//            s += ":";
//            s += (ring[i]);
            s += "]";
        }
        return s;
        
    }
    
    void displayNode(uint32_t n) {}
    void display() {}
};
