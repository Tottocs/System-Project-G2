#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "v.0.1"

//#include <Arduino.h>
#include "include/motor-group2.h"

void Set_Motor_pwm(int pwm, int IN1_PIN, int IN2_PIN) {
  if (pwm < 0) {
    analogWrite(IN1_PIN, -pwm);
    digitalWrite(IN2_PIN, LOW);
  } else {
    digitalWrite(IN1_PIN, LOW);
    analogWrite(IN2_PIN, pwm);
  }
}

void Set_Motor_Currents(int pwm_A, int pwm_B) {
  Set_Motor_pwm(pwm_A, MOT_A1_PIN, MOT_A2_PIN);
  Set_Motor_pwm(pwm_B, MOT_B1_PIN, MOT_B2_PIN);
}

void Spin_And_Wait(int pwm_A, int pwm_B, int duration) {
  Set_Motor_Currents(pwm_A, pwm_B);
  delay(duration);
}