// read RPM
 volatile int half_revolutions = 0;
 int rpm = 0;
 unsigned long lastmillis = 0;
 String pole;
const int E1 = 5;
const int M1 = 4;
const int E2 = 6;
const int M2 = 7;
 
 unsigned long timeold;
 void setup(){
 Serial.begin(9600); 
 
 attachInterrupt(0, rpm_fan, FALLING);


   half_revolutions = 0;
   rpm = 0;
   timeold = 0;
 }
 void loop(){
   moveForward(70);
   if(digitalRead(2)==HIGH){
    pole="North";

  }
   if(digitalRead(2)==LOW){
    pole="SOUTH";
  }
 if (half_revolutions >= 20) { 
     //Update RPM every 20 counts, increase this for better RPM resolution,
     //decrease for faster update
     rpm = 30*1000/(millis() - timeold)*half_revolutions;
    
  /*   
  Serial.print("RPM =\t"); //print the word "RPM" and tab.
  Serial.print(rpm); // print the rpm value.
  Serial.print("\t Hz=\t"); //print the word "Hz".
  Serial.print(half_revolutions/2); //print revolutions per second or Hz. And print new line or enter
  Serial.print("\t Pole:=\t");
  Serial.println(pole);
  */
   
   timeold = millis();
     half_revolutions = 0;
   }
  
 }
 // this code will be executed every time the interrupt 0 (pin2) gets low.
 void rpm_fan(){
  half_revolutions++;
  Serial.println(half_revolutions);

 }

 void moveForward(long power) {
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, power);
  analogWrite(E2, power);
}

