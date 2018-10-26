#include "ring.hpp"

uint16_t PIXELS;
uint8_t R, G, B, P;


void setup() {
  Serial.begin(9600);

  PIXELS = 300;
  randomSeed(analogRead(0));
  P = 12;
  R = random(256); G = random(256); B = random(256);
  Ring ring;

  // Test node bitfield getters & setters work as expected
  // These should be idempotent
  uint32_t n;
  uint16_t pos = random(PIXELS);
  uint8_t  hue = random(256);
  uint8_t  lum = random(256);

  Ring::hue(n, hue); Ring::lum(n, lum); Ring::pos(n, pos);

  if (Ring::hue(n) != hue) Serial.println("Node ops");
  if (Ring::lum(n) != lum) Serial.println("Node ops");
  if (Ring::pos(n) != pos) Serial.println("Node ops");

  ring.init(P);
  for (uint8_t i = 1; i < ring.nodes; i++) {
    uint16_t large = Ring::pos(ring.ring[i]);
    uint16_t small = Ring::pos(ring.ring[i - 1]);
    if (large < small) Serial.println("Sorting");
  }

  // Test resize stability (should not modify nodes in the list)
  String b4 = ring.to_string(), af;
  

  for (uint8_t _t = 0; _t < 10; _t++) {
    ring.populate(P);
    af = ring.to_string();
    if (b4 != af) {
      Serial.println("Stability");
      Serial.println(b4);
      Serial.println(af);
    }
  }

  ring.print();
  Serial.println("Done.");
}

void loop() {


}
