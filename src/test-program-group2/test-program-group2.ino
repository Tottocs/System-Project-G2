
//Including project libraries
#include "include/ultrasonic-sensor-group2.h"
#include "include/ir-sensor-group2.h"
#include "include/motor-group2.h"
#include "include/rgb-sensor-group2.h"
#include "include/door-group2.h"

#include <Servo.h>

#define DOOR_OPEN_STATE 1
#define DOOR_CLOSED_STATE 0 

#define BUZZ_FREQ 3000
#define BUZZ_DURATION 500
#define BEEP_PAUSE_TIME 300
#define NUM_BEEPS 2

#define BUZZ_PIN 2
#define MAGNET_PIN 12

#define BUTTON 7

#define DOOR_PIN 9

#define CRANE_SERVO_PIN 10

RgbSensor rgbSensor;
Servo CraneServo; 
Servo door;

int Dropoff = 1;
char ObjectColour = 0;

void setup() {

  Serial.begin(9600);
  //Servo setup
  door.attach(DOOR_PIN);
  CraneServo.attach(CRANE_SERVO_PIN);
  rgbSensor.begin();

  pinMode(DOOR_PIN, OUTPUT);
  pinMode(MAGNET_PIN, OUTPUT);

  pinMode(CRANE_SERVO_PIN, OUTPUT);

  //door.write(0);
  //delay(2000);
  //door.write(100);
  
  CraneServo.write(45);
  delay(2000);
  CraneServo.write(180);
  delay(2000);
  
  tone(BUZZ_PIN, 3000);
  delay(1000);
  noTone(BUZZ_PIN);
  


}

void loop() {
  digitalWrite(MAGNET_PIN, HIGH);
  int rgb = rgbSensor.detectDominantColor();
  if (rgb == 1) {
    Serial.println("Red");
  }
  else if (rgb == 2) {
    Serial.println("Green");
  }
  else if (rgb == 3) {
    Serial.println("Blue");
  }
  else {
    Serial.println("No Object");
  }
  delay(1000);
  digitalWrite(MAGNET_PIN, LOW);
  delay(500);
}