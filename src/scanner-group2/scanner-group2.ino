#define FIRMWARE_VERSION "v.0.1"

/*
Author: Torin Stanton-Andersson

Description:

[x] Detect object
[x] Store distance in array
[] Determine size of object
[] Ignore objects above 50cm

*/

#include <stdio.h>
#include <stdint.h>
#include <Servo.h>
#include "include\ultrasonic-sensor-group2.h"

#define BAUD_RATE 9600

#define LEFT_TRIG_PIN 7
#define RIGHT_TRIG_PIN 8
#define ECHO_PIN 0
#define SERVO_PIN 9

#define SERVO_DELAY 50
#define GET_DIST_DELAY 1

#define RANGE_DEGREES 180
#define RANGE_CM 50
#define ANGLE_START 1
#define DISTANCE_PRECISION 3

#define AVERAGE(a,b) (a+b)/2

// Setup objects
Servo rotator;
US_Sensor us_left(LEFT_TRIG_PIN);
US_Sensor us_right(RIGHT_TRIG_PIN);

int DistAngle[RANGE_DEGREES];
int Distance, DistLeft, DistRight;
int Angle;
bool ObjectInSight;

void setup() {
  //Serial.begin(BAUD_RATE);
  US_Sensor::Setup_Echo_Pin(ECHO_PIN);
  rotator.attach(SERVO_PIN);
  //Serial.print(sizeof(DistAngle)/sizeof(DistAngle[0]));
}

void loop() {

  for (Angle = ANGLE_START; Angle <= RANGE_DEGREES; Angle++){ 
    //Set the angle of 
    rotator.write(Angle);
    delay(SERVO_DELAY);

    //Get distance from both us sensors
    DistLeft = (int)us_left.Get_Distance_CM();
    delay(GET_DIST_DELAY);
    DistRight = (int)us_right.Get_Distance_CM();
    
    // Detect if an object is seen by both us sensors and is in range
    ObjectInSight =   (abs(DistLeft-DistRight)<=DISTANCE_PRECISION &&
                      max(DistLeft,DistRight)<=RANGE_CM && !0);
    Distance = (ObjectInSight) ? AVERAGE(DistLeft,DistRight) : 0;
    
    //Set distance in array 
    DistAngle[Angle-1] = Distance;
  }
}