#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "v.0.2"
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
#define BLACK_MARGIN_SHIFT 200
#define CAL_TIMEOUT 10000
#define CAL_NUMBER 3

#define SMALL_CHANGE 40 // Default 40
#define LARGE_CHANGE 80 // Default 80
#define OFF_TRACK 120   // Default 120

//sensors
int IRPins[N_SENSORS];

int IRSensorDigital[N_SENSORS] = {0, 0, 0};
int IRSensorRaw[N_SENSORS] = {0, 0, 0};

int Threshold[3] = {500,500,500};
int IR_Sensors = B000;
int Error = 0;
int ErrorLast = 0;
int time1, press;

bool Setup_IR_Sensors(int Arr[N_SENSORS], int ButtonPin) { // {Left, Middle, Right}
  for (int i = 0; i < N_SENSORS; i++) {IRPins[i] = Arr[i];}
  
  // Add in calibaration for Threshold. Example below
  time1 = millis();
  press = digitalRead(ButtonPin);
  while (press == HIGH) { // Normally closed switch
    //throw std::runtime_error("IR calibration timeout");
    if (CAL_TIMEOUT <= (millis() - time1)) {return 0;}
    press = digitalRead(ButtonPin);
  } 
  
  for (int i = 0; i < N_SENSORS; i++) {
    for (int j = 0; j < CAL_NUMBER; j++) {
      IRSensorRaw[i] = analogRead(IRPins[i]);
      if (j > 0) {
        Threshold[i] = (Threshold[i] + IRSensorRaw[i])/2;
      }
      else { 
        Threshold[i] = IRSensorRaw[i];
      }
      delay(10);
      }
    Threshold[i] = Threshold[i] - (int)BLACK_MARGIN_SHIFT;
  }
  /*
  if (Threshold < BLACK_MARGIN_SHIFT) {
    throw std::runtime_error("IR sensor threshold too low - Recalibrate please");
  }
  */
  return 1;
}

//sensor scanning
int Scan() {
  // Add checking for setup
  IR_Sensors = B000;

  for (int i = 0; i < N_SENSORS; i++) {
    IRSensorRaw[i] = analogRead(IRPins[i]);
    IRSensorDigital[i] = (IRSensorRaw[i] >= Threshold[i]) ? 1 : 0;

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
        *LeftMotorSpeed  = -OFF_TRACK;
        *RightMotorSpeed = OFF_TRACK;
      } 
      else {
        //off track to right
        *LeftMotorSpeed  = OFF_TRACK;
        *RightMotorSpeed = -OFF_TRACK;
      }
      return;
    case B111:
    while (true) {
      Scan();
      //right turn until line found again
      *LeftMotorSpeed = (int)MAX_SPD;
      *RightMotorSpeed = (int)-MAX_SPD;
      //stop when line found
      if(IR_Sensors == B010 || IR_Sensors == B011 || IR_Sensors == B110){
        delay(20);
        Scan();
        if(IR_Sensors == B010 || IR_Sensors == B011 || IR_Sensors == B110){
          break;
            }
        }
    }
    return;

    case B100: Error = -LARGE_CHANGE; break; //Left sharp
    case B110: Error = -SMALL_CHANGE; break; //Left soft
    case B010: Error = 0;             break;
    case B011: Error = SMALL_CHANGE;  break; //Right soft
    case B001: Error = LARGE_CHANGE;  break; //Right sharp
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
