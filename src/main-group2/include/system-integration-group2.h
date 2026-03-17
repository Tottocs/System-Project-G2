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

//90degree turn
bool turned90 = false;

#define REFRESH_RATE 50 //Refresh rate of entire programme in ms

//Creating objects
Servo arm1; // Add servo names
Servo arm2;
Servo door;
Servo scanner;

US_Sensor us_sens1(TRIG_PIN_1); //front sensor
US_Sensor us_sens1(TRIG_PIN_2); //side sensor
US_Sensor us_sens1(TRIG_PIN_3); //back sensor

//Motor speed variables
int LeftMotorSpeed = 0;
int RightMotorSpeed = 0;

//US sensor variables
unsigned long DistanceFront; //front sensor 
unsigned long DistanceSide; //side sensor
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
  PICKUP = 2,
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
  us_sens1.Setup_Echo_Pin(ECHO_PIN);
  Setup_IR_Sensors(IR_Sensor_Pins, BUTTON);

  //Setup main motor pins
  Setup_Main_Motors(MOT_A1_PIN, MOT_A2_PIN, 
                    MOT_B1_PIN, MOT_B2_PIN);

  //Setup calibration
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {

  DistanceFront = us_sens1.Get_Distance_CM();
  DistanceSide = us_sens2.Get_Distance_CM();
  DistanceBack = us_sens3.Get_Distance_CM();

  IR_Sensor_Status = Scan();
  // USE IR_Sensor_Status to create other code
  Update_Direction(&LeftMotorSpeed, &RightMotorSpeed);
  Set_Motor_Currents(LeftMotorSpeed, RightMotorSpeed);

  switch(CurrentState)
  {

    case STARTING:
      Starting();
    break;

    case PICKUP:
      Pickup();
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

    case FINSHED:
      Finished();
    break;

    default:
      CurrentState = ERROR;
  }


  delay(REFRESH_RATE);
}

//STATE 1
void Starting()
{
  //Driving foward to reach line
  if(IR_Sensor_Status == B000)
  {
    Set_Motor_Currents(80,80);
    return;
  }

  //start line following
  Update_Direction(&LeftMotorSpeed,&RightMotorSpeed);
  Set_Motor_Currents(LeftMotorSpeed,RightMotorSpeed);

  //once line is in middle of sensors wsitch to next state
  if(IR_Sensor_Status == B010)
  {
    CurrentState = PICKUP;
  }

}

//STATE 2
void Pickup()
{

  if(Distance > MIN_DIST && DistanceFront < MAX_DIST)
  {
    CurrentState = OBSTACLE;
    return;
  }

  Update_Direction(&LeftMotorSpeed,&RightMotorSpeed);
  Set_Motor_Currents(LeftMotorSpeed,RightMotorSpeed);

  if(ObjectDetected())
  {
    PickupObject();
    pickup++;
  }

  if(pickup >= 3)
  {
    CurrentState = LINE_DROPOFF;
  }

}


//STATE 3 drop off 
void Dropoff()
{

  Update_Direction(&LeftMotorSpeed,&RightMotorSpeed);
  Set_Motor_Currents(LeftMotorSpeed,RightMotorSpeed);

  if(DropPointDetected())
  {

    ReverseToBin();

    BinsVisited++;

    if(BinsVisited >= 3)
    {
      CurrentState = HOME;
    }

  }

}


//STATE 4 - go home
void Home()
{

  Update_Direction(&LeftMotorSpeed,&RightMotorSpeed);
  Set_Motor_Currents(LeftMotorSpeed,RightMotorSpeed);

  if(IR_Sensor_Status == B111)
  {

    Set_Motor_Currents(80,80);

    Turn180();


  }

}


//STATE 5 - avoid obstacle
void Obstacle()
{

  Beep(2);

  delay(3000);

  if(Distance > MIN_DIST && Distance < MAX_DIST)
  {
    Beep(3);
    FollowObstacleEdge();
  }

  IR_Sensor_Status = Scan();

  if(IR_Sensor_Status != B000)
  {
    CurrentState = PICKUP;
  }

}

//STATE 6 FINISHED
void FINISHED()
{
  Set_Motor_Currents(0,0);
}
break;








































