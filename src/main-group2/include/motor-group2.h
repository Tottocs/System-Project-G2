#ifndef MOTOR_GROUP2_H
#define MOTOR_GROUP2_H


void Setup_Main_Motors(	
	int MOT_A1_PIN, int MOT_A2_PIN, 
	int MOT_B1_PIN, int MOT_B2_PIN);
void Set_Motor_pwm(int pwm, int IN1_PIN, int IN2_PIN);
void Set_Motor_Currents(int pwm_A, int pwm_B);
void Spin_And_Wait(int pwm_A, int pwm_B, int duration);

#endif