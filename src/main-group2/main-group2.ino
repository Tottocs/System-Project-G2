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

#define REFRESH_RATE 50 //Refresh rate of entire programme in ms

//Creating objects
Servo arm1; // Add servo names
Servo arm2;
Servo door;
Servo scanner;

US_Sensor us_sens1(TRIG_PIN_1);

//Motor speed variables
int LeftMotorSpeed = 0;
int RightMotorSpeed = 0;

//US sensor variables
unsigned long Distance;
//IR Sensor variable
int IR_Sensor_Status = B000;

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

  Distance = us_sens1.Get_Distance_CM();
  Distance = us_sens1.Get_Distance_CM();
  Distance = us_sens1.Get_Distance_CM();

  IR_Sensor_Status = Scan();
  // USE IR_Sensor_Status to create other code
  Update_Direction(&LeftMotorSpeed, &RightMotorSpeed);
  Set_Motor_Currents(LeftMotorSpeed, RightMotorSpeed);

  delay(REFRESH_RATE);
}
