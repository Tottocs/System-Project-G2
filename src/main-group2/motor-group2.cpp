#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "v.0.1"
#endif

#include <Arduino.h>
#include "include/motor-group2.h"

#define MAIN_MOTORS_STATUS true 
#define SERIAL_STATUS false

#define MOTOR_CORRECT_PERCENT 100 

int _MOT_A1_PIN;
int _MOT_A2_PIN;
int _MOT_B1_PIN;
int _MOT_B2_PIN;

bool Initialisation = false;

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
    analogWrite(IN1_PIN, -pwm*MOTOR_CORRECT_PERCENT/100);
    digitalWrite(IN2_PIN, LOW);
  } else {
    digitalWrite(IN1_PIN, LOW);
    analogWrite(IN2_PIN, pwm*MOTOR_CORRECT_PERCENT/100);
  }
}

void Set_Motor_Currents(int pwm_A, int pwm_B) {
  Set_Motor_pwm(pwm_A, _MOT_A1_PIN, _MOT_A2_PIN);
  Set_Motor_pwm(pwm_B, _MOT_B1_PIN, _MOT_B2_PIN);
}
