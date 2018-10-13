#define MAX_SPARKLES 180 //heap overflow n such

struct node {
  int8_t mag;
  uint16_t pos;
  node *next;
  node(int8_t mag, uint16_t pos, node *next) {
    this->mag = mag;
    this->pos = pos;
    this->next = next;
  }
  node() {
    this->mag = 0;
    this->pos = rand() % PIXELS;
    this->next = this;
  }
};

int8_t interMag(node* A, node* B, uint16_t pos) {
  uint16_t mag = abs(A->mag) << 8;
  uint16_t dist = B->pos - A->pos;
  if (!dist) return 0;
  uint16_t dFromA = pos - A->pos;
  int16_t mag_step = int16_t(abs(B->mag << 8) - mag) / dist;
  mag += mag_step * dFromA;
  mag >>= 8;
    return rand() & 1 ? -mag : mag;
//  return mag;
}


class list {
  private:
    node *head;
    node *tail;
    uint8_t nodes;

  public:
    list() {
      head = new node;
      tail = head;
      nodes = 0;
    }
    void populate (uint8_t sparkles) {
      while (nodes < sparkles) insert(rand() % PIXELS, rand());
    }
    void terminate(uint8_t sparkles) {
      while (nodes > sparkles) cut(head->pos);
    }

    void displayMono(int8_t mag) {
      uint8_t disp = abs(mag);
      if (disp < 128) disp <<= 1;
      else disp = 255;
      send2Pixels(
        (uint16_t(R * disp) + 1) >> 8,
        (uint16_t(G * disp) + 1) >> 8,
        (uint16_t(B * disp) + 1) >> 8
      );
    }

    void displayWhite() {
      
    }

    void displayDiscrete() {
      node *n = head;
      uint16_t p;
      for (p = 0; n->next != head && p < PIXELS;) {
        for (p; p < n->pos && p < PIXELS; p++) send2Pixels(0, 0, 0);
        displayMono(n->mag);
        p++;
        n = n->next;
      }
      for (p; p < PIXELS; p++) send2Pixels(0, 0, 0);
    }

    uint8_t printGrad(node* A, node* B) {
      uint16_t mag = abs(A->mag) << 8;
      uint16_t dist = B->pos - A->pos;
      if (!dist) return 0;
      int16_t mag_step = int16_t(abs(B->mag << 8) - mag) / dist;
      uint16_t p = 1;
//      displayMono(mag >> 8);
      send2Pixels(255,255,255);
      mag += mag_step;
      for (; p < dist; p++) {
        displayMono(mag >> 8);
        mag += mag_step;
      }
      return dist;
    }

    void displayContinuous() {
      node *n = head;
      head->pos += PIXELS;
      node origin(interMag(tail, head, PIXELS), 0, head);
      head->pos -= PIXELS;

      uint16_t p = 0;

      //    if (head->pos > 0) {
      n = &origin;
      p = printGrad(n, head);
      n = head;
      //    }

      for (; n->next != head;) {
        //      for (; p < n->pos; p++) send2Pixels(0, 0, 0);
        p += printGrad(n, n->next);
        n = n->next;
      }

      //    if (tail->pos < PIXELS) {
      origin.pos = PIXELS;
      n = &origin;
      p = printGrad(tail, n);
      //    }
    }

    bool insert(uint16_t pos, int8_t mag) {
      nodes++;
      node *cur = head;
      if (pos < head->pos) {
        head = new node(mag, pos, head);
        tail->next = head;
        return true;
      }
      do {
        if (cur->next->pos == pos) {
          nodes--;
          return false;
        }
        if (cur->next->pos >  pos) {
          cur->next = new node(mag, pos, cur->next);
          return true;
        }
        cur = cur->next;
      }
      while (cur->next != head);
      cur->next = new node(mag, pos, cur->next);
      tail = cur->next;
      return true;
    }

    bool insertAlive(uint16_t pos) {
      if (pos > PIXELS || pos == head->pos) return false;
      nodes++;
      node *cur = head;
      if (pos < head->pos) {

        head = new node(interMag(cur, cur->next, pos), pos, head);

        tail->next = head;
        return true;
      }
#define WIDTH 3
      do {
        if (cur->next->pos > pos ) {
          if (cur->next->pos - pos < WIDTH) {
            nodes--;
            return false;
          }
          if (pos - cur->pos < WIDTH) {
            nodes--;
            return false;
          }
          cur->next = new node(interMag(cur, cur->next, pos), pos, cur->next);
          return true;
        }
        if (cur->next->pos == pos) {
          nodes--;
          return false;
        }
        cur = cur->next;
      }
      while (cur->next != head);
      cur->next = new node(interMag(cur, cur->next, pos), pos, cur->next);
      tail = cur->next;
      return true;
    }

    bool cut(uint16_t pos) {  //delete @ position
      if (pos > PIXELS || pos < head->pos) return false;
      node *cur = head;
      if (pos == head->pos) {
        nodes--;
        node* n = head->next;
        delete head;
        head = n;
        tail->next = head;
        return true;
      }
      do {
        if (cur->next->pos >  pos) return false;
        if (cur->next->pos == pos) {
          node *n = cur->next->next;
          delete cur->next;
          cur->next = n;
          nodes--;
          if (cur->next == head) tail = cur;
          return true;
        }
        cur = cur->next;
      }
      while (cur->next != head);
      return false;
    }

    void updateDiscrete() {
      node *cur = head;
      do {
        int8_t newmag = cur->mag + 1;

        if (cur->mag < 0 && newmag > -1) {
          if ((rand() % 5) != 4) {
            cur->mag = -1;
            cur = cur->next;
            continue;
          }
          uint16_t del_pos = cur->pos;
          cur = cur->next;
          cut(del_pos);
          bool reinsert;
          do {
            reinsert = !insert((rand() % PIXELS), 0);
          }
          while (reinsert);
          continue;
        }
        cur->mag = newmag;
        cur = cur->next;
      }
      while (cur->next != head);
    }

    void updateContinuous() {
      node *pre = tail;
      node *cur = head;
      do {
        int8_t newmag = cur->mag + 1;

        if (!(abs(newmag) - abs(interMag(pre, cur->next, cur->pos)))) {
          uint16_t del_pos = cur->pos;
          cur = cur->next;
          cut(del_pos);
          bool reinsert;
          do {
            del_pos = rand() % PIXELS;
            reinsert = !insertAlive(del_pos);
          }
          while (reinsert);
          continue;
        }
        cur->mag = newmag;
        pre = cur;
        cur = cur->next;
      }
      while (cur->next != head);
    }

};

list sparkles;

void sparkle(void) {
  sparkles.updateDiscrete();
  sparkles.displayDiscrete();
  delayMicroseconds(MAX_SPARKLES - P);
}

void gradient(void) {
  sparkles.updateContinuous();
  sparkles.displayContinuous();
  delayMicroseconds(MAX_SPARKLES - P);
}




