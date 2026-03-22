#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "v.0.4" //Implementation of line following
#endif

/*
Authors: Lucy Grierson, Torin Stanton-Andersson

[] Ultrasonic Sensor
[] RGB Sensor
[x] Motors
[] Servos (Which ones)
[x] IR sensors
[] Status LEDs
[x] Buzzer

Optional:
[] Eyes

*/


//Including project libraries
#include "include/ultrasonic-sensor-group2.h" 
#include "include/ir-sensor-group2.h"
#include "include/motor-group2.h"

//Including online libraries 
#include <Servo.h>

//UART only needed for debugging and or comunication between chips. Will slow the arduino computation
#define SERIAL_STATUS false // set false for field operation
#define UART_BAUDRATE 9600 //might need to be changed

// PINS
//---------------------------------
//Servo pins
#define PWM_SERVO_PIN 6 //Needs to be changed

//Motor pins
#define MOT_A1_PIN 5
#define MOT_A2_PIN 6
#define MOT_B1_PIN 9
#define MOT_B2_PIN 10

#define BUZZ_PIN 9

//Peripheral 
#define BUTTON 7 // Change

//Debug LEDs
#define ERROR_LED LED_BUILTIN

int IR_Sensor_Pins[] = {A5,A4,A3};

// CONSTANTS
//------------------------------------

//MAX and MIN distances for detectable object (cm)
#define MIN_DIST 1 //Needs to be changed
#define MAX_DIST 20 //Needs to be changed

//Number of rubiish objects to pick up
#define N_OBJECTS 3

//Delays in ms
#define START_DELAY 1000
#define DRV_OFF_BLK_DEL 500
#define BASE_DELAY 400
#define DROPOFF_DELAY 1500
#define OBSTACLE_DELAY 2000
#define BLK_CONFIRM_DELAY 50

//Timout
#define BLK_LINE_TIMEOUT 500UL 
#define BLINK_LENGTH 500UL

//Buzzer
#define BUZZ_FREQ 3000
#define BUZZ_DURATION 500
#define BEEP_PAUSE_TIME 300
#define NUM_BEEPS 2

//Refresh rate of entire programme in ms
#define REFRESH_RATE 50

//Speeds
#define NOMINAL_SPD 120
#define STARTING_SPD 80
#define DROPOFF_SPD 40
#define OBSTACLE_SPD 40
#define LINE_CAL_SPD 60
 
//Distances in cm
#define DIST_TO_DROPOFF 5

//Line following
#define ALL_BLACK B111
#define NO_BLACK B000

// OBJECTS
// --------------------------------------
Servo arm; // Add servo names
Servo door;
//Servo scanner;

// VARIABLES
//---------------------------------------

//90 degree turn checker
bool turned90 = false;

//180 degree turn checker
bool turned180 = false;

//state 5
int obstacleStage = 0;

//Motor speed variables
int LeftMotorSpeed = 0;
int RightMotorSpeed = 0;

//US sensor variables
unsigned long DistanceFront; //front sensor 
unsigned long DistanceRight; //side sensor
unsigned long DistanceBack; //back sensor

//IR Sensor variable
volatile int IR_Sensor_Status = B000;

//pickup and dropoff
int pickup = 0;
int dropoff = 0;
//
int visitedBins = 0;

//state 3
int dropOff = 0;

int ObjectDetected = 0;

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

  //Setup up ir sensor pins
  if (!Setup_IR_Sensors(IR_Sensor_Pins, BUTTON)) {
    CurrentState = ERROR;
  } 

  //Setup main motor pins
  Setup_Main_Motors(MOT_A1_PIN, MOT_A2_PIN, 
                    MOT_B1_PIN, MOT_B2_PIN);

  //Setup calibration
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  if (CurrentState != ERROR) {
  CurrentState = FOLLOW_LINE;
  }
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
  //delay(REFRESH_RATE); //might get rid
}

// State functions
// -------------------------------------------------

