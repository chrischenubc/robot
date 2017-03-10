#include<Servo.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
Servo servo;                    //Create an object myServo
SoftwareSerial BT(9, 10); 
// initialize the LCD library with the numbers of the interface pins

//robot motor pin configuration
//Arduino PWM Speed Control： E1 is right motor; E2 is left motor
const int E1 = 5;
const int M1 = 4;
const int E2 = 6;
const int M2 = 7;

//pin configurations
const int TRIG_PIN = 12;
const int ECHO_PIN = 11;
const int SERVO_PIN = 13;
const int LM_PIN = A0;      //Pin for LM-35 temperature sensor
const int SWITCH_PIN = 8;        //Pin for pull up Switch
const int MAIN_SWITCH_PIN = A5;   // main switch pin configuration

//Declaration of variables for basic function 2
//Pins for 2 optical reflective sensors
const int ORsensorpin_1 = A1;
const int ORsensorpin_2 = A2;
const int ORsensorpin_3 = A3;
const int ORsensorpin_4 = A4;


//define some constants here
const int DISTANCE_LIMIT = 15;
const int MAX_SPEED = 255;
const int DECELERATION = 45;
const int LINE_FOLLOW_SPEED = 190;
const int TURNING_SPEED_NORM = 120;
const int TURNING_SPEED_DIFF = 80;
const int TURNING_SPEED_MAX = 225;



//define some global variables here

char instr='A';
int dis;
int main_switch = 0; 
int temperature;
int lcd_distance;
int flag=1;
int velocity=255;
//declaration of variables for hall effect sensor
volatile byte half_revolutions;
unsigned int rpm;
unsigned long timeold;

float speedLeft;
float speedRight;
long normalPower=200;
long newPower=normalPower;



int midVal = 0;
int minSensorVal = 1023;
int maxSensorVal = 0;

int TempORsensorVal_2;
int ORsensorVal_1;
int ORsensorVal_2;
int ORsensorVal_3;
int ORsensorVal_4;

void setup()
{
  pinMode(M1, OUTPUT);        //DC motor
  pinMode(M2, OUTPUT);
  
  pinMode(TRIG_PIN, OUTPUT);    //Sonar
  pinMode(ECHO_PIN, INPUT);
 
  pinMode(SWITCH_PIN, INPUT_PULLUP);    //Switches
  pinMode(MAIN_SWITCH_PIN,INPUT);
  Serial.begin(9600);
  servo.attach(SERVO_PIN);
  BT.begin(9600);
}

void loop()
{     
  main_switch = digitalRead(MAIN_SWITCH_PIN);
  if(main_switch == HIGH){    
   /* choose the mode based on the pushed button */  
   chooseMode(flag);
   //readSwitch();
   update_flag(instr);
  /* update the flag based on the signal received */
  
  /* after successfully change the mode, make it stop and wait for 1 sec */
   Serial.print("Successfully changed the mode ");
   Stop();
   delay(1000); 
  }
}

/*
 * Mode 1: Basic functionality 1
 * The robot will move at its maximum speed forward until it detects an object in front of it at some distance limit
 * When the robot detects an object, it will decelerate and rotate the sensors to find the direction that has larger space
 * Then the robot turn left or right towards that direction.
 */

void mode1(){

  Serial.println("In mode 1");      //For Debug use
  moveForward(velocity);
  
  do {
    //LCD output instrctions
    /*lcd.setCursor(0,0);
    lcd.print("temp is ");
    lcd.print(readTmpLM());
    lcd.setCursor(0,1);
    lcd.print("dis is ");
    lcd.setCursor(7,1);
    lcd.print(readSonar());*/
    
    Serial.println("moving forward");
   // update_keyboard();
    update_instr();
    if(digitalRead(MAIN_SWITCH_PIN)==LOW){
      return;
    }
    if( flag!=1 ||instr =='F' || instr =='C') {
      return;   //check the interrupt flag
    }
    
    dis=readSonar();
    //detect range, if less than limit, decelerate
    if (dis < DISTANCE_LIMIT  && dis !=-1) {
     decelerate(DECELERATION);
     Serial.println("break out the loop!!!!");
     Stop();
     break;
    }
     dis=readSonar();
  }while((dis >= DISTANCE_LIMIT || dis ==-1 )&& digitalRead(MAIN_SWITCH_PIN)==HIGH);   //check if anychanges after one loop
    //update_keyboard();
    update_instr();
   // this code block is used to monitor switches
   if(digitalRead(MAIN_SWITCH_PIN)==LOW){
    return;
   }
    if(flag!=1 || instr =='F' || instr =='C' ) {
      return; //return to loop method
    }
    Stop();
  int scanVal=scanAround(2);
  delay(500);
  rotate(scanVal);  
}



