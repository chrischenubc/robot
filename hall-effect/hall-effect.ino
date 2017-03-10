// read RPM
 int half_revolutions = 0;
 int rps = 0;
 unsigned long lastmillis = 0;
String pole;
const int E1 = 5;
const int M1 = 4;
const int E2 = 6;
const int M2 = 7;
 
 void setup(){
 Serial.begin(9600); 
 attachInterrupt(0, rpm_fan, FALLING);
 }
 void loop(){
   moveForward(90);
   if(digitalRead(2)==HIGH){
    pole="north";

  }
   if(digitalRead(2)==LOW){
    pole="SOUTH";
  }
 if (millis() - lastmillis == 1000){ //Uptade every one second, this will be equal to reading frecuency (Hz).
 detachInterrupt(0);//Disable interrupt when calculating
 rps = half_revolutions ; // Convert frecuency to RPM, note: this works for one interruption per full rotation. For two interrups per full rotation use half_revolutions * 30.
 Serial.print("RPM =\t"); //print the word "RPM" and tab.
 Serial.print(rps); // print the rpm value.
 Serial.print("\t Hz=\t"); //print the word "Hz".
 Serial.print(half_revolutions); //print revolutions per second or Hz. And print new line or enter.
 Serial.print("\t Pole:\t"); //print the word "Hz".
 Serial.println(pole);
 half_revolutions = 0; // Restart the RPM counter
 lastmillis = millis(); // Uptade lasmillis
 attachInterrupt(digitalPinToInterrupt(2), rpm_fan, FALLING); //enable interrupt
  }

  
 }
 // this code will be executed every time the interrupt 0 (pin2) gets low.
 void rpm_fan(){
  half_revolutions++;
 }

 void moveForward(long power) {
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, power);
  analogWrite(E2, power);
}

