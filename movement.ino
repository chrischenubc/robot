#include<Servo.h>

//Arduino PWM Speed Control： E1 is right motor; E2 is left motor
int E1 = 5;  
int M1 = 4; 
int E2 = 6;                      
int M2 = 7;

int maxSpeed = 255;
int Rscanval, MRscanval, Cscanval, MLscanval,  Lscanval;
int decelerateLevel = 10;
int distancelimit_f = 25;
int num_cycle = 0;

void setup() 
{ 
    pinMode(M1, OUTPUT);   
    pinMode(M2, OUTPUT);
} 

void loop() 
{ 
    MoveForward();
    delay(2000);
    Decelerate_Stop(255);
    delay(1000);
    TurnLeft(750);
    Decelerate_Stop(255);
    delay(1000);
    MoveForward();
    delay(2000);
    TurnRight(1500);
    Decelerate_Stop(255);
    delay(1000);
    MoveForward();
    delay(2000);
    Decelerate_Stop(255);
    delay(1000);
    TurnLeft(750);
    Decelerate_Stop(255);
    delay(1000);
    MoveForward();
    delay(2000);
    Decelerate_Stop(8);
    delay(5000);
}

void MoveForward(void){
   digitalWrite(M1, HIGH);
   digitalWrite(M2, HIGH);
   analogWrite(E1, 255);
   analogWrite(E2, 255);
}

void Decelerate_Stop(int level){
  int i;

  for(i = 255; i >= 0; i = i - level){
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, i);
    analogWrite(E2, i);
  }
}

void TurnRight(long Time){
  unsigned long oldtime; 

  oldtime = millis();

  while(millis() - oldtime <= Time){
    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
    analogWrite(E1, 255);
    analogWrite(E2, 255);
  }
}

void TurnLeft(long Time){
  unsigned long oldtime; 

  oldtime = millis();

  while(millis() - oldtime <= Time){
    digitalWrite(M1, LOW);
    digitalWrite(M2, HIGH);
    analogWrite(E1, 255);
    analogWrite(E2, 255);
  }
}

