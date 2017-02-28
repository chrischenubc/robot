#include<Servo.h>

//Arduino PWM Speed Control： E1 is right motor; E2 is left motor
int E1 = 5;  
int M1 = 4; 
int E2 = 6;                      
int M2 = 7;

int mainTrig = 12; 
int mainEcho = 11;
int servo = 13;

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
    pinMode(servo. OUTPUT);
} 

void loop() 
{ 
  int confirm;

  MoveForward();
  while(SensorScan_Main(mainTrig, mainEcho) >= 10){
    if(SensorScan_Main(mainTrig, mainEcho) < 10){
      break;
    }
  }

    ScanAround();

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

    cm_f = SensorScan_Main(SensorTrig, SensorEcho);

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

void MoveForward(void){
   digitalWrite(M1, HIGH);
   digitalWrite(M2, HIGH);
   analogWrite(E1, 255);
   analogWrite(E2, 255);
}

void Decelerate_Stop(int decelerateLevel){
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

void ScanAround () {
  unsigned long oldtime;

  Serial.println("ScanAround");
  /*oldtime = millis();*/
  Cscanval = SensorScan_Main(SensorTrig, SensorEcho);
  if(Cscanval < distancelimit_f){
    Decelerate_Stop(decelerateLevel);
  }
  servo.write(120);
  
 
  Serial.println("Already at 120 degree");
  MLscanval = SensorScan_Main(SensorTrig, SensorEcho);
  if(MLscanval < distancelimit_f){
    Decelerate_Stop(decelerateLevel);
  }
  servo.write(160);

  Lscanval = SensorScan_Main(SensorTrig, SensorEcho);
  if(Lscanval < distancelimit_f){
    Decelerate_Stop(decelerateLevel);  
  }
  servo.write(120);

  MLscanval = SensorScan_Main(SensorTrig, SensorEcho);
  if(MLscanval < distancelimit_f){
    Decelerate_Stop(decelerate_Level);  
  }
  servo.write(80);

  Cscanval = SensorScan_Main(SensorTrig, SensorEcho);
  if(Cscanval < distancelimit_f){
    Decelerate_Stop(decelerateLevel);  
  }
  servo.write(40);

  MRscanval = SensorScan_Main(SensorTrig, SensorEcho);
  if(MRscanval < distancelimit_f){
    Decelerate_Stop(decelerateLevel);  
  }
  servo_sensor.write(0);

  servo_sensor.write(80);
  while(millis() - oldtime <= 300){
    if(Scan_Vertical() == 'B'){
      MoveBack(Backtime);
      TurnRight(200);
    }  
  }
}

char Decide(){
  char choice;
  ScanAround();
  if(LscanVal > RcanVal && LscanVal > cScanVal){
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

long SensorScan_Main (int Trigpin, int Echopin){
    long duration, cm;
    
    digitalWrite(Trigpin, LOW);
    delayMicroseconds(2);
    digitalWrite(Trigpin, HIGH);
    delayMicroseconds(5);
    digitalWrite(Trigpin, LOW);
    delayMicroseconds(10);

    duration = pulseIn(Echopin, HIGH);

    cm = microsecondsToCentimeters(duration);

    return cm;
}
    

