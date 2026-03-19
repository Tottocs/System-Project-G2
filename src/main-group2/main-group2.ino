#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "v.0.3" //Implementation of line following
#endif

/*
[] Ultrasonic Sensor
[] RGB Sensor
[x] Motors
[] Servos (Which ones)
[x] IR sensors
[] Status LEDs
[] Buzzer or sound

Optional:
[] Eyes

*/

//Including project libraries
#include "include/ultrasonic-sensor-group2.h" 
#include "include/ir-sensor-group2.h"
#include "include/motor-group2.h"

//Including online libraries 
#include <Servo.h>

//UART only needed for debugging and testing. Will slow the arduino computation
#define SERIAL_STATUS false // set false for field operation
#define UART_BAUDRATE 9600 //might need to be changed

//Pins on Arduino UNO 
#define TRIG_PIN_1 8 //Needs to be changed
#define TRIG_PIN_2 8 //Needs to be changed
#define TRIG_PIN_3 8 //Needs to be changed
#define ECHO_PIN 2 //Global

//Servo pins
#define PWM_SERVO_PIN 6 //Needs to be changed

//Motor pins
#define MOT_A1_PIN 5
#define MOT_A2_PIN 6
#define MOT_B1_PIN 9
#define MOT_B2_PIN 10

//Peripheral 
#define BUTTON 7 // Change

int IR_Sensor_Pins[] = {A5,A4,A3};

//MAX and MIN distances for detectable object (cm)
#define MIN_DIST 1 //Needs to be changed
#define MAX_DIST 20 //Needs to be changed

//Delays in ms
#define START_DELAY 1000
#define BASE_DELAY 400

#define REFRESH_RATE 50 //Refresh rate of entire programme in ms

//
int visitedBins = 0;

//state 3
int dropOff = 0;

//90 degree turn checker
bool turned90 = false;

//180 degree turn checker
bool turned180 = false;

//Creating objects
Servo arm1; // Add servo names
Servo arm2;
Servo door;
Servo scanner;

//state 5
int obstacleStage = 0;

//Motor speed variables
int LeftMotorSpeed = 0;
int RightMotorSpeed = 0;

//US sensor variables
unsigned long DistanceFront; //front sensor 
unsigned long
DistanceRight; //side sensor
unsigned long DistanceBack; //back sensor
//IR Sensor variable
int IR_Sensor_Status = B000;

//pickup and dropoff
int pickup = 0;
int dropoff = 0;

//states
enum RobotState {
  ERROR = 0,
  STARTING = 1,
  FOLLOW_LINE = 2,
  DROPOFF = 3,
  HOME = 4,
  OBSTACLE = 5,
  FINISHED = 6
};

RobotState CurrentState = STARTING;

void setup() {
  //setup serial
  #if SERIAL_STATUS == true 
  Serial.begin(UART_BAUDRATE); //Only used for debugging
  #endif
  //Button for calibration
  pinMode(BUTTON, INPUT_PULLUP);

  //Setup up ultrasonic sensor pins
  US_Sensor::Setup_Echo_Pin(ECHO_PIN);
  Setup_IR_Sensors(IR_Sensor_Pins, BUTTON);

  //Setup main motor pins
  Setup_Main_Motors(MOT_A1_PIN, MOT_A2_PIN, 
                    MOT_B1_PIN, MOT_B2_PIN);

  //Setup calibration
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  IR_Sensor_Status = Scan();

  switch(CurrentState)
  {

    case STARTING:
      Starting();
    break;

    case FOLLOW_LINE:
      FollowLine();
    break;

    case DROPOFF:
      Dropoff();
    break;

    case HOME:
      Home();
    break;

    case OBSTACLE:
      Obstacle();
    break;


    default:
      CurrentState = ERROR;
  }
  delay(REFRESH_RATE); //might get rid
}

//STATE 1 go from charging point to line  
void Starting()
{
  Set_Motor_Currents(60,60);
  delay(400);
  //once one 90 degree turn has been made, switch to line following with pickup mode
  if(IR_Sensor_Status != B111)
  {
    CurrentState = PICKUP;
  }
}