//basic functionality 2
void mode2(){
  lineFollowPrepare();
  //delay(1000);
  while(flag==2){                //indicates line following
    //Serial.println("In mode 2");    //for debug use
   // update_keyboard();
    update_instr();
    if(digitalRead(MAIN_SWITCH_PIN)==LOW){
      return;
    }
    else if(flag!=2 || instr=='A' || instr=='C') {
      return;
    }
    lineFollow();      
  }
 
}



//additional functionality 
//will be controlled by bluetooth
void mode3(){
  while(flag==3 ){
    Serial.println("In mode 3");
    read_instruction();
    //read_keyboard();
    if(digitalRead(MAIN_SWITCH_PIN)==LOW){
      return;
    }
    else if(flag!=3 || instr =='A' || instr =='F') {
      return;
    }   
  }  
}

/**
 * Choose the mode based on the signal it received
 * @param flag: the mode we want to choose
 * if it is '1', then change to mode1;
 * if it is '2', then change to mode2;
 * if it is '3', then change to mode3
 */
void chooseMode(int flag){
  switch(flag){
    case 1:mode1();
    break;
    case 2:mode2();
    break;
    case 3:mode3();
    break;
    default : 
    mode1();
  }
}

char readSwitch(){
  if( digitalRead(SWITCH_PIN)==LOW){
    return '2';  
   }
   else if (digitalRead(SWITCH_PIN)==HIGH) {
    return '1';
   }   
}

void update_flag(char instr){
  if(instr=='A'){
        flag=1;
      }
  else if (instr=='F'){
        flag=2;
  }
  else if (instr=='C'){
        flag=3;
  }      
}


/*
 * The function is to move robot forward at the given power
 * and the robot will not stop moving forward until it receives a new instruction,such as Stop()
 * @param power to control the speed
 * the higher the power is, the fast the robot moves
 * the power is range from [0,255]
 * 
 * 
 */
void moveForward(long power) {
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, power);
  analogWrite(E2, power);
}

void moveBackward(long power) {
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  analogWrite(E1, power);
  analogWrite(E2, power);
}

/**
 * The function is to stop the robot immedietly
 */
void Stop() {
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, 0);
  analogWrite(E2, 0);
  newPower=normalPower;
}

/**
 * make the robot stop gradully based on the deceleraion it is given
 * @param deceler: the decelertion level
 * if it is small, it takes more time for robot to stop
 */ 
void decelerate(int DECELERATION) {
  for (int i = 255; i >= 0; i = i - DECELERATION) {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, i);
    analogWrite(E2, i);
    delay(5);
  }
}

void turnRight(long Time) {
  Serial.println("turn Right");
  unsigned long oldtime;
  oldtime = millis();

  while (millis() - oldtime <= Time) {
    digitalWrite(M1, LOW);
    digitalWrite(M2, HIGH);
    analogWrite(E1, 255);
    analogWrite(E2, 255);
  }
}

void turnLeft(long Time) {
  Serial.println("turn left");
  unsigned long oldtime;
  oldtime = millis();

  while (millis() - oldtime <= Time) {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
    analogWrite(E1, 255);
    analogWrite(E2, 255);
  }
}

/**
 * scan the surrounding from 0 to 180 degrees
 * @param parts:divide 180 degrees into several parts
 *  the bigger the parts is, the sensor will stop more times to record values
 * @return the angle the robot need to turn 
 *  the angle should between [0,180]
 */
 int scanAround(int parts){
  int vals[parts];
  int maxDegree=0;
  int maxAngle=0;
  servo.write(0); //turn the servo to 0 degree
  for(int i=0;i<=parts;i++){
    servo.write(i*180/parts);
    delay(500);
    vals[i]=readSonarAvg();
    Serial.print("Distance at:");   //for debug use
    Serial.println(i);
    Serial.println(vals[i]);
    
    if (vals[i]>=maxDegree){
      maxAngle=i;
      maxDegree=vals[i];
    }
  }
 servo.write(90);  //make the sensor facing forward again
 return maxAngle;
}
/**
 * rotate the robot based on the given degree 
 * if 0<=degree<90, turn right
 * if 90<degree<=180, turn left
 * if degree=90, do nothing
 */
