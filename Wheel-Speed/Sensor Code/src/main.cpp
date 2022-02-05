#include <Arduino.h>
#include <FreqCount.h>

/* These values correspond with the commented code
int pin13 = 13;
float pinRead;
int value = 0;
*/
float R = 0.01; //input this value in meters

void setup() {
  
  Serial.begin(9600);
  FreqCount.begin(1000); //1000 means that the frequency is measured per second

}

void loop() {

/*  pinRead = analogRead(pin13);
  if(pinRead >= 550) {
    value = 0;
  }
  else {
    value = 1;
  }*/

  if(FreqCount.available()) {
  unsigned long count = FreqCount.read();
  float w = count*(17*(PI/180)); //count is the number of teeth per second, then it converts to theta traveled based off spacing of the teeth
  float v = R*w;
  float rpm = w*(60/(2*PI));
  float rpm2 = count*(60/18);
  Serial.print(count); //Frequency in Hz
  Serial.println(" ");
  Serial.print(w); //w is omega, which is angular speed in rads/sec
  Serial.println(" ");
  Serial.print(rpm); //output rpm
  Serial.print(" ");
  Serial.print(rpm2); //second output rpm
  Serial.println(" ");
  Serial.println(v);  //linear velocity in m/s
  }
} 