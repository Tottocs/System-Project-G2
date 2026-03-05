#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "v.0.1"
#endif

/*
Author: Lucy Grierson
Edited By: Torin Stanton-Andersson 

*/
#include <Arduino.h>
#include "include/ir-sensor-group2.h"

#ifndef N_SENSORS
#define N_SENSORS 3 //DO NOT EDIT

#define MAX_SPD 180 //Editable
#define BLACK_MARGIN_SHIFT 100
#define CAL_TIMEOUT 10000

//sensors
int IRPins[N_SENSORS];

int IRSensorDigital[N_SENSORS] = {0, 0, 0};
int IRSensorRaw[N_SENSORS] = {0, 0, 0};

int Threshold = 500;
int IR_Sensors = B000;
int Error = 0;
int ErrorLast = 0;

void Setup_IR_Sensors(int Arr[N_SENSORS]/*, int Button*/) { // {Left, Middle, Right}
  for (int i = 0; i < N_SENSORS; i++) {IRPins[i] = Arr[i];}

  // Add in calibaration for Threshold. Example below
  /* 
  while (digitalRead(Button == 1) { // Normally closed switch
    delay(CAL_TIMEOUT);
    throw std::runtime_error("IR calibration timeout");
  } 
  
  for (int i = 0; i < N_SENSORS; i++) {
    IRSensorRaw[i] = analogRead(IRPins[i]);
    if (i > 0) {
      Threshold = (Threshold + IRSensorRaw[i]) / 2;
    }
    else { 
      Threshold = IRSensorRaw[i];
    }
  }

  Threshold = Threshold - (int)BLACK_MARGIN_SHIFT;
  if (Threshold < BLACK_MARGIN_SHIFT) {
    throw std::runtime_error("IR sensor threshold too low - Recalibrate please");
  }
  */
  
}

//sensor scanning
int Scan() {
  IRSensors = B000;

  for (int i = 0; i < N_SENSORS; i++) {
    IRSensorRaw[i] = analogRead(IRPins[i]);
    IRSensorDigital[i] = (IRSensorRaw[i] >= THRESHOLD) ? 1 : 0;

    int b = (int)N_SENSORS - 1 - i;
    IRSensors |= (IRSensorDigital[i] << b);
  }

  // CANNOT USE SERIAL SEPARATE FROM INO FILE
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
void Update_Direction(LeftMotor, RightMotor) {
  
  ErrorLast = Error;

  switch (IRSensors) {

    case B000:  
      //off track
      if (ErrorLast < 0) {
        //off track to left
        *LeftMotorSpeed  = -120;
        *RightMotorSpeed = 120;
      } 
      else {
        //off track to right
        *LeftMotorSpeed  = 120;
        *RightMotorSpeed = -120;
      }
      return;

    case B100: Error = -80; break; // 
    case B110: Error = -40; break;
    case B010: Error = 0;   break;
    case B011: Error = 40;  break;
    case B001: Error = 80;  break;
    case B111: Error = 1;   break;
    default:   Error = ErrorLast;
  }

  //smoothing turning
  if (Error >= 0) {
    *LeftMotorSpeed = (int)MAX_SPD;
    *RightMotorSpeed = (int)MAX_SPD - Error;
  } else {
    *LeftMotorSpeed = (int)MAX_SPD + Error;
    *RightMotorSpeed = (int)MAX_SPD;
  }
  return int *LeftMotorSpeed, int *RightMotorSpeed;
}