void rotate(int degree){
  if( degree==0){
    turnRight(250);
  }
  else if ( degree==2){
    turnLeft(250); //TODO Chanve the value to make it turn 90 degree
  }
}

 /** 
  *  Function: Control the sonar to send a pulse, and measure the duration from the echo, calculate the distance as per temperature
  *  @return: a long that indicates the distance it gets, in cm, from 0 to 200, -1 if the range is not reasonable.
  */


long readSonar(){
    long duration, distance;
    digitalWrite(TRIG_PIN, HIGH);           //set trigger pin to HIGH
    delayMicroseconds(1000);
    digitalWrite(TRIG_PIN, LOW);            //set trigger pin to LOW
    duration = pulseIn(ECHO_PIN, HIGH);     //read echo pin
    
    long temp=readTmpLM();                 //read temperature
    long sound_speed=331.5 + (0.6 * temp);                 //calculate the sound speed at the point
    distance = (duration * sound_speed * 0.0001)/2;        //compute distance from duration of echo Pin
   // Serial.println(duration);
    Serial.println(distance);
   // delay(50); //Warning: make delay 50ms
    if (distance >= 200 || distance <= 0){   //deciding whether or not distance is reasonable
        return(-1);                         //if not, return -1
    }
    else{
        return(distance);
    }
  }

/**
 * Function: Read from analog pin the LM35 temperature sensor is connected to, and map the voltage to temperature in degree celsius
 * @Param: None
 * @Return: Float value indicates the temperature reading from LM35, nan if the sensor is not ready
*/
float readTmpLM(){
  float readVoltage = analogRead(LM_PIN);
  delay(10);
  readVoltage = analogRead(LM_PIN);        //Read twice and discard the first value so that the reading is not interfered by noise
  float tmp = map(readVoltage, 0, 225, 0, 100);
  temperature=tmp;
  
  return tmp;
}

int readSonarAvg(){
  int a=readSonar();
  delay(5);
  int b=readSonar();
  delay(5);
  int c=readSonar();
  delay(5);
  int d=readSonar();
  return (a+b+c+d)/4;
}


void update_instr() {
  if(BT.available()) {
     String s;
      
      s = BT.readString();
      
      instr=s[0];
      Serial.print("keyboard command received:    ");
      Serial.println(instr);
    } 
}


/****************************************************************************
/*
 * This method is to use Bluetooth to control the car. We will receive the char type 
 * input from android app via bluetooth, which we named it as "instruction". Then we use
 * several if statments to determine the command that we receive. 'R' means turn right. 'L'
 * means turn left. 'M' means move forward. And 'S' means sudden stop. 'D' means deacceleration.
 * Besides, we also have '1','2' and '3' represents 3 different models, Auto represents '1',
 * '2' means Tracing model, '3' means Manual mode. If we receive the signal except these, it will return error 
 */
void read_instruction(){
    char instruction;
  
    if( BT.available()){
      // if text arrived in from BT serial

      instruction = BT .read();
      
       Serial.println(instruction);
      if(instruction == 'R' ){             //turn right instruction
        turnRight(750);                  // I think the time interval for this is quite long
        Serial.println("Turn Right");
      }else if(instruction == 'L' ){      //turn left instruction
        turnLeft(750);
        Serial.println("Turn Left");
      }else if(instruction == 'M' ){      //move forward instruction
        moveForward(velocity);
        Serial.println("Move Forward");
      }
      else if(instruction == 'B' ){      //move forward instruction
        moveBackward(velocity);
        Serial.println("Move Backward");
      }
      else if(instruction == 'D' ){      //move forward instruction
        decelerate(DECELERATION);
        Serial.println("Decelerate");
      }
      else if(instruction == 'S' ){      //sudden stop insturction  
        Stop();
        Serial.println("Sudden Stop");
      }
      else if(instruction == 'A'){     //switch mode to automatical driving     
         instr='A';
      }
      else if(instruction == 'F'){     //switch mode to automatical driving      
         instr='F';          
      }
      else if(instruction == 'C'){     //switch mode to automatical driving       
         instr='C';           
      }       
      else{                            //instruction do not exist
         Serial.println("Wrong Instruction");   // print out wrong instruction
      }
    }
}

