#include <Wire.h>
#include "Adafruit_TCS34725.h"
// connect SCL - analog 5
// connect SDA - analog 4

/* Initialise sensor */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_614MS, 
  TCS34725_GAIN_16X
);

// PWM pin for LED
const int ledPin = 9;
const int pwmValue = 0; 

// Threshold factor (10% higher required)
const float threshold = 1.1;

void setup(void) {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, pwmValue);

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }
}

void loop(void) {
  uint32_t rTotal = 0;
  uint32_t gTotal = 0;
  uint32_t bTotal = 0;

  for (int i = 0; i < 3; i++)
  {
    uint16_t r,g,b,c;
    tcs.getRawData(&r,&g,&b,&c);

    rTotal += r;
    gTotal += g;
    bTotal += b;

    delay(50);
  }

  float rAvg = rTotal / 10.0;
  float gAvg = gTotal / 10.0;
  float bAvg = bTotal / 10.0;

  // Apply scaling
  float r_scaled = rAvg;
  float g_scaled = gAvg * 1.30;
  float b_scaled = bAvg * 1.60;

  String dominantColor = "NONE";

  // Check dominance with threshold
  if ((r_scaled > (g_scaled * threshold)) &&
      (r_scaled > (b_scaled * threshold))) {
    dominantColor = "RED";
  } 
  else if (g_scaled > r_scaled * threshold && g_scaled > b_scaled * threshold) {
    dominantColor = "GREEN";
  } 
  else if (b_scaled > r_scaled * threshold && b_scaled > g_scaled * threshold) {
    dominantColor = "BLUE";
  }

  // Print results
  Serial.print("Scaled -> ");
  Serial.print("R="); Serial.print(r_scaled);
  Serial.print(" G="); Serial.print(g_scaled);
  Serial.print(" B="); Serial.print(b_scaled);

  Serial.print(" | Dominant: ");
  Serial.println(dominantColor);

  delay(500);
}