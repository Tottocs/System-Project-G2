#include "inlcude/rgb-sensor-group2.h"

#define SERIAL_BAUD_RATE 9600
#define LED_PIN 9
#define LED_PWM_VALUE 0

RgbSensor sensor;

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    pinMode(LED_PIN, OUTPUT);
    analogWrite(LED_PIN, LED_PWM_VALUE);

    sensor.begin();
}

void loop() {
    sensor.detectDominantColor();
}