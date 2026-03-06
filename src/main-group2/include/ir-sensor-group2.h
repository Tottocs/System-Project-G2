#ifndef IR_SENSOR_GROUP2_H
#define IR_SENSOR_GROUP2_H

#define N_SENSORS 3

void Setup_IR_Sensors(int Arr[N_SENSORS]);
void Update_Direction(int* LeftMotorSpeed, int* RightMotorSpeed);
int Scan();

#endif