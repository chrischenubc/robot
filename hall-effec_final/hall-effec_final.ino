// read RPM
 volatile int half_revolutions = 0;
 int rpm = 0;
 unsigned long lastmillis = 0;
double interval;



const int HALL_EFFECT_PIN=2;
 
 String pole;
const int E1 = 5;
const int M1 = 4;
const int E2 = 6;
const int M2 = 7;
 volatile int record_time=0;
 unsigned long timeold;
 void setup(){
 Serial.begin(9600); 
 pinMode(2,INPUT_PULLUP);
 attachInterrupt(digitalPinToInterrupt(2), rpm_fan, FALLING);

   half_revolutions = 0;
   rpm = 0;
   timeold = 0;
 }
 void loop(){
if (record_time==0) { 
      timeold=millis();
 }
 else if(record_time==1){
      interval=millis-timeold;
 }
  
   moveForward(255);
   if(digitalRead(2)==HIGH){
    pole="North";

  }
   if(digitalRead(2)==LOW){
    pole="SOUTH";
  }
 
     //Update RPM every 20 counts, increase this for better RPM resolution,
     //decrease for faster update
     rpm = 30*1000/(millis() - timeold)*half_revolutions;
    
     
    Serial.print("RPM =\t"); //print the word "RPM" and tab.
    Serial.print(rpm); // print the rpm value.
    Serial.print("\t Hz=\t"); //print the word "Hz".
    Serial.print(half_revolutions/2); //print revolutions per second or Hz. And print new line or enter
    Serial.print("\t Pole:=\t");
    Serial.println(pole);
    
   
   timeold = millis();
     half_revolutions = 0;
   }
  
 }
 // this code will be executed every time the interrupt 0 (pin2) gets low.
 void rpm_fan(){
  //half_revolutions++;
  if(record_time==0){
    record_time=1;
  }
  else if(record_time==1){
    record_time=0;
  }
  Serial.println(half_revolutions);

 }


void hall_effect(){
  while(digitalRead(HALL_EFFECT_PIN)==
  
  
}

 void moveForward(long power) {
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, power);
  analogWrite(E2, power);
}