/****************************************************************************
/*
 * THIS FUNCTION IS FOR TESTING USE
 * TO READ THE SERIAL INPUT FROM KEYBOARD
 * IT CAN BE DELETED AFTER THE TESTS
 */
void read_keyboard(){
    char instruction;
  
    if( Serial.available()){
      // if text arrived in from serial

      String s= Serial.readString();
      instruction=s[0];
      
      Serial.println(instruction);
      if(instruction == 'R' || instruction == 'r'){             //turn right instruction
        turnRight(750);                  // I think the time interval for this is quite long
        Serial.println("Turn Right");
      }else if(instruction == 'L' || instruction == 'l'){      //turn left instruction
        turnLeft(750);
        Serial.println("Turn Left");
      }else if(instruction == 'M' || instruction == 'm'){      //move forward instruction
        moveForward(255);
        Serial.println("Move Forward");
      }
      else if(instruction == 'D' || instruction == 'd'){      //move forward instruction
        decelerate(DECELERATION);
        Serial.println("Decelerate");
      }
      else if(instruction == 'S' || instruction == 's'){      //sudden stop insturction  
        Stop();
        Serial.println("Sudden Stop");
      }
      else if(instruction == '1'){     //switch mode to automatical driving        
         instr='1';
      }
      else if(instruction == '2'){     //switch mode to automatical driving       
         instr='2';          
      }
      else if(instruction == '3'){     //switch mode to automatical driving
         instr='3';           
      }       
      else{                            //instruction do not exist
         Serial.println("Wrong Instruction");         
      }
    }
}


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


/*
 * THIS FUNCTION IS FOR TESTING USE
 * TO READ THE SERIAL INPUT FROM KEYBOARD
 * IT CAN BE DELETED AFTER THE TESTS
 */
void update_keyboard() {
     if(Serial.available()){
      String s= Serial.readString();
      instr=s[0];
      Serial.print("keyboard command received:    ");
      Serial.println(instr);
     }
}


/*
 * The function is to move robot forward at the given power
 * and the robot will not stop moving forward until it receives a new instruction,such as Stop()
 * @param power to control the speed
 * the higher the power is, the fast the robot moves
 * the power is range from [0,255]
 * 
 * 
 */
void adjustMovSpeed(long leftPower, long rightPower) {
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, leftPower);
  analogWrite(E2, rightPower);
}

//Used for hall effect sensor
void compareRpm(){
  
  if(speedLeft>speedRight){
    adjustMovSpeed(newPower-5,normalPower);
  }
  else if(speedLeft<speedRight){
    adjustMovSpeed(newPower+5,normalPower);
  }
}

char lineFollow(){
  int turningTime = 1000;

  scanGround();

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
    }else if( (ORsensorVal_1 + ORsensorVal_2)/2 > ((ORsensorVal_3 + ORsensorVal_4)/2 + 200) ){
      turnRightForward_Slow(TURNING_SPEED_NORM, turningTime);
    }
    
  //if left sensors(3, 4) read higher value, then the robot should turn left
    if( (ORsensorVal_1 + ORsensorVal_2)/2 < ((ORsensorVal_3 + ORsensorVal_4)/2 - 500) ){
      Serial.println("In turningleft");
      turnLeftForward_Fast(TURNING_SPEED_MAX, turningTime);
    }else if( (ORsensorVal_1 + ORsensorVal_2)/2 < ((ORsensorVal_3 + ORsensorVal_4)/2 - 200)){
      turnLeftForward_Slow(TURNING_SPEED_NORM, turningTime);
    }
    
  }else{
    Serial.println("In Stop");

    Stop();
    
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
  ORsensorVal_2 = analogRead(ORsensorpin_2);
  ORsensorVal_3 = analogRead(ORsensorpin_3);
  ORsensorVal_4 = analogRead(ORsensorpin_4);
}

