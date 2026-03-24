#ifndef RGB_SENSOR_H
#define RGB_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

class RgbSensor {
public:
    RgbSensor();

    int begin();
    char detectDominantColor();    
    //void update();

private:
    // Sensor
    Adafruit_TCS34725 tcs;

    // Constants
    static const int SAMPLE_COUNT = 3;
    static const int SAMPLE_DELAY_MS = 50;
    static const int LOOP_DELAY_MS = 500;

    static constexpr float THRESHOLD_FACTOR = 1.1;

    static constexpr float R_SCALING_FACTOR = 1.0;
    static constexpr float G_SCALING_FACTOR = 1.30;
    static constexpr float B_SCALING_FACTOR = 1.60;

    // Internal helpers
    void readAverages(float &rAvg, float &gAvg, float &bAvg);
};

#endif