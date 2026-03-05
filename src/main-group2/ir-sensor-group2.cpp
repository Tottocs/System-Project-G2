#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "v.0.1"

/*
Author: Lucy Grierson
Edited By: Torin Stanton-Andersson 

*/
#include <Arduino.h>
#include "include/ir-sensor-group2.h"

#define N_SENSORS 3 //DO NOT EDIT

#define MAX_SPD 180 //Editable
#define THRESHOLD 500

//sensors
int IRPins[N_SENSORS];

int IRSensorDigital[N_SENSORS] = {0, 0, 0};
int irSensorRaw[N_SENSORS] = {0, 0, 0};

int Threshold = 500;
int IR_Sensors = B000;
int Error = 0;
int ErrorLast = 0;

void Setup_IR_Sensors(int arr[N_SENSORS]) { // {Left, Middle, Right}
  for (int i=0; i<N_SENSORS; i++) {IRPins[i] = arr[i]}
  // Add in calibaration for THRESHOLD

}

//sensor scanning
int Scan() {
  if 

  IRSensors = B000;

  for (int i = 0; i < N_SENSORS; i++) {
    irSensorRaw[i] = analogRead(irPins[i]);
    IRSensorDigital[i] = (irSensorRaw[i] >= THRESHOLD) ? 1 : 0;

    int b = N_SENSORS - 1 - i;
    IRSensors |= (IRSensorDigital[i] << b);
  }

  /*
  Serial.print("IR raw: L=");
  Serial.print(irSensorRaw[0]);
  Serial.print(" C=");
  Serial.print(irSensorRaw[1]);
  Serial.print(" R=");
  Serial.print(irSensorRaw[2]);

  Serial.print(" | dig: ");
  Serial.print(irSensorDigital[0]);
  Serial.print(" ");
  Serial.print(irSensorDigital[1]);
  Serial.print(" ");
  Serial.print(irSensorDigital[2]);

  Serial.print(" | bin=");
  Serial.print(IRSensors, BIN);

  Serial.print(" | err=");
  Serial.println(Error);
  */

  return IRSensors;
}

//line following
void UpdateDirection() {

  ErrorLast = Error;

  switch (IRSensors) {

    case B000:  
      //off track
      if (ErrorLast < 0) {
        //off track to left
        LeftMotorSpeed  = -120;
        RightMotorSpeed = 120;
      } 
      else {
        //off track to right
        LeftMotorSpeed  = 120;
        RightMotorSpeed = -120;
      }
      return;

    case B100: Error = -80; break;
    case B110: Error = -40; break;
    case B010: Error = 0;   break;
    case B011: Error = 40;  break;
    case B001: Error = 80;  break;
    case B111: Error = 1;   break;
    default:   Error = ErrorLast;
  }

  //smoothing turning
  if (Error >= 0) {
    LeftMotorSpeed = MAX_SPD;
    RightMotorSpeed = MAX_SPD - Error;
  } else {
    LeftMotorSpeed = MAX_SPD + Error;
    RightMotorSpeed = MAX_SPD;
  }
}