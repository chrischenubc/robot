#include<Servo.h>
#include <SoftwareSerial.h>
Servo servo;                    //Create an object myServo
SoftwareSerial BT(9, 10); 
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

const int LM_PIN = A0;      //Pin for LM-35 temperature sensor
const int SWITCH_PIN = 3;        //Pin for pull up Switch

//define some constants here
const int DISTANCE_LIMIT = 35;
const int MAX_SPEED = 255;

const int DECELERATION = 45;

volatile byte half_revolutions;
unsigned int rpm;
unsigned long timeold;

volatile int flag=1;
int dis;

char instr='1';

void setup()
{
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
 
  pinMode(SWITCH_PIN, INPUT);

  servo.attach(SERVO_PIN);
  Serial.begin(9600);
  BT.begin(9600);
  
  //interrupt when button is read HIGH
   //attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), isr, HIGH); 
//   attachInterrupt(0, rpm_fun, RISING);
}

void isr(){
  flag++;
  if(flag==3){
    flag=1;
  }
}
void loop()
{    
   /* choose the mode based on the flag we have */    
  chooseMode(flag);
  update_flag(instr);
  Serial.print("Successfully changed the mode ");
  //Stop();
  //delay(1000); 
}

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

void mode1(){
  //servo.write(90);  //make the sensor facing forward again
   Serial.println("In mode 1");
  moveForward(180);
  do {
    Serial.println("moving forward");

    // update_keyboard();
    update_instr();
    if(flag!=1 || instr =='2' || instr =='3' ) {
      return; //check the interrupt flag
    }
    
    dis=readSonar();
    if (dis < DISTANCE_LIMIT  && dis !=-1) {
     decelerate(DECELERATION);
     
     Serial.println("break out the loop!!!!");
     break;
    }
     dis=readSonar();
  }while(dis >= DISTANCE_LIMIT || dis ==-1);
 
   // update_keyboard();
    update_instr();
    if(flag!=1 || instr =='2' || instr =='3' ) {
      return; //check the interrupt flag
    }
  int scanVal=scanAround(2);
  Serial.print("degree is.....");
  Serial.print(scanVal);
 rotate(scanVal);  
}

//basic functionality 2
void mode2(){

  while(flag==2) {
    Serial.println("In mode 2");

   // update_keyboard(); 
  update_instr();
    if(flag!=2 || instr =='1' || instr =='3' ) {
      return;
    }

    /**
     * TODO:do someting here: codes about function 2
     */
       
  }
 
}

//additional functionality 
void mode3(){

  
  while(flag==3 ){
   // Serial.println("In mode 3");
    read_instruction();
   // read_keyboard(); 
    //update_keyboard();
    if(flag!=3 || instr =='1' || instr =='2' ) {
      return;
    }   
  }  
}


void update_flag(char instr){
  if(instr=='1'){
        flag=1;
      }
  else if (instr=='2'){
        flag=2;
  }
  else if (instr=='3'){
        flag=3;
  }
      
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

   /*  update_keyboard(); 
    if(flag!=1 || (instr=BT.read()) =='B' || instr =='M' ) {
      switch(instr){
        case 'M':flag=3;
      }
      return -1; //check the interrupt flag
    }*/
    servo.write(i*180/parts);
    delay(500);
    vals[i]=readSonarAvg();
    Serial.print("Distance at:");
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
/*TODO: The robot will then use the servo motor to move the ultrasonic sensor 
 * independently to scan from left to the right side of the robot and picks the side 
 * that has the best/longest free space for movement and then it turns 90 degrees 
 * (to left or right dependent on the input from the sensor) 
 * and continues the movement until it reaches another object.
 * 
 * How to decide it to turn?
 */ 
 
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
 * Function: Control the sonar to send a pulse, and measure the duration from the echo, calculate the distance as per temperature
 * @return: a long that indicates the distance it gets, in cm, from 0 to 200, -1 if the range is not reasonable.
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
    Serial.println(distance);
       delay(50); //Warning: make delay 50ms
    if (distance >= 200 || distance <= 0){   //deciding whether or not distance is reasonable
        return(-1);                         //if not, return -1
    }
    else{
        return(distance);
    }
  }

/**
 * Function: Read from analog pin the LM35 temperature sensor is connected to, and map the voltage to temperature in degree celsius
 * Param: None
 * Return: Float value indicates the temperature reading from LM35, nan if the sensor is not ready
*/
float readTmpLM(){
  float readVoltage = analogRead(LM_PIN);
  delay(10);
  readVoltage = analogRead(LM_PIN);        //Read twice and discard the first value so that the reading is not interfered by noise
  float tmp = map(readVoltage, 0, 225, 0, 100);
  return tmp;
}

/**
 * Function: Read the status of the switch ON/OFF
 * Param: None
 * Return: integer value indicate the status of the switch, '1' for On, '0' for Off
*/

int readSwitch(){
    if(digitalRead(SWITCH_PIN) == LOW){     //Use pull up resistor, switch considered closed when reading is low
          return 1;
      }
    return 0;
}


/****************************************************************************
/*
 * This method is to use Bluetooth to control the car
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
        moveForward(255);
        Serial.println("Move Forward");
      }
      else if(instruction == 'D' ){      //move forward instruction
        decelerate(DECELERATION);
        Serial.println("Decelerate");
      }
      else if(instruction == 'S' ){      //sudden stop insturction  
        Stop();
        Serial.println("Sudden Stop");
      }
      else if(instruction == '1'){     //switch mode to automatical driving
         flag=1;
         instr='1';
      }
      else if(instruction == '2'){     //switch mode to automatical driving
         flag=2;
         instr='2';          
      }
      else if(instruction == '3'){     //switch mode to automatical driving
         flag=3;
         instr='3';           
      }       
      else{                            //instruction do not exist
         Serial.println("Wrong Instruction");         
      }
    }
}


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
         flag=1;
         instr='1';
      }
      else if(instruction == '2'){     //switch mode to automatical driving
         flag=2;
         instr='2';          
      }
      else if(instruction == '3'){     //switch mode to automatical driving
         flag=3;
         instr='3';           
      }       
      else{                            //instruction do not exist
         Serial.println("Wrong Instruction");         
      }
    }
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
  
void update_instr() {
  if(BT.available()) {
     String s;
      s = BT.readString();
      instr=s[0];
      Serial.print("keyboard command received:    ");
      Serial.println(instr);
    } 
    //commented out because i dont think it is useful
    /*else {
      instr = NULL;
      Serial.println("No new Bluetooth command");
    }*/
}

