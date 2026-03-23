#ifndef IR_SENSOR_GROUP2_H
#define IR_SENSOR_GROUP2_H

#define N_SENSORS 3

bool Setup_IR_Sensors(int Arr[N_SENSORS], int ButtonPin, int LED);
void Update_Direction(int* LeftMotorSpeed, int* RightMotorSpeed);
int Scan();
void Off_Track(int ErrorLast, int* LeftMotorSpeed,int* RightMotorSpeed);

#endif