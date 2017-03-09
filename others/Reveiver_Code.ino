/*
 The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 1K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystalDisplay

 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

String readStr="";
void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  Serial.begin(9600);
}

void loop() {
  // when characters arrive over the serial port...
  if (Serial.available()) {
    delay(10);
    lcd.clear();
    int flag=Serial.parseInt();
    int value;
    if(flag>=1001 && flag<=1003){
    switch(flag){
      case 1001:
        value=Serial.parseInt();
        if(value>=10 && value<=30){
          lcd.print("Temp: ");
          lcd.print(value);
          delay(900);
        }
        lcd.clear();
      case 1002: 
        value=Serial.parseInt();
        if(value>=0 && value<=20){
          lcd.print("Speed: ");
          lcd.print(value);
          delay(900);
        }
        lcd.clear();
      case 1003:
        value=Serial.parseInt();
        if(value>=0 && value<=200){
          lcd.print("Distance: ");
          lcd.print(value);
          delay(900);
        }
        lcd.clear();
      default: 
        lcd.print("No data");
      }
    }
  }
}

