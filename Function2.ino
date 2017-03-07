#include<Servo.h>
Servo servo;                    //Create an object myServo

//robot motor pin configuration
//Arduino PWM Speed Control: E1 is right motor; E2 is left motor
const int E1 = 5;
const int M1 = 4;
const int E2 = 6;
const int M2 = 7;

//sonar sensor pin configuration
const int TRIG_PIN = 12;
const int ECHO_PIN = 11;
const int SERVO_PIN = 13;

//shift register pin configuration
const int latchPin = 8;
const int clockPin = 9;
const int dataPin = 10;

const int LM_READ_PIN = A0;      //Pin for LM-35 temperature sensor
const int SWITCH_PIN = 3;        //Pin for pull up Switch

//Pins for 2 Hall effect sensors
//const int lHEsensorPin = A1;
//const int rHEsensorPin = A2;

/**************************************************************/
/**************************************************************/
//Pins for 2 optical reflective sensors
const int ORsensorpin_1 = A1;
const int ORsensorpin_2 = A2;
const int ORsensorpin_3 = A3;
const int ORsensorpin_4 = A4;

//define some constants for basic function 1 here
const int DISTANCE_LIMIT = 25;
const int MAX_SPEED = 255;

const int DECELERATION = 10;
int flag=1;

//Declaration of variables for basic function 2
const int LINE_FOLLOW_SPEED = 190;
const int TURNING_SPEED_NORM = 120;
const int TURNING_SPEED_DIFF = 80;
const int TURNING_SPEED_MAX = 225;

int midVal = 0;
int minSensorVal = 1023;
int maxSensorVal = 0;

int TempORsensorVal_2;
int ORsensorVal_1;
int ORsensorVal_2;
int ORsensorVal_3;
int ORsensorVal_4;

void setup(){
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SERVO_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);

  //Declaration of input pins for 5 optical reflective sensors
  for(int i = 14; i < 20; i++){
    pinMode(i, INPUT);
  }

  lineFollowPrepare();
  delay(2000);
  
  Serial.begin(9600);
}

void loop(){
 //lineFollowPrepare();
 //delay(2000);

 lineFollow();
}

void moveForward(long power) {
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, power);
  analogWrite(E2, power);

}


/**
 * make the robot stop immidietly
 */
void Stop() {
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, 0);
  analogWrite(E2, 0);
  Serial.println("In Stop");
}

/*TODO: We need several testing to find a reasonable connection between 
 * level and DISTANCE_LIMIT to make it approach the object as much as possible
 * right now, just use arbitary level and DISTANCE_LIMIT
 */

 
void turnLeft(int power, long Time) {
  unsigned long oldtime;
  oldtime = millis();

  while (millis() - oldtime <= Time) {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
    analogWrite(E1, power);
    analogWrite(E2, power);
  }
}

void turnRight(int power, long Time) {
  unsigned long oldtime;
  oldtime = millis();

  while (millis() - oldtime <= Time) {
    digitalWrite(M1, LOW);
    digitalWrite(M2, HIGH);
    analogWrite(E1, power);
    analogWrite(E2, power);
  }
}

/***************************************************************************************************/
/*The functions below are for basic function 2; In this case, 5 optical reflective sensors are used*/
/***************************************************************************************************/

//Additional moving functions for basic function 2
void turnLeftForward_Slow(int power, long Time) {
  unsigned long oldtime;
  oldtime = millis();

  while (millis() - oldtime <= Time) {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, power);
    analogWrite(E2, power - TURNING_SPEED_DIFF);

    Serial.println("Left");

  scanGround();

  //If the right sensors(1, 2) start to give higher values, then it means the car is moving too left,
  //so turnleft function should break and stop the robot from moving left
    if( (ORsensorVal_1 + ORsensorVal_2)/2 >= ((ORsensorVal_3 + ORsensorVal_4)/2 ) &&
        !(ORsensorVal_1 < midVal - 50 &&  ORsensorVal_2 < midVal - 50 &&
          ORsensorVal_3 < midVal - 50  && ORsensorVal_4 < midVal - 50 ) ){
      break;
    }
  }
}

