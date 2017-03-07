volatile byte  count = 0;
byte numCount = 8; //number of pulse intervals to measure
  

volatile unsigned long startTime;
volatile unsigned long endTime;
unsigned long copy_startTime;
unsigned long copy_endTime;

volatile boolean finishCount = false;
float period;

unsigned int rpm = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("start...");

  attachInterrupt(digitalPinToInterrupt(3), isrCount, FALLING);//interrupt on pin3
}

void loop()
{
  if (finishCount == true)
  {
    finishCount = false;//reset flag
    // disable interrupts, make protected copy of time values
    noInterrupts();
    copy_startTime = startTime;
    copy_endTime = endTime;
    count = 0;
    interrupts();

    period = (copy_endTime - copy_startTime) / 1000.0; //micros to millis
    //debug prints
    Serial.print(period); //total time for numCount
    Serial.print('\t');
    Serial.println(period/numCount);//time between individual pulses
    
    rpm = numCount * 60.0 * (1000.0 / period);//one count per revolution
    //rpm = numCount * 30.0 * (1000.0 / period);//two counts per revolution
  
    Serial.print("RPM = ");
    Serial.println(rpm);
  }
}


void isrCount()
{
  if (count == 0)//first entry to isr
  {
    startTime = micros();
  }

  if (count == numCount)
  {
    endTime = micros();
    finishCount = true;    
  }
  count++; //increment after test for numCount
}
