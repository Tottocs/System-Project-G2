#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "v.0.2"
#endif
/*
Author: Torin Stanton-Andersson
*/

#include <Arduino.h>
#include "include/motor-group2.h"

#define MAIN_MOTORS_STATUS true 
#define SERIAL_STATUS false

//Correction for the left servo, taken from calibration
#define MOTOR_CORRECT_PERCENT 127 

//Macro for dealing with percentage
#define MULT_PERCENT(a,b) a*b/100

#define UPDATE_90_DELAY 500
#define UPDATE_180_DELAY 1000
#define TURN_SPD 50

int _MOT_A1_PIN;
int _MOT_A2_PIN;
int _MOT_B1_PIN;
int _MOT_B2_PIN;

bool Initialisation = false;

int Corrected_pwm_A;

// Setup pins before running motors
void Setup_Main_Motors(
  int MOT_A1_PIN, int MOT_A2_PIN, 
  int MOT_B1_PIN, int MOT_B2_PIN) {

  _MOT_A1_PIN = MOT_A1_PIN;
  _MOT_A2_PIN = MOT_A2_PIN;
  _MOT_B1_PIN = MOT_B1_PIN;
  _MOT_B2_PIN = MOT_B2_PIN;
  pinMode(MOT_A1_PIN, OUTPUT);
  pinMode(MOT_A2_PIN, OUTPUT);
  pinMode(MOT_B1_PIN, OUTPUT);
  pinMode(MOT_B2_PIN, OUTPUT);
  Initialisation = true;
}

void Set_Motor_pwm(int pwm, int IN1_PIN, int IN2_PIN) {

  if ((MAIN_MOTORS_STATUS == true) && (Initialisation == false)) {
    #if SERIAL_STATUS == true
    Serial.print("Error: Motors not initialised");
    #endif
    return;
  }

  if (pwm < 0) {
    analogWrite(IN1_PIN, -pwm);
    digitalWrite(IN2_PIN, LOW);
  } else {
    digitalWrite(IN1_PIN, LOW);
    analogWrite(IN2_PIN, pwm);
  }
}

void Set_Motor_Currents(int pwm_A, int pwm_B) {
  Corrected_pwm_A = MULT_PERCENT(pwm_A, MOTOR_CORRECT_PERCENT);
  Set_Motor_pwm(Corrected_pwm_A, _MOT_A1_PIN, _MOT_A2_PIN);
  Set_Motor_pwm(pwm_B, _MOT_B1_PIN, _MOT_B2_PIN);
}

//90 degree anticlockwise turn helper
void Turn_90_Anti_Clockwise()
{
  //left forward right backward
  Set_Motor_Currents(TURN_SPD, -TURN_SPD);
  delay(UPDATE_90_DELAY); //might need adjusted

  Set_Motor_Currents(0,0);
}

//90 degree clockwise turn helper
void Turn_90_Clockwise()
{
  //right forward left backward
  Set_Motor_Currents(-TURN_SPD, TURN_SPD);
  delay(UPDATE_90_DELAY); //might need adjusted

  Set_Motor_Currents(0,0);
}

//180 degree turn helper
void Turn_180()
{
  //left forward right backward
  Set_Motor_Currents(TURN_SPD, -TURN_SPD);
  delay(UPDATE_180_DELAY); //might need adjusted

  Set_Motor_Currents(0,0);
}