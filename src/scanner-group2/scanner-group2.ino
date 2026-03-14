#define FIRMWARE_VERSION "v.0.2"

/*
Author: Torin Stanton-Andersson

Description:
Used on slave board (this case: Arduino Mini)
Scans for object until it detects something. 
If object is seen, it sends a signal to master board.
Afterwards it scans object to detect how big it is and 
if object < 5cm big, sends message to pick it up.

[x] Detect object
[x] Store distance in array
[] Determine size of object
[x] Ignore objects above 50cm

*/

//#include <stdio.h>
//#include <stdint.h>
#include <Servo.h>
#include "include\ultrasonic-sensor-group2.h"

#define BAUD_RATE 9600
#define SERIAL_DELAY 50
#define SEND_MSG 255
#define RECIEVED_MSG 254


#define LEFT_TRIG_PIN 7
#define RIGHT_TRIG_PIN 8
#define ECHO_PIN 2
#define SERVO_PIN 9
#define SEND_ISR_PIN 4

#define SERVO_SLOW_DELAY 50
#define SERVO_FAST_DELAY 10
#define GET_DIST_DELAY 1

#define RANGE_DEGREES 180
#define RANGE_CM 50
#define ANGLE_START 0
#define DISTANCE_PRECISION 3
#define FULL_ROT (RANGE_DEGREES*2)-1

#define SCAN_BASIC 0
#define SCAN_ADV 1

//Example of using macro (not the best use)
#define AVERAGE(a,b) (a+b)/2

//Setup objects
Servo rotator;
US_Sensor us_left(LEFT_TRIG_PIN);
US_Sensor us_right(RIGHT_TRIG_PIN);

int DistAngle[RANGE_DEGREES];
int Distance, DistLeft, DistRight;
int Angle;
bool ObjectInSight;

//Main code
// --------------------------------------------------

void setup() {
  //Setup serial
  Serial.begin(BAUD_RATE);
  Serial1.begin(BAUD_RATE);
  //Setup pins
  pinMode(SEND_ISR_PIN, OUTPUT);
  US_Sensor::Setup_Echo_Pin(ECHO_PIN);
  rotator.attach(SERVO_PIN);
}

void loop() {
  ObjectInSight == false;

  if (ObjectInSight == false) {
    ObjectInSight = Scanner(&DistAngle[0], SCAN_BASIC);
  }
  else {
    digitalWrite(SEND_ISR_PIN, HIGH;)
    delay(SERIAL_DELAY)
    Serial1.write(SEND_MSG)
    ObjectInSight = Scanner(&DistAngle[0], SCAN_ADV);
    while (!Serial1.available());
    if Serial.read(RECIEVED_MSG);
    }
  }
}

//Supplementary functions
// ----------------------------------------------------

int Scanner(int* PtrDistAngle, int ScanType) {
  static int Basic = 0;
  static int Advanced = 1;

  rotator.write(0);
  delay(SERVO_SLOW_DELAY);
  //Loop through every angle and add it to array
  for (i = ANGLE_START; i < (RANGE_DEGREES*2); i++){ 
    //Set the angle of servo
    int Angle = (i < RANGE_DEGREES) ? i : FULL_ROT-i;
    rotator.write(Angle);
    delay(SERVO_FAST_DELAY);

    //Get distance from both us sensors
    DistLeft = (int)us_left.Get_Distance_CM();
    delay(GET_DIST_DELAY);
    DistRight = (int)us_right.Get_Distance_CM();
    
    //If set to basic scanning:
    if (ScanType == Basic && DistLeft <= RANGE_CM && DistRight <= RANGE_CM) {
      ObjectInSight = true;    
      return ObjectInSight;
    }
    //If set to advanced scanning
    if (ScanType == Advanced) {
      // Detect if an object is seen by both us sensors and is in range
      ObjectInSight =   (abs(DistLeft-DistRight) <= DISTANCE_PRECISION &&
                        max(DistLeft,DistRight) <= RANGE_CM && 
                        min(DistLeft,DistRight) != 0);
      Distance = (ObjectInSight) ? AVERAGE(DistLeft,DistRight) : 0;
      
      //Set distance in array 
      *(PtrDistAngle+Angle) = Distance;
      Serial1.write("<");
      Serial1.write(Distance);
      Serial1.write(">");
    }
  }
  return ObjectInSight;
}


