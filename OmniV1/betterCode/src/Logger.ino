#include <Arduino.h>
#include <SD.h>
#include "Adafruit_10DOF.h"
#include "Adafruit_ADS1X15.h"
#include "FreqMeasureMulti.h"
#include <TimeLib.h>

File MainLog;

//Accel
Adafruit_L3GD20_Unified gyro  = Adafruit_L3GD20_Unified(69);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(420);

//Shock Pot
//Adafruit_ADS1115 Ads1115s;
int16_t frontRight,frontLeft,rearRight,rearLeft;

//Brake  data
Adafruit_ADS1115 Ads1115;
int16_t frontPressure,rearPressure;

//Wheel Speed
FreqMeasureMulti right;
FreqMeasureMulti left;
FreqMeasureMulti Output; 
double sum=0;
int count=0;

bool Ready = false;
int sensor = 0;
const int chipSelect = BUILTIN_SDCARD;
int Id = 1111;
const char filename;
String dataString;

void setup() {
Serial.begin(115200);
while (!Serial);  // Wait for Arduino Serial Monitor to open
delay(100);
if (timeStatus()!= timeSet) {
  Serial.println("Unable to sync with the RTC");
} 
else {
  Serial.println("RTC has set the system time");
}
setSyncProvider(Teensy3Clock.get);
//right.begin(5);
//left.begin(6);
//Output.begin(9);

Serial.println("setup 1-3");
//Sensor Initialization
  if(!accel.begin())
    {
      Serial.println(F("Accel detected"));
      while(1);
    }
  if(!gyro.begin())
    {
      Serial.println(F("Gyro error detected"));
      while(1);
    }
  /*
  if (!Ads1115.begin(0x48)) {
      Serial.println(F("Failed to initialize ADS-Shock."));
      while (1);
    }
    */
Serial.println("setup 2-3");
  //Sd card Initalization
  Serial.print("Initializing SD card...");
      if (!SD.begin(chipSelect)) {
        Serial.println("initialization failed!");
        return;
      }
      Serial.println("initialization done.");

Serial.println("Setup 3-3");
}

void SD_Write(String dataString)
{
      File dataFile = SD.open("Run_1", FILE_WRITE);
      // If the data file is available, write to it:
      if (dataFile) {
        dataFile.println(dataString);
        //Serial.println(dataString);
        
        dataFile.close();
      } else {
        // If the file isn't open, report an error:
        Serial.println("Error opening SD for " + String("Run_1"));
      }
}


//Sensor Log
void AccelLog(){
  //create event 
  sensors_event_t event;

  //get event
  accel.getEvent(&event);
  gyro.getEvent(&event);

  //Sensor Datapoints
  float AccelX = event.acceleration.x;
  float AccelY = event.acceleration.y;
  float AccelZ = event.acceleration.z;
  float GyroX = event.gyro.x;
  float GyroY = event.gyro.y;
  float GyroZ = event.gyro.z;
  dataString += millis();
  dataString += ", ";
  dataString += AccelX;
  dataString += ", ";
  dataString += AccelY;
  dataString += ", ";
  dataString += AccelZ;
  dataString += ", ";
  dataString += GyroX;
  dataString += ", ";
  dataString += GyroY;
  dataString += ", ";
  dataString += GyroZ;
  dataString += (String(", "));
}

void ShockLog(){
  frontRight = analogRead(A0);
  frontLeft = analogRead(A1);
  rearLeft = analogRead(A2);
  rearRight = analogRead(A3);
  //dataString += millis();
  dataString += frontRight;
  dataString += ", ";
  dataString += frontLeft;
  dataString += ", ";
  dataString += rearLeft;
  dataString += ", ";
  dataString += rearRight;
  dataString += String("\n");
}
/*
void BrakeLog(){
  frontPressure = Ads1115.readADC_SingleEnded(0);
  rearPressure = Ads1115.readADC_SingleEnded(1);
  dataString += frontPressure;
  dataString += ", ";
  dataString += rearPressure;
  dataString += ", ";
  dataString += String(" \n ");
}
void WheelLog(){
int16_t Left;
int16_t Right;
int16_t output;

  if(left.available()){
  sum = sum + left.read();
    count = count + 1;
    if (count > 30) {
      float Leftfrequency = left.countToFrequency(sum / count);
      //Serial.println(Leftfrequency);
      Left = Leftfrequency;
      sum = 0;
      count = 0;
    }
  }
  if (right.available()){
  sum = sum + right.read();
    count = count + 1;
    if (count > 30) {
      float Rightfrequency = right.countToFrequency(sum / count);
      //Serial.println(Rightfrequency);
      Right = Rightfrequency;
      sum = 0;
      count = 0;   
    }
  }
  if(Output.available()){
  sum = sum + Output.read();
    count = count + 1;
    if (count > 30) {
      float Outputfrequency = Output.countToFrequency(sum / count);
      //Serial.println(Outputfrequency);
      output = Outputfrequency;
      sum = 0;
      count = 0;
    }
  }
  dataString += Left;
  dataString += ", ";
  dataString += Right;
  dataString += ", ";
  dataString += output;
  //dataString += " \n ";
}
*/



void loop() {
  if (Serial.available()) {
  time_t t = processSyncMessage();
  if (t != 0) {
    Teensy3Clock.set(t); // set the RTC
    setTime(t);
  }
}
 //Serial.println("To loop"); 
  if(!Ready){
    dataString += (String("Run 2|Accel(M/s^2)XYZ | Gyro(rad/s)XYZ | ShockPots - FrontRight, FrontLeft, RearRight, RearLeft \n"));
    delay(100);
    Ready = true;
  }

Serial.println("ready");  
  AccelLog();
  ShockLog();
  //BrakeLog();
  //WheelLog();
  SD_Write(dataString);
  Serial.println(dataString);
  delay(50);
  dataString = "";
}





#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
  }
  return pctime;
}