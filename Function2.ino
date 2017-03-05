#include<Servo.h>
Servo servo;                    //Create an object myServo

//robot motor pin configuration
//Arduino PWM Speed Control： E1 is right motor; E2 is left motor
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
const int LINE_FOLLOW_SPEED = 255;
const int TURNING_SPEED = 200;
int midVal = 0;
int minSensorVal = 0;
int maxSensorVal = 0;

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
  delay(1000);
  
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
}

/*TODO: We need several testing to find a reasonable connection between 
 * level and DISTANCE_LIMIT to make it approach the object as much as possible
 * right now, just use arbitary level and DISTANCE_LIMIT
 */
 
void decelerate(int level) {
  for (int i = 255; i >= 0; i = i - level) {
    if(i < 0){
      i = 0;
    }
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, i);
    analogWrite(E2, i);
  }
}

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
void turnLeftForward(int power, long Time) {
  unsigned long oldtime;
  oldtime = millis();

  while (millis() - oldtime <= Time) {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, power);
    analogWrite(E2, 0);

    if(((ORsensorVal_1 + ORsensorVal_2)/2) <= ((ORsensorVal_3 + ORsensorVal_4)/2 - 200)){
      Stop();
      delay(10);
      break;
    }
  }
}

void turnRightForward(int power, long Time) {
  unsigned long oldtime;
  oldtime = millis();

  while (millis() - oldtime <= Time) {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, 0);
    analogWrite(E2, power);

    if(((ORsensorVal_1 + ORsensorVal_2)/2) >= ((ORsensorVal_3 + ORsensorVal_4)/2 + 200)){
      Stop();
      delay(10);
      break;
    }
  }
}

char lineFollow(){
  int turningTime = 200;

  /*ORsensorVal_1 = analogRead(ORsensorpin_1);
  ORsensorVal_2 = analogRead(ORsensorpin_2);
  ORsensorVal_3 = analogRead(ORsensorpin_3);
  ORsensorVal_4 = analogRead(ORsensorpin_4);*/

  /*Serial.println(ORsensorVal_1);
  Serial.println(ORsensorVal_2);
  Serial.println(ORsensorVal_3);
  Serial.println(ORsensorVal_4);*/

  if((analogRead(ORsensorpin_1) + analogRead(ORsensorpin_2)/2) <= ((analogRead(ORsensorpin_3) + analogRead(ORsensorpin_4))/2 + 200) &&
      ((analogRead(ORsensorpin_1) + analogRead(ORsensorpin_2)) >= ((analogRead(ORsensorpin_3) + analogRead(ORsensorpin_4))/2 - 200))){
          moveForward(LINE_FOLLOW_SPEED);
          //Serial.println("In Forward");
  }

  if(((analogRead(ORsensorpin_1) + analogRead(ORsensorpin_2))/2) > ((analogRead(ORsensorpin_3) + analogRead(ORsensorpin_4))/2 + 200)){
    //Serial.println("In turningleft");
    turnLeftForward(TURNING_SPEED, turningTime);
    //Stop();
  }
  
  if(((analogRead(ORsensorpin_1) + analogRead(ORsensorpin_2))/2) < ((analogRead(ORsensorpin_3) + analogRead(ORsensorpin_4))/2 - 200)){
    //Serial.println("In turningrigft");
    turnRightForward(TURNING_SPEED, turningTime);
    //Stop();
  }
  
  if(analogRead(ORsensorpin_1) > midVal && analogRead(ORsensorpin_2) > midVal &&
     analogRead(ORsensorpin_3) > midVal && analogRead(ORsensorpin_4) > midVal){
    //Serial.println("In Stop");
    Stop();
  }
  
}

//Preparing and calibrating
void lineFollowPrepare(){
  int refVal = 0;
  int i = 0;

  //Calibrating the sensors, finding max and min reflectance values
  for(i = 15; i < 20; i++){
    refVal = analogRead(i);
    if(refVal > maxSensorVal){
      maxSensorVal = refVal;
    }

    minSensorVal = refVal;
    if(refVal < minSensorVal){
      minSensorVal = refVal;
    }

    delay(1);
  }

  midVal = (maxSensorVal + minSensorVal)/2;
}
