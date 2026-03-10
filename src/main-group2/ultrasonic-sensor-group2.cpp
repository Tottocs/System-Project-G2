#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "v.1.0"
#endif

/* 
Author: Torin Stanton-Andersson
Taken from a previous assignment by this author.

The function, Get_Distance, uses the HCSR04 ultrasonic sensor to output a distance measurement.
It outputs an unsigned long in units of cm. 

To create an object of US_Sensor one must provide a trigger pin number.
The function, Setup_Echo_Pin, requires an input pin number corresponding to the echo pin.
After that, <object>.Get_Distance_CM() can be used to return the distance.
*/

#include "include\ultrasonic-sensor-group2.h"
#include <Arduino.h>

//Defining constants
#define WRITE_DELAY 5
#define TRIG_PULSE_LENGTH 10 //Specified in datasheet
#define HALF_SPD_SOUND 171
#define MAG_SHIFT 10000
#define PULSE_TIMEOUT 5000UL //Timeout in us. Max recordable distance ~400 cm

US_Sensor* US_Sensor::ActiveSensor = nullptr;
int US_Sensor::_EchoPin = 0;

//Setup
US_Sensor::US_Sensor(int TrigPin){
  //Setting pins
  _TrigPin = TrigPin;
  pinMode(_TrigPin, OUTPUT);
}

bool US_Sensor::Setup_Echo_Pin(int EchoPin){
  _EchoPin = EchoPin;
  pinMode(_EchoPin, INPUT);
  int PinCheck = digitalPinToInterrupt(_EchoPin);
  //Interrupt pin
  if (PinCheck != NOT_AN_INTERRUPT){ // Checking if EchoPin is vaolid
    attachInterrupt(digitalPinToInterrupt(_EchoPin), Echo_ISR, CHANGE);
    return 1;
  }
  return 0;
}

//Internal interrupt functions
void US_Sensor::Echo_ISR(void) //Call interrupt with active sensor
{
    if (ActiveSensor)
        (*ActiveSensor).Handle_Interrupt();
}

void US_Sensor::Handle_Interrupt(void) {
  //Serial.println("ISR");
  //Checks for High input
  if (digitalRead(_EchoPin)) { // Same as ==HIGH
    EchoStart = micros();
  } 
  else { //If LOW set duration to the difference in time
    EchoDuration = micros() - EchoStart;
    EchoComplete = true;
  }
}

//Getting distance function
unsigned long US_Sensor::Get_Distance_CM(void){
  //Setting variables
  unsigned long SendTime;
  ActiveSensor = this; //ARM this sensor
  EchoComplete = false; 

  //Sending trig pulse
  digitalWrite(_TrigPin, LOW);
  delayMicroseconds(WRITE_DELAY);
  digitalWrite(_TrigPin, HIGH);
  delayMicroseconds(TRIG_PULSE_LENGTH);
  digitalWrite(_TrigPin, LOW);

  //Wait for echo // Might need to change so it does not wait 
  SendTime = micros();
  while (!EchoComplete) {
    if (micros() - SendTime>= PULSE_TIMEOUT) {
      //Out of range. Use timeout as echo pulse
      EchoComplete = true;
      return 0;
    }
  }

  //Converting echo pulse duration to cm
  Distance = (EchoDuration*HALF_SPD_SOUND)/MAG_SHIFT;

  return Distance;
}
