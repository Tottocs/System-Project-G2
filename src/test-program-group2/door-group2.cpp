#include "include/door-group2.h"
#include <Servo.h>

#define DOOR_ANGLE 100
#define INIT_ANGLE 0
#define OPEN_STATE 1

void setDropoff(Servo &DoorServo, int Dropoff) {
  if (Dropoff == OPEN_STATE) {
    DoorServo.write(DOOR_ANGLE);  // Door DOWN
  } else {
    DoorServo.write(INIT_ANGLE);    // Door UP
  }
}