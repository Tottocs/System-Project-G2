#ifndef IR_SENSOR_GROUP2_H
#define IR_SENSOR_GROUP2_H


#ifndef N_SENSORS
#define N_SENSORS 3
#endif

void Setup_IR_Sensors(int arr[N_SENSORS]);
int Update_Direction(int* LeftMotorSpeed, int* RightMotorSpeed);
int Scan();

#endif