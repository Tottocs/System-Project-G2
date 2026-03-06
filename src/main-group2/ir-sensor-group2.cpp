#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "v.0.1"
#endif

/*
Author: Lucy Grierson
Edited By: Torin Stanton-Andersson 

*/
#include <Arduino.h>
#include "include/ir-sensor-group2.h"

#define SERIAL_STATUS false

#ifndef N_SENSORS
#define N_SENSORS 3 //DO NOT EDIT
#endif

#define MAX_SPD 150 //Editable default = 180
#define BLACK_MARGIN_SHIFT 100
#define CAL_TIMEOUT 10000

#define SMALL_CHANGE 40
#define LARGE_CHANGE 80

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
  // Add checking for setup
  IR_Sensors = B000;

  for (int i = 0; i < N_SENSORS; i++) {
    IRSensorRaw[i] = analogRead(IRPins[i]);
    IRSensorDigital[i] = (IRSensorRaw[i] >= Threshold) ? 1 : 0;

    int b = (int)N_SENSORS - 1 - i;
    IR_Sensors |= (IRSensorDigital[i] << b);
  }

  // CANNOT USE SERIAL SEPARATE FROM INO FILE
  #if SERIAL_STATUS == true
  Serial.print("IR raw: L=");
  Serial.print(IRSensorRaw[0]);
  Serial.print(" C=");
  Serial.print(IRSensorRaw[1]);
  Serial.print(" R=");
  Serial.print(IRSensorRaw[2]);

  Serial.print(" | dig: ");
  Serial.print(IRSensorDigital[0]);
  Serial.print(" ");
  Serial.print(IRSensorDigital[1]);
  Serial.print(" ");
  Serial.print(IRSensorDigital[2]);

  Serial.print(" | bin=");
  Serial.print(IR_Sensors, BIN);

  Serial.print(" | err=");
  Serial.println(Error);  
  #endif
  

  return IR_Sensors;
}

//line following
void Update_Direction(int* LeftMotorSpeed,int* RightMotorSpeed) {
  
  ErrorLast = Error;

  switch  (IR_Sensors) {

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

    case B100: Error = -LARGE_CHANGE; break; //Left sharp
    case B110: Error = -SMALL_CHANGE; break; //Left soft
    case B010: Error = 0;             break;
    case B011: Error = SMALL_CHANGE;  break; //Right soft
    case B001: Error = LARGE_CHANGE;  break; //Right sharp
    case B111: Error = 1;             break;
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
  return;
}