/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
 
 volatile int half_revolutions = 0;
 double rps= 0;
unsigned long long lastmillis = 0;
 String pole;
const int E1 = 5;
const int M1 = 4;
const int E2 = 6;
const int M2 = 7;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(2,INPUT_PULLUP);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), rpm_fan, FALLING);
}

// the loop function runs over and over again forever
void loop() {
  moveForward(255);
  //Serial.println(half_revolutions);
  if(digitalRead(2)==HIGH){
    pole="North";
  }
  else if(digitalRead(2)==LOW){
    pole="SOUTH";
  }
  delay
 /*if (half_revolutions==40){ //Uptade every one second, this will be equal to reading frecuency (Hz).
  
//  detachInterrupt(digitalPinToInterrupt(2));//Disable interrupt when calculating
 // int arc_time = half_revolutions*1000/(millis() - lastmillis) ; // Convert frecuency to RPM, note: this works for one interruption per full rotation. For two interrups per full rotation use half_revolutions * 30.
  //rps = arc_time * 5;
 
 rps = 8.0*1000/(millis() - lastmillis) ; // Convert frecuency to RPM, note: this works for one interruption per full rotation. For two interrups per full rotation use half_revolutions * 30.
 
 Serial.print("RPs =\t"); //print the word "RPM" and tab.
 Serial.println(rps); // print the rpm value.

 half_revolutions = 0; // Restart the RPS counter
 lastmillis = millis(); // Uptade lasmillis
}*/

}
void moveForward(long power) {
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, power);
  analogWrite(E2, power);
}

void rpm_fan(){
  half_revolutions++;
 }

