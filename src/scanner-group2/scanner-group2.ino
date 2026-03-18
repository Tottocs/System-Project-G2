#define FIRMWARE_VERSION "v.0.3"

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
#define SEND_DELAY 50
#define RECIEVE_TIMEOUT 100 
#define START_MSG 255
#define END_MSG 254
#define RECIEVED_MSG 253
#define UNRECEIVED_MSG 252

#define LEFT_TRIG_PIN 7
#define RIGHT_TRIG_PIN 8
#define ECHO_PIN 2
#define SERVO_PIN 9
#define SEND_ISR_PIN 4
#define STATUS_LED 12

#define SERVO_SLOW_DELAY 50
#define SERVO_FAST_DELAY 10
#define GET_DIST_DELAY 10

#define RANGE_DEGREES 180
#define RANGE_CM 20
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

byte DistAngle[RANGE_DEGREES];
int Distance, DistLeft, DistRight;
int Angle;
bool ObjectInSight = false;

//Main code
// --------------------------------------------------

void setup() {
  //Setup serial
  Serial.begin(BAUD_RATE);

  //Setup pins
  pinMode(SEND_ISR_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  US_Sensor::Setup_Echo_Pin(ECHO_PIN);
  rotator.attach(SERVO_PIN);
  digitalWrite(SEND_ISR_PIN, LOW);
}

void loop() {
  
  //Serial.print(ObjectInSight);
  //ObjectInSight = false;
  if (ObjectInSight == false) {
    digitalWrite(STATUS_LED, HIGH);
    ObjectInSight = Scanner(&DistAngle[0], SCAN_BASIC);
  }
  else if (ObjectInSight) {
    digitalWrite(SEND_ISR_PIN, HIGH);

    digitalWrite(STATUS_LED, LOW);
    delay(SEND_DELAY);
    Serial.write(START_MSG);
    Scanner(&DistAngle[0], SCAN_ADV);
    Serial.write(END_MSG);
    if (!Wait_For_Ack(RECIEVE_TIMEOUT)) {
      Send_Array();
    }
    else {
      Serial.println("ObjectInSight not defined");
    }
    digitalWrite(SEND_ISR_PIN, LOW);
  }
}

//Supplementary functions
// ----------------------------------------------------

int Scanner(byte* PtrDistAngle, int ScanType) {
  static int Basic = 0;
  static int Advanced = 1;

  rotator.write(ANGLE_START);
  delay(SERVO_SLOW_DELAY);

  bool InRangeLeft = false;
  bool InRangeRight = false;
  
  //Loop through every angle and add it to array
  for (int i = ANGLE_START; i < (RANGE_DEGREES*2); i++){ 

    //Set the angle of servo
    int Angle = (i < RANGE_DEGREES) ? i : FULL_ROT-i;
    rotator.write(Angle);
    delay(SERVO_FAST_DELAY);

    //Get distance from both us sensors
    DistLeft = (int)us_left.Get_Distance_CM();
    delay(GET_DIST_DELAY);
    DistRight = (int)us_right.Get_Distance_CM();

    //If set to basic scanning:
    InRangeLeft = ((DistLeft <= RANGE_CM) && (DistLeft != 0));
    InRangeRight = ((DistRight <= RANGE_CM) && (DistRight != 0));
    //Serial.println(InRangeRight);
    if (ScanType == Basic && (InRangeLeft || InRangeRight)) {
      ObjectInSight = true;
      //Serial.print("Distance: ");
      //Serial.println(DistRight);    
      return ObjectInSight;
    }

    //If set to advanced scanning
    if (ScanType == Advanced) {
      // Detect if an object is seen by both us sensors and is in range
      ObjectInSight =   (abs(DistLeft-DistRight) <= DISTANCE_PRECISION &&
                        max(DistLeft,DistRight) <= RANGE_CM && 
                        min(DistLeft,DistRight) != 0);
      Distance = (ObjectInSight) ? AVERAGE(DistLeft,DistRight) : 0;
      Serial.println(Distance);
      //Set distance in array 
      *(PtrDistAngle+Angle) = Distance;
      //Serial.println(Distance);
    }
  }
  return ObjectInSight = false;
}

bool Wait_For_Ack(unsigned long Timeout) {
  int t0 = millis();
  int Msg;
  while (millis() - t0 < Timeout){
    
    if (Serial.available() > 0) {
      Msg = Serial.read();
      if (Msg == RECIEVED_MSG) return true;
      else if (Msg == UNRECEIVED_MSG) return false;
    }
  }
  return false;
}

void Send_Array() {
  for (int i = ANGLE_START; i < RANGE_DEGREES; i++) {
    Serial.write(DistAngle[i]);
  }
  Serial.write(END_MSG);
  return;
}
