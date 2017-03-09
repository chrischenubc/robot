
void setup() {
  Serial.begin(9600);
}

void loop() {
  int temp;
  int speedOfRobot;
  int distance;

  temp=20+random(-2,2);
  speedOfRobot=10+random(-10,10);
  distance=100+random(-100,100);

  //Serial.write("This message is used to block the two readings");
  Serial.print(1001);
  Serial.write("This message is used to block the two readings");
  Serial.print(temp);
  Serial.write("This message is used to block the two readings");
  Serial.print(1002);
  Serial.write("This message is used to block the two readings");
  Serial.print(speedOfRobot);
  Serial.write("This message is used to block the two readings");
  Serial.print(1003);
  Serial.write("This message is used to block the two readings");
  Serial.print(distance);
  Serial.write("This message is used to block the two readings");
}



void sendData(int temp, int speedOfRobot, int distance){
   Serial.print(1001);                                                        //Identify which data this is
  Serial.write("This message is used to block the two readings");
  Serial.print(temp);                                                         //data value of temperature reading
  Serial.write("This message is used to block the two readings");
  Serial.print(1002);
  Serial.write("This message is used to block the two readings");
  Serial.print(speedOfRobot);
  Serial.write("This message is used to block the two readings");
  Serial.print(1003);
  Serial.write("This message is used to block the two readings");
  Serial.print(distance);
  Serial.write("This message is used to block the two readings");
  
  }