//STATE 2 line following and pickup
void FollowLine()
{

  if(DistanceFront > MIN_DIST && DistanceFront < MAX_DIST)
  {
    CurrentState = OBSTACLE;
    return;
  }

  Update_Direction(&LeftMotorSpeed,&RightMotorSpeed);
  Set_Motor_Currents(LeftMotorSpeed,RightMotorSpeed);

  if(ObjectDetected())
  {
    PickupObject();
    //pickup code here or call helper ^
    pickup++;
  }

  if(pickup >= 3)
  {
    CurrentState = DROPOFF;
  }

}


//STATE 3 drop off 

void Dropoff()
{

  switch(dropoff)
  {

    // delay until drop off point is detected
    case 0:

      if((IR_Sensor_Status == B111) && (DistanceRight > MIN_DIST && DistanceRight < MAX_DIST))
      {
        Set_Motor_Currents(40,40);
        delay(1500);
        dropoff = 1;
      }

    break;

    //reverse turning
    case 1:

      Turn_90_Anti_Clockwise();
      dropoff = 2;

    break;

    //reverse until 5cm away
    case 2:

      Set_Motor_Currents(-120, -120);

      if(DistanceBack <= 5) //change this for distance
      {
        Set_Motor_Currents(0,0);
        dropoff = 3;
      }

    break;

    //drop objects
    case 3:
    DropOffObject();
    //add dropoff code here or call helper ^
    
    
    dropoff = 4;
      
    break;

    //move forward
    case 4:

      Set_Motor_Currents(120,120);
      delay(300);

      dropoff = 5;

    break;

    //go back to line following
    case 5:

      Update_Direction(&LeftMotorSpeed,&RightMotorSpeed);
      Set_Motor_Currents(LeftMotorSpeed,RightMotorSpeed);

      if(IR_Sensor_Status != B000)
      {
        dropoff = 0;
        visitedBins++;

        if(visitedBins >= 3)
        {
          CurrentState = HOME;
        }
      }

    break;
  }
}


//STATE 4 go home
void Home()
{

  //if buggy has reacherd thick line, 180 then stop
  if(IR_Sensor_Status == B111)
  {
    Set_Motor_Currents(0,0);
    Turn180();
    while(1); // stop forever
  }
 
}


//STATE 5 avoid obstacle
void Obstacle()
{

  switch(obstacleStage)
  {

    //obstacle detected
    case 0:

      if(DistanceFront > MIN_DIST && DistanceFront < MAX_DIST)
      {
        Set_Motor_Currents(0,0);
        Beep(2); //................................................................. need to make beep function
        delay(2000);
        Beep(3);
        Set_Motor_Currents(-40,-40);
        delay(2000);

        Turn_90_Anticlockwise(); // face alongside object
        obstacleStage = 1;
      }

    break;

    //following side of obstacle
    case 1:

      //wall following
      //if obstacle within range
      if(DistanceRight > 8 && DistanceRight < 12)
{
  Set_Motor_Currents(50,50);
}
else if(DistanceRight < 8) //turn left too close
{
  Set_Motor_Currents(50,80);
}
else if(DistanceRight > 12 && DistanceRight < 20) //turn right too far
{
  Set_Motor_Currents(80,50);
}
else if(DistanceRight > MAX_DIST)//no obstacle detected go back to line following
{
  Turn_90_Clockwise();
  obstacleStage = 2;
}
    break;

    //go back to line following
    case 2:

      //ignore ultrasonic sensors
      Update_Direction(&LeftMotorSpeed,&RightMotorSpeed);
      Set_Motor_Currents(LeftMotorSpeed,RightMotorSpeed);

      //if line is detected go abck to pickup state
      if(IR_Sensor_Status != B000)
      {
        obstacleStage = 0; // reset
        CurrentState = PICKUP;
      }

    break;
  }
}
