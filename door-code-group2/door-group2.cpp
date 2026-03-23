#include "door-group2.h"
#include <Servo.h>

#define DOOR_ANGLE 100
#define INIT_ANGLE 0
#define OPEN_STATE 1

void setDropoff(int dropoff) {
  if (dropoff == OPEN_STATE) {
    myServo.write(DOOR_ANGLE);  // Door DOWN
  } else {
    myServo.write(INIT_ANGLE);    // Door UP
  }
}