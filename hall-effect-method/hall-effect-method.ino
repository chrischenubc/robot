// read RPM
 volatile int half_revolutions = 0;
 int rps = 0;
 unsigned long lastmillis = 0;
 String pole;
const int E1 = 5;
const int M1 = 4;
const int E2 = 6;
const int M2 = 7;
 
 unsigned long timeold;
 void setup(){
 Serial.begin(9600); 
 pinMode(2,INPUT_PULLUP);
 attachInterrupt(digitalPinToInterrupt(2), rpm_fan, FALLING);


   half_revolutions = 0;
   rps = 0;
   timeold = 0;
 }
 void loop(){
   moveForward(255);
   if(digitalRead(2)==HIGH){
    pole="North";

  }
   if(digitalRead(2)==LOW){
    pole="SOUTH";
  }
 if (half_revolutions >= 2) { 
     //Update RPM every 20 counts, increase this for better RPM resolution,
     //decrease for faster update
    // rpm = 30*1000/(millis() - timeold)*half_revolutions;
    double arc_time;
    arc_time=half_revolutions/(millis()-timeold)*1000;
    rps=arc_time*5;
     
    Serial.print("RPs =\t"); //print the word "RPM" and tab.
    Serial.print(rps); // print the rpm value.
   
   
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
