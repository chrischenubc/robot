#include<Servo.h>
#include <SoftwareSerial.h>
SoftwareSerial BT(10, 11); 
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

  read_instruction();
  /*  moveForward(255);
    delay(2000);
    Decelerate_Stop(255);
    delay(1000);
    TurnLeft(750);
    Decelerate_Stop(255);
    delay(1000);
    moveForward(255);
    delay(2000);
    TurnRight(1500);
    Decelerate_Stop(255);
    delay(1000);
    moveForward(255);
    delay(2000);
    Decelerate_Stop(255);
    delay(1000);
    TurnLeft(750);
    Decelerate_Stop(255);
    delay(1000);
    moveForward(255);
    delay(2000);
    Decelerate_Stop(8);
    delay(5000);*/
}


void moveForward(long power){
   digitalWrite(M1, HIGH);
   digitalWrite(M2, HIGH);
   analogWrite(E1, power);
   analogWrite(E2, power);
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



void suddenStop(){
   digitalWrite(M1, HIGH);
   digitalWrite(M2, HIGH);
   analogWrite(E1, 0);
   analogWrite(E2, 0);
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
      if(instruction == 'R'){             //turn right instruction
        TurnRight(1500);                  // I think the time interval for this is quite long
        BT.println("Turn Right");
       }else if(instruction == 'L'){      //turn left instruction
        TurnLeft(1500);
        BT.println("Turn Left");
       }else if(instruction == 'M'){      //move forward instruction
        moveForward(255);
        BT.println("Move Forward");
       }else if(instruction == 'S'){      //sudden stop insturction
        suddenStop();
        BT.println("Sudden Stop");
        }
        
       /* else if(instruction == 'A'){     //switch mode to automatical driving
            moveForward(255);
            while(sensorScanMain(mainTrig, mainEcho) >= 10){
              if(sensorScanMain(mainTrig, mainEcho) < 10){
                break;
              }
            }
        }*/
        
        else{                            //instruction do not exist
          BT.println("Wrong Instruction");
          Decelerate_Stop(255);
          }
 
  }else{
      Decelerate_Stop(255);               //if there is no instruction for car
    }    
}
