#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "v.0.2" //Initial commit

/*
[] Ultrasonic Sensor
[] RGB Sensor
[x] Motors
[] Servos (Which ones)
[x] IR sensors
[] Status LEDs

Optional:
[] Eyes
[] Buzzer or sound

*/

//Including project libraries
#include "include/ultrasonic-sensor-group2.h" 
#include "include/ir-sensor-group2.h"
#include "include/motor-group2.h"

//Including online libraries 
#include <Servo.h>

//UART only needed for debugging and testing. Will slow the arduino computation
#define UART_BAUDRATE 9600 //might need to be changed

//Pins on Arduino UNO 
#define TRIG_PIN_1 8 //Needs to be changed
#define TRIG_PIN_2 8 //Needs to be changed
#define TRIG_PIN_3 8 //Needs to be changed
#define ECHO_PIN 2 //Global

#define PWM_SERVO_PIN 6 //Needs to be changed

//MAX and MIN distances for detectable object (cm)
#define MIN_DIST 15 //Needs to be changed
#define MAX_DIST 195 //Needs to be changed

//Motor pins
#define MOT_A1_PIN 5
#define MOT_A2_PIN 6
#define MOT_B1_PIN 9
#define MOT_B2_PIN 10

//Creating Servo objects
Servo arm1; // Add servo names
Servo arm2;
Servo door;
Servo scanner;

US_Sensor us_sens1(TRIG_PIN_1);

//Motor speed variables
int LeftMotorSpeed = 0;
int RightMotorSpeed = 0;
int* LeftMotor, RightMotor;

//US sensor variables
unsigned long Distance;
int IR_Sensor_Status = B000;

void setup() {
  //setup serial
  Serial.begin(UART_BAUDRATE);

  //Setup up ultrasonic sensor pins
  us_sens1.Setup_Echo_Pin(ECHO_PIN);

  //Setup motor pins
  pinMode(MOT_A1_PIN, OUTPUT);
  pinMode(MOT_A2_PIN, OUTPUT);
  pinMode(MOT_B1_PIN, OUTPUT);
  pinMode(MOT_B2_PIN, OUTPUT);
}

void loop() {
  Distance = us_sens1.Get_Distance_CM();

  Scan();
  UpdateDirection();
  spin_and_wait(leftServoSpeed, rightServoSpeed, 8);
}