void turnLeftForward_Fast(int power, long Time) {
  unsigned long oldtime;
  oldtime = millis();

  while (millis() - oldtime <= Time) {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, power);
    analogWrite(E2, 0);

    Serial.println("FasterLeft");

  scanGround();
    
  //If the right sensors(1, 2) start to give higher values, then it means the car is moving too left,
  //so turnleft function should break and stop the robot from moving left
    if( (ORsensorVal_1 + ORsensorVal_2)/2 >= ((ORsensorVal_3 + ORsensorVal_4)/2 ) && 
        !(ORsensorVal_1 < midVal - 50 &&  ORsensorVal_2 < midVal - 50 &&
          ORsensorVal_3 < midVal - 50  && ORsensorVal_4 < midVal - 50 ) ){
      break;
    }
  }
}

void turnRightForward_Slow(int power, long Time) {
  unsigned long oldtime;
  oldtime = millis();

  while (millis() - oldtime <= Time) {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, power - TURNING_SPEED_DIFF);
    analogWrite(E2, power);

  scanGround();

  //If the left sensors(3, 4) start to give higher values, then it means the car is moving too right,
  //so turnright function should break and stop the robot from moving right
    if( (ORsensorVal_1 + ORsensorVal_2)/2 <= ((ORsensorVal_3 + ORsensorVal_4)/2) && 
         !(ORsensorVal_1 < midVal - 50 &&  ORsensorVal_2 < midVal - 50 &&
          ORsensorVal_3 < midVal - 50  && ORsensorVal_4 < midVal - 50 ) ){
      break;
    }
  }
}

void turnRightForward_Fast(int power, long Time) {
  unsigned long oldtime;
  oldtime = millis();

  while (millis() - oldtime <= Time) {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, 0);
    analogWrite(E2, power);

  scanGround();

  //If the left sensors(3, 4) start to give higher values, then it means the car is moving too right,
  //so turnright function should break and stop the robot from moving right
    if( (ORsensorVal_1 + ORsensorVal_2)/2 <= ((ORsensorVal_3 + ORsensorVal_4)/2) && 
         !(ORsensorVal_1 < midVal - 50 &&  ORsensorVal_2 < midVal - 50 &&
          ORsensorVal_3 < midVal - 50  && ORsensorVal_4 < midVal - 50 ) ){
      break;
    }
  }
}

void sharpTurnLeft(int power, long Time) {
  unsigned long oldtime;
  oldtime = millis();

  while (millis() - oldtime <= Time) {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
    analogWrite(E1, power);
    analogWrite(E2, power);

  scanGround();

  //If the right sensors(1, 2) start to give higher values, then it means the car is moving too left,
  //so turnright function should break and stop the robot from moving left
    if( (ORsensorVal_1 + ORsensorVal_2)/2 >= ((ORsensorVal_3 + ORsensorVal_4)/2) && 
         !(ORsensorVal_1 < midVal - 50 &&  ORsensorVal_2 < midVal - 50 &&
          ORsensorVal_3 < midVal - 50  && ORsensorVal_4 < midVal + 50 ) ){
      break;
    }
  }
}

void sharpTurnRight(int power, long Time) {
  unsigned long oldtime;
  oldtime = millis();

  while (millis() - oldtime <= Time) {
    digitalWrite(M1, LOW);
    digitalWrite(M2, HIGH);
    analogWrite(E1, power);
    analogWrite(E2, power);
    
  scanGround();

  //If the left sensors(1, 2) start to give higher values, then it means the car is moving too right,
  //so turnright function should break and stop the robot from moving right
    if( (ORsensorVal_1 + ORsensorVal_2)/2 >= ((ORsensorVal_3 + ORsensorVal_4)/2) && 
         !(ORsensorVal_1 < midVal - 50 &&  ORsensorVal_2 < midVal - 50 &&
          ORsensorVal_3 < midVal - 50  && ORsensorVal_4 < midVal - 50 ) ){
      break;
    }
  }
}

boolean checkBreakpoint(long Time) {
  unsigned long oldtime;
  oldtime = millis();

  while(millis() - oldtime < Time){
    moveForward(LINE_FOLLOW_SPEED);

    scanGround();
  
    if(!(ORsensorVal_1 < midVal - 50 &&  ORsensorVal_2 < midVal - 50 &&
     ORsensorVal_3 < midVal - 50  && ORsensorVal_4 < midVal - 50) ){
      return true;
    }
  }

  return false;
}

