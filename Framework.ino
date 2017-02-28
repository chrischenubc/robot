#include<Servo.h>

Servo servo;                    //Create an object myServo

//Arduino PWM Speed Control： E1 is right motor; E2 is left motor
int E1 = 5;  
int M1 = 4; 
int E2 = 6;                      
int M2 = 7;

int mainTrig = 12; 
int mainEcho = 11;
int servoPin = 13;

int maxSpeed = 255;
int Rscanval, MRscanval, Cscanval, MLscanval,  Lscanval;
int decelerateLevel = 10;
int distancelimit_f = 25;
int num_cycle = 0;

void setup() 
{ 
    pinMode(M1, OUTPUT);   
    pinMode(M2, OUTPUT);
    pinMode(mainTrig, OUTPUT);
    pinMode(mainEcho, INPUT);
    pinMode(servoPin, OUTPUT);
} 

void loop() 
{ 
  int confirm;

  moveForward(255);
  while(sensorScanMain(mainTrig, mainEcho) >= 10){
    if(sensorScanMain(mainTrig, mainEcho) < 10){
      break;
    }
  }

    scanAround();

    char c=decide();
      
     /* if(Rscanval < distancelimit_f || MRscanval < distancelimit_f){
        /*Serial.println("Ready to move back!");
         if(Lscanval <= distancelimit_reverse || MLscanval <= distancelimit_reverse){
          MoveBack(Backtime);  
        }
        /*Serial.println("Already moved back!");
        TurnLeft(300); 
      }
      num_cycle = 0;  
    }*/

    long cm_f = sensorScanMain(mainTrig, mainEcho);

//.....
   /* if(cm_f <= distanceLimit_Front){
      confirm++;  
    }

    if(cm_f > distancelimit_f){
      confirm = 0;  
    }

    if(confirm >= 15){
      Stop();
      MoveBack(Backtime);
      char choice = Decide();
      TurnAround(choice);
      confirm = 0;  
    }*/

    
    /*for android
   /* Serial.print(cm_f);
    Serial.println("cm");*/
}

void moveForward(long power){
   digitalWrite(M1, HIGH);
   digitalWrite(M2, HIGH);
   analogWrite(E1, power);
   analogWrite(E2, power);
}

void suddenStop(){
   digitalWrite(M1, HIGH);
   digitalWrite(M2, HIGH);
   analogWrite(E1, 0);
   analogWrite(E2, 0);
  }

void deceleToStop(int level){
  int i;

  for(i = 255; i >= 0; i = i - level){
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, i);
    analogWrite(E2, i);
  }
}

void turnRight(long Time){
  unsigned long oldtime; 

  oldtime = millis();

  while(millis() - oldtime <= Time){
    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
    analogWrite(E1, 255);
    analogWrite(E2, 255);
  }
}

void turnLeft(long Time){
  unsigned long oldtime; 

  oldtime = millis();

  while(millis() - oldtime <= Time){
    digitalWrite(M1, LOW);
    digitalWrite(M2, HIGH);
    analogWrite(E1, 255);
    analogWrite(E2, 255);
  }
}

void scanAround () {
  unsigned long oldtime;

  long decelerateLevel=16;
  Serial.println("ScanAround");
  /*oldtime = millis();*/
  Cscanval = sensorScanMain(mainTrig, mainEcho);
  if(Cscanval < distancelimit_f){
    deceleToStop(decelerateLevel);
  }
  servo.write(120);
  
 
  Serial.println("Already at 120 degree");
  MLscanval = sensorScanMain(mainTrig, mainEcho);
  if(MLscanval < distancelimit_f){
    deceleToStop(decelerateLevel);
  }
  servo.write(160);

  Lscanval = sensorScanMain(mainTrig, mainEcho);
  if(Lscanval < distancelimit_f){
    deceleToStop(decelerateLevel);  
  }
  servo.write(120);

  MLscanval = sensorScanMain(mainTrig, mainEcho);
  if(MLscanval < distancelimit_f){
    deceleToStop(decelerateLevel);  
  }
  servo.write(80);

  Cscanval = sensorScanMain(mainTrig, mainEcho);
  if(Cscanval < distancelimit_f){
    deceleToStop(decelerateLevel);  
  }
  servo.write(40);

  MRscanval = sensorScanMain(mainTrig, mainEcho);
  if(MRscanval < distancelimit_f){
    deceleToStop(decelerateLevel);  
  }
  servo.write(0);

  servo.write(80);
  /*while(millis() - oldtime <= 300){
    if(Scan_Vertical() == 'B'){
      MoveBack(Backtime);
      TurnRight(200);
    }  
  }*/
}

char decide(){
  char choice;
  scanAround();
  if(Lscanval > Rscanval && Lscanval > Cscanval){
    choice = 'L';
  }
  else if(Rscanval > Lscanval && Rscanval > Cscanval){
    choice = 'R';
  }
  else{
    choice = 'F';
  }
  return choice;
}

long sensorScanMain (int Trigpin, int Echopin){
    long duration, cm;
    
    digitalWrite(Trigpin, LOW);
    delayMicroseconds(2);
    digitalWrite(Trigpin, HIGH);
    delayMicroseconds(5);
    digitalWrite(Trigpin, LOW);
    delayMicroseconds(10);

    duration = pulseIn(Echopin, HIGH);

    cm = 0;

    return cm;
}
    

