#define SPARKLES 75

struct node {
  int8_t mag;
  uint16_t pos;
  node *next;
};

class list {
  private:
    node *head, *tail;
    uint8_t nodes;
  public:
    list() {
      head = nullptr; tail = nullptr; nodes = 0;
      while (nodes < SPARKLES) {
        uint8_t m = random(256);
        uint16_t p = random(SCLERA);
        insert(p, m);
      }
    }

    uint8_t length(void) {
      return nodes;
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
          else {
            head = cur->next;
          }
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

        int8_t newmag = cur->mag + 1 + (F>>3);
        
        if (cur->mag<0 && newmag>-1) {
          uint16_t del_pos = cur->pos;
          cur = cur->next;
          cut(del_pos);
          bool reinsert;
          do { reinsert = !insert(random(SCLERA), 0); } while (reinsert);
          continue;
        }        
        cur->mag = newmag;
        cur = cur->next;
      }
    }

#define SPARKLEBLOCK(PIXELFUNC, LENGTH, MAP_FROM, MAP_WRAP)\
    temp=head;\
    for (p = 0; temp != nullptr && p < LENGTH;) {\
        for (p; p < temp->pos && p < LENGTH; p++) PIXELFUNC(0, 0, 0);\
        uint8_t disp = abs(temp->mag); if (disp < 128) disp <<= 1; else disp = 255;\
        uint8_t h = map(MAP_FROM, 0, MAP_WRAP, 0, 255) + (millis() >> 6);\
        PIXELFUNC(\
          (uint16_t(rainbow(h)       * disp) + 1) >> 8,\
          (uint16_t(rainbow(h - 85)  * disp) + 1) >> 8,\
          (uint16_t(rainbow(h - 170) * disp) + 1) >> 8\
        ); p++; temp = temp->next;\
      } for (p; p < LENGTH; p++) PIXELFUNC(0, 0, 0);\

    void display() {
      node *temp;
      uint16_t p;
      SPARKLEBLOCK(sendPixel,  SCLERA,  p,      SCLERA)
      SPARKLEBLOCK(sendPixel2, PIXELS2, p%IRIS, (p > IRIS ? PUPIL : IRIS))
    }

    void displayRGB() {
      node *temp=head;
      uint16_t p;
      for (p = 0; temp != nullptr && p < PIXELS;) {
        for (p; p < temp->pos && p < PIXELS; p++) send2Pixels(0, 0, 0);
        uint8_t disp = abs(temp->mag); if (disp < 128) disp <<= 1; else disp = 255;
        send2Pixels(
          (uint16_t(R * disp) + 1) >> 8,
          (uint16_t(G * disp) + 1) >> 8,
          (uint16_t(B * disp) + 1) >> 8
        ); p++; temp = temp->next;
      } for (p; p < PIXELS; p++) send2Pixels(0, 0, 0);
    }
};

list sparkles;

void Sparkle(void) {
  sparkles.update();
  sparkles.display();
  delayMicroseconds(SCLERA - SPARKLES);
}