char lineFollow(){
  int turningTime = 1000;

  scanGround();
  /*Serial.print(midVal);
  Serial.println("wtf");

  Serial.println(ORsensorVal_1);
  Serial.println(ORsensorVal_2);
  Serial.println(ORsensorVal_3);
  Serial.println(ORsensorVal_4);*/

  //delay(500);

  //If all the values are lower than midVal, it means the robot is on the surface
  if(!(ORsensorVal_1 < midVal - 50 &&  ORsensorVal_2 < midVal - 50 &&
     ORsensorVal_3 < midVal - 50  && ORsensorVal_4 < midVal - 50 )){

    //If the difference between left and right side sensors are lower than 200, go forward
    if( ((ORsensorVal_1 + ORsensorVal_2)/2 <= ((ORsensorVal_3 + ORsensorVal_4)/2 + 200)) &&
        ((ORsensorVal_1 + ORsensorVal_2)/2 >= ((ORsensorVal_3 + ORsensorVal_4)/2 - 200)) ){
          moveForward(LINE_FOLLOW_SPEED);
    }else if(ORsensorVal_1 < midVal && ORsensorVal_2 < midVal &&
       ORsensorVal_3 < midVal && ORsensorVal_4 < midVal){
        moveForward(LINE_FOLLOW_SPEED);  
    }

  //Sharp turn
    if(ORsensorVal_1 > midVal + 100 &&  ORsensorVal_2 > midVal + 100 && ORsensorVal_3 > midVal + 100 && ORsensorVal_4 < midVal - 100 ){
      sharpTurnRight(160, turningTime);
    }else if(ORsensorVal_1 < midVal - 100 && ORsensorVal_2 > midVal + 100 &&  ORsensorVal_3 > midVal + 100 && ORsensorVal_4 > midVal + 100){
      sharpTurnLeft(160, turningTime);
    }    
    
  //if right sensors(1, 2) read higher value, then the robot should turn right
    if( (ORsensorVal_1 + ORsensorVal_2/2) > ((ORsensorVal_3 + ORsensorVal_4)/2 + 500) ){
      Serial.println("In turningright");
      turnRightForward_Fast(TURNING_SPEED_MAX, turningTime);
      //Stop();
    }else if( (ORsensorVal_1 + ORsensorVal_2)/2 > ((ORsensorVal_3 + ORsensorVal_4)/2 + 200) ){
      turnRightForward_Slow(TURNING_SPEED_NORM, turningTime);
    }
    
  //if left sensors(3, 4) read higher value, then the robot should turn left
    if( (ORsensorVal_1 + ORsensorVal_2)/2 < ((ORsensorVal_3 + ORsensorVal_4)/2 - 500) ){
      Serial.println("In turningleft");
      turnLeftForward_Fast(TURNING_SPEED_MAX, turningTime);
      //Stop();
    }else if( (ORsensorVal_1 + ORsensorVal_2)/2 < ((ORsensorVal_3 + ORsensorVal_4)/2 - 200)){
      turnLeftForward_Slow(TURNING_SPEED_NORM, turningTime);
    }
    
  }else{
    Serial.println("In Stop");

    //boolean isBreakpoint = checkBreakpoint(1000);

    Stop();
    
    /*if(isBreakpoint == false){
      Stop();
    }*/  
    /*int numCycle = 0;
    for(numCycle = 0; numCycle <= 100; numCycle++){
      
    }*/
  }
}

//Preparing and calibrating
void lineFollowPrepare(){
  int refVal = 0;
  int count = 0;
  int i = 0;

  //Calibrating the sensors, finding max and min reflectance values
  for(count = 0; count < 10; count++){
    for(i = 15; i < 20; i++){
      refVal = analogRead(i);
      if(refVal > maxSensorVal){
        maxSensorVal = refVal;
      }
  
      if(refVal < minSensorVal){
        minSensorVal = refVal; 
      }                               
  
      delay(1);
    }
  }

  midVal = (maxSensorVal + minSensorVal)/2;
}

void scanGround(){
  ORsensorVal_1 = analogRead(ORsensorpin_1);
  //TempORsensorVal_2 = analogRead(ORsensorpin_2);
  ORsensorVal_2 = analogRead(ORsensorpin_2);
  //ORsensorVal_2 = map(TempORsensorVal_2, minSensorVal + 10, maxSensorVal - 100, minSensorVal, maxSensorVal);
  ORsensorVal_3 = analogRead(ORsensorpin_3);
  ORsensorVal_4 = analogRead(ORsensorpin_4);
}

