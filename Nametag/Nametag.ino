#include <LiquidCrystal.h>
#include "chars.h"

//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//                  RS, E, D4, D5, D6, D7
//VSS→GND, VDD→5, V0→knob or pwm, RW→GND 

LiquidCrystal lcd(2, 3, 4, 5, 6, 7, 8);
const uint16_t DELAY = 2500;

void setup() {
  pinMode(9, OUTPUT);
  pinMode(10, INPUT);
  digitalWrite(9, HIGH);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Michael Bartlett");
}

void loop() {
  lcd.setCursor(0, 1);
  
  String major = "Undergrad Junior";
  for (int i=0; i<major.length(); i++) {
  	lcd.print(major.substring(i,i+1));	
  	delay(50);
  }

if (buttonWait(DELAY)) return;
  
  lcd.setCursor(0, 1);
  major = "Major: Comp Sci ";
  for (int i=0; i<major.length(); i++) {
  	lcd.print(major.substring(i,i+1));	
  	delay(50);
  }

if (buttonWait(DELAY)) return;

  major = " GPA:  3.83";
  lcd.setCursor(5,1);
  for (int i=0; i<major.length(); i++) {
  	lcd.print(major.substring(i,i+1));	
  	delay(50);
  }

if (buttonWait(DELAY)) return;

  lcd.setCursor(0, 1);
  major = "Cumulative: 3.25";
  for (int i=0; i<major.length(); i++) {
  	lcd.print(major.substring(i,i+1));	
  	delay(50);
  }
  
  if (buttonWait(DELAY)) return;
  
}

bool buttonWait(int _delay) {
	unsigned long int wait = millis();
	while (millis() - wait < _delay) {
  		if (!digitalRead(10)) {
  			clearPac(0);
  			drawM(17);
  			lcd.setCursor(24,0);
  			lcd.print("School");
  			lcd.setCursor(23,1);
  			lcd.print("of Mines");
  			for (int i=0;i<16;i++) { lcd.scrollDisplayLeft(); delay(DELAY/30); }
  			delay(DELAY);
  			lcd.clear();
  			drawM(1);
  			lcd.setCursor(8,0);
  			lcd.print("School");
  			lcd.setCursor(7,1);
  			lcd.print("of Mines");
  			lcd.setCursor(16, 0);
			lcd.print("Michael Bartlett");
  			for (int i=0;i<16;i++) { lcd.scrollDisplayLeft(); delay(DELAY/30); }
  			lcd.clear();
  			lcd.print("Michael Bartlett");
  			return true;
  		}
	}
	return false;
}

void drawM(uint8_t start) {
  for (int i=0; i<8; i++) lcd.createChar(i, M[i]);
  lcd.setCursor(start,0);
  for (uint8_t i=0; i<8; i++) {
  	if (i == 4) lcd.setCursor(start,1);
  	lcd.write(i);
  }
}

void clearPac(int8_t offset) {
	int delay1 = 75;
	for (int i=0; i<8; i++) lcd.createChar(i, Pac[i]);
	lcd.setCursor(offset,0);
	lcd.write(4);
	lcd.setCursor(offset,1);
	lcd.write(5);
	delay(delay1);
	lcd.setCursor(offset,0);
	lcd.write(2);
	lcd.setCursor(offset,1);
	lcd.write(3);
	delay(delay1);
	lcd.setCursor(offset,0);
	lcd.write(4);
	lcd.setCursor(offset,1);
	lcd.write(5);
	delay(delay1);
	lcd.setCursor(offset,0);
	lcd.write(6);
	lcd.setCursor(offset,1);
	lcd.write(7);
	delay(delay1);
	for (int i=-1+offset; i<16+offset; i++) {
		lcd.setCursor(i,0);
		lcd.print(' ');
		lcd.write(uint8_t(0));
		lcd.write(4);
		lcd.setCursor(i,1);
		lcd.print(' ');
		lcd.write(1);
		lcd.write(5);
		delay(delay1);
		if (i > 13+offset) continue;
		lcd.setCursor(i+2,0);
		lcd.write(2);
		lcd.setCursor(i+2,1);
		lcd.write(3);
		delay(delay1);
		lcd.setCursor(i+2,0);
		lcd.write(4);
		lcd.setCursor(i+2,1);
		lcd.write(5);
		delay(delay1);
		lcd.setCursor(i+2,0);
		lcd.write(6);
		lcd.setCursor(i+2,1);
		lcd.write(7);
		delay(delay1);
	}
	lcd.clear();
}


//  for (int i=major.length()-1; i>-1; i--) {
//  	lcd.setCursor(i, 1);
//  	lcd.print(" ");
//  	delay(50);
//  }
