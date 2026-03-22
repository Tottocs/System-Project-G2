#include <Wire.h>
#include "Adafruit_TCS34725.h"


#define SERIAL_BAUD_RATE 9600
#define LED_PIN 9
#define LED_PWM_VALUE 0

#define SAMPLE_COUNT 3
#define SAMPLE_DELAY_MS 50
#define LOOP_DELAY_MS 500

#define THRESHOLD_FACTOR 1.1

#define R_SCALING_FACTOR 1.0
#define G_SCALING_FACTOR 1.30
#define B_SCALING_FACTOR 1.60


Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_614MS, 
  TCS34725_GAIN_16X
);

void setup(void) {
  Serial.begin(SERIAL_BAUD_RATE);

  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, LED_PWM_VALUE);

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

  for (int i = 0; i < SAMPLE_COUNT; i++)
  {
    uint16_t r,g,b,c;
    tcs.getRawData(&r,&g,&b,&c);

    rTotal += r;
    gTotal += g;
    bTotal += b;

    delay(SAMPLE_DELAY_MS);
  }

  float rAvg = rTotal / (float)SAMPLE_COUNT; 
  float gAvg = gTotal / (float)SAMPLE_COUNT; 
  float bAvg = bTotal / (float)SAMPLE_COUNT; 

  // Apply scaling
  float r_scaled = rAvg * R_SCALING_FACTOR;
  float g_scaled = gAvg * G_SCALING_FACTOR;
  float b_scaled = bAvg * B_SCALING_FACTOR;

  String dominantColor = "NONE";

  // Check dominance with threshold
  if ((r_scaled > (g_scaled * THRESHOLD_FACTOR)) &&
      (r_scaled > (b_scaled * THRESHOLD_FACTOR))) {
    dominantColor = "RED";
  } 
  else if (g_scaled > r_scaled * THRESHOLD_FACTOR && g_scaled > b_scaled * THRESHOLD_FACTOR) {
    dominantColor = "GREEN";
  } 
  else if (b_scaled > r_scaled * THRESHOLD_FACTOR && b_scaled > g_scaled * THRESHOLD_FACTOR) {
    dominantColor = "BLUE";
  }

  // Print results
  Serial.print("Scaled -> ");
  Serial.print("R="); Serial.print(r_scaled);
  Serial.print(" G="); Serial.print(g_scaled);
  Serial.print(" B="); Serial.print(b_scaled);

  Serial.print(" | Dominant: ");
  Serial.println(dominantColor);

  delay(LOOP_DELAY_MS);
}