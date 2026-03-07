#include <Wire.h>
#include "Adafruit_TCS34725.h"

// create an instance of the TCS34725 sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);

int readingCount = 0;
String lastDetectedColour = "";

void setup() {
  Serial.begin(9600);

  if (tcs.begin()) {
    Serial.println("Found TCS34725 sensor");  
  } else{
    Serial.println("No TCS34725 found ... check your connections");
    while (1); //halt the program
  }
}

void loop() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c); 

  String detectedColour = "";

  if (c > 50) {
    float sum = c;
    float red = r / sum;
    float green = g / sum;
    float blue = b / sum; 

    float rgRatio = red / green;
    float grRatio = green / red;

    if (blue < 0.20 && rgRatio > 0.85 && rgRatio < 1.15) {
      detectedColour = "YELLOW";
    }
    else if (blue < 0.20 && rgRatio >= 1.15 && rgRatio < 1.8) {
      detectedColour = "ORANGE";
    }
    else if (blue < 0.20 && rgRatio >= 1.8) {
      detectedColour = "RED";
    }
    else if (blue < 0.30 && grRatio >= 1.2) {
      detectedColour = "GREEN";
    }
    else {
      detectedColour = "UNKNOWN";
    }
  } else {
    detectedColour = "NO COLOUR";
  }

  // Count readings
  readingCount++;
  lastDetectedColour = detectedColour;

  // Only print after 3 readings
  if (readingCount >= 3) {
    Serial.print("Detected Colour after 3 readings: ");
    Serial.println(lastDetectedColour);
    readingCount = 0;  // reset counter
  }

  delay(500);
}
