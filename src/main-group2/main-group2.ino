#define FIRMWARE_VERSION "v.0.1" //Initial commit

/*
[] Ultrasonic Sensor
[] RGB Sensor
[] Motors
[] Servos (Which ones)
[] IR sensors
[] Status LEDs

Optional:
[] Eyes
[] Buzzer or sound

*/

#include "include/ultrasonic-sensor-group2.h" //Including custom library
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

//Creating Servo objects
Servo arm1; // Add servo names
Servo arm2;
Servo door;
Servo scanner;

US_Sensor us_sens1(TRIG_PIN_1);

//Variables
unsigned long Distance;

void setup() {
  //Seting up ultrasonic sensor pins
  us_sens1.Setup_Echo_Pin(ECHO_PIN);
}

void loop() {
  Distance = us_sens1.Get_Distance_CM();

}
