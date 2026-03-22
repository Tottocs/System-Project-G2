#include "include/rgb-sensor-group2.h" 

#define SERIAL_STATUS false

RgbSensor::RgbSensor() 
  : tcs(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_16X) {}

int RgbSensor::begin() {
    if (tcs.begin()) {
        #if SERIAL_STATUS == true 
        Serial.println("Found sensor");
        #endif
        return (1);
    } else {
        #if SERIAL_STATUS == true 
        Serial.println("No TCS34725 found ... check your connections");
        #endif
        return (0);
    }
}

void RgbSensor::readAverages(float &rAvg, float &gAvg, float &bAvg) {
    uint32_t rTotal = 0;
    uint32_t gTotal = 0;
    uint32_t bTotal = 0;

    for (int i = 0; i < SAMPLE_COUNT; i++) {
        uint16_t r, g, b, c;
        tcs.getRawData(&r, &g, &b, &c);

        rTotal += r;
        gTotal += g;
        bTotal += b;

        delay(SAMPLE_DELAY_MS);
    }

    rAvg = rTotal / (float)SAMPLE_COUNT;
    gAvg = gTotal / (float)SAMPLE_COUNT;
    bAvg = bTotal / (float)SAMPLE_COUNT;
}

char RgbSensor::detectDominantColor() {
    float rAvg, gAvg, bAvg;
    readAverages(rAvg, gAvg, bAvg);
    float r_scaled = rAvg * R_SCALING_FACTOR;
    float g_scaled = gAvg * G_SCALING_FACTOR;
    float b_scaled = bAvg * B_SCALING_FACTOR;
    if ((r_scaled > g_scaled * THRESHOLD_FACTOR) && (r_scaled > b_scaled * THRESHOLD_FACTOR)) {
        return "R";
    } 
    else if ((g_scaled > r_scaled * THRESHOLD_FACTOR) && (g_scaled > b_scaled * THRESHOLD_FACTOR)) {
        return "G";
    } 
    else if ((b_scaled > r_scaled * THRESHOLD_FACTOR) && (b_scaled > g_scaled * THRESHOLD_FACTOR)) {
        return "B";
    }

    return "N";
}

void RgbSensor::update() {
    float rAvg, gAvg, bAvg;
    readAverages(rAvg, gAvg, bAvg);

    float r_scaled = rAvg * R_SCALING_FACTOR;
    float g_scaled = gAvg * G_SCALING_FACTOR;
    float b_scaled = bAvg * B_SCALING_FACTOR;

    char dominantColor = detectDominantColor();

    #if SERIAL_STATUS == true
    Serial.print("Scaled -> ");
    Serial.print("R="); Serial.print(r_scaled);
    Serial.print(" G="); Serial.print(g_scaled);
    Serial.print(" B="); Serial.print(b_scaled);

    Serial.print(" | Dominant: ");
    Serial.println(dominantColor);
    #endif

    delay(LOOP_DELAY_MS);
}