//STATE 1 go from charging point to line  
void Starting()
{
  Set_Motor_Currents(STARTING_SPD,STARTING_SPD);
  delay(DRV_OFF_BLK_DEL);
  
  if(IR_Sensor_Status != ALL_BLACK) {
    CurrentState = FOLLOW_LINE;
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

  if (LeftMotorSpeed == 0 && RightMotorSpeed == 0) {
    Set_Motor_Currents(LeftMotorSpeed,RightMotorSpeed);
    delay(BLK_CONFIRM_DELAY);
    Calibrate_On_Line(BLK_LINE_TIMEOUT);
  }



  if(ObjectDetected)
  {
    //PickupObject();
    //pickup code here or call helper ^
    pickup++;
  }

  if(pickup >= N_OBJECTS)
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

      if((IR_Sensor_Status == ALL_BLACK) && (DistanceRight > MIN_DIST && DistanceRight < MAX_DIST))
      {
        Set_Motor_Currents(DROPOFF_SPD,DROPOFF_SPD);
        delay(DROPOFF_DELAY);
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

      Set_Motor_Currents(-NOMINAL_SPD, -NOMINAL_SPD);

      if(DistanceBack <= DIST_TO_DROPOFF) //change DIST_TO_DROPOFF this for distance
      {
        Set_Motor_Currents(0,0);
        dropoff = 3;
      }

      break;

    //drop objects
    case 3:
      //DropOffObject();
      //add dropoff code here or call helper ^
    
    
      dropoff = 4;
      
      break;

    //move forward
    case 4:

      Set_Motor_Currents(NOMINAL_SPD,NOMINAL_SPD);
      delay(300);

      dropoff = 5;

      break;

    //go back to line following
    case 5:

      Update_Direction(&LeftMotorSpeed,&RightMotorSpeed);
      Set_Motor_Currents(LeftMotorSpeed,RightMotorSpeed);

      if(IR_Sensor_Status != NO_BLACK)
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
  if(IR_Sensor_Status == ALL_BLACK)
  {
    Set_Motor_Currents(0,0);
    Turn_180();
    exit(0); // stop forever until reset or power cycled
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
        Beep(NUM_BEEPS); 
        delay(OBSTACLE_DELAY);
        Beep(NUM_BEEPS);
        Set_Motor_Currents(-OBSTACLE_SPD,-OBSTACLE_SPD);
        delay(OBSTACLE_DELAY);

        Turn_90_Anti_Clockwise(); // face alongside object
        obstacleStage = 1;
      }

      break;

    //following side of obstacle
    case 1:

      //wall following
      //if obstacle within range
      if(DistanceRight > 8 && DistanceRight < 12) //Maybe use abs()<<<<<<<<<<<<<<<<<<<<<
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
      if(IR_Sensor_Status != NO_BLACK)
      {
        obstacleStage = 0; // reset
        CurrentState = FOLLOW_LINE;
      }

      break;
  }
}

void Error() {
  Blink(ERROR_LED, BLINK_LENGTH);
}

// Supplementary functions
// --------------------------------------------------------

void Beep(int BeepNumber) {
  int i = 0;
  while (i < BeepNumber) {
    tone(BUZZ_PIN, BUZZ_FREQ, BUZZ_DURATION);
    i++;
    delay(BEEP_PAUSE_TIME);
  }
}

void Blink(int LED_Pin, unsigned long BlinkLength) {
  unsigned long t0 = millis();
  unsigned long t1 = millis();
  while (t1-t0 >= BlinkLength) {
    t1 = millis();
    digitalWrite(LED_Pin, HIGH);
    }
  digitalWrite(LED_Pin, LOW);
}

void Calibrate_On_Line(unsigned long Timeout)  {
  unsigned long t0 = millis();
  unsigned long t1 = millis();
  IR_Sensor_Status = Scan();
  if (IR_Sensor_Status != ALL_BLACK) {
    while (t1-t0 >= Timeout) {
      t1 = millis();
      Set_Motor_Currents(-LINE_CAL_SPD,-LINE_CAL_SPD);
    }
  }  
}
