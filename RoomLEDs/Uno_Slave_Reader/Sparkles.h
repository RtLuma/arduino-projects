#define SPARKLES 100

struct node {
  uint8_t mag;
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
        uint16_t p = random(SCLERA) | (random(2) ? 0x8000 : 0);
        insert(p, m);
      }
    }

    uint8_t length(void) {
      return nodes;
    }

    void display() {
      node *temp = head;
      uint16_t p;
      for (p = 0; temp != nullptr;) {
        for (p; p < (0x7fff & temp->pos); p++) send2Pixels(0, 0, 0);
        send2Pixels(temp->mag, temp->mag, temp->mag); p++;
        //        send2Pixels(
        //          (uint16_t(255 * (temp->mag)) + 1) >> 8,
        //          (uint16_t(255 * (temp->mag)) + 1) >> 8,
        //          (uint16_t(255 * (temp->mag)) + 1) >> 8
        //        );
        temp = temp->next;
      }
      for (p; p < SCLERA; p++) send2Pixels(0, 0, 0);
    }

    bool insert(uint16_t pos, uint8_t mag) { //at position
      pos &= 0x7fff;
      node *pre;
      node *cur = head;
      node *temp = new node; temp->mag = mag; temp->pos = pos; temp->next = nullptr;
      nodes++;
      if (head == nullptr)  {
        head = temp;
        tail = temp;
        temp = nullptr;
        delete temp;
        return true;
      }
      if ((0x7fff & head->pos) > pos) {
        temp->next = head;
        head = temp;
        temp = nullptr;
        delete temp;
        return true;
      }
      while (cur->next != nullptr) {
        pre = cur;
        cur = cur->next;
        if ((0x7fff & pre->pos) == pos || (0x7fff & cur->pos) == pos) {
          delete temp;
          nodes--;
          return false;
        }
        if ((0x7fff & cur->pos) > pos) {
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
        uint16_t actual_pos = cur->pos & 0x7fff;
        if (actual_pos == pos) {
          if (pre != nullptr) pre->next = cur->next;
          else {
            head = cur->next;
          }
          delete cur;
          nodes--;
          return true;
        }
        if (actual_pos > pos) return false;
        pre = cur;
        cur = cur->next;
      }
      return false;
    }

    void update() {
      node *cur = head;
      while (cur != nullptr) {
        if (cur->pos & 0x8000) {
          if (!--(cur->mag)) {
            if (random(5) != 4) {
              cur->mag++;
              cur=cur->next;
              continue;
            }
            uint16_t del_pos = (cur->pos & 0x7fff);
            cur = cur->next;            
            cut(del_pos);
            bool reinsert;
            do {
              reinsert = !insert(random(SCLERA), random(16));
            } while (reinsert);
            continue;
          }
        }
        else if (!++(cur->mag)) {
          cur->mag = 255;
          cur->pos |= 0x8000;
        }
        cur = cur->next;
      }
    }
};

list sparkles;

void Sparkle(void) {
  sparkles.update();
  sparkles.display();
//  delayMicroseconds(20);
}

