#include "door-group2.h"

#define DOOR_PIN 9

int dropoff = 0;   // CHANGE THIS: 0 = UP, 1 = DOWN

Servo door(DOOR_PIN);      // Servo on pin 9

void setup() {
  door.attach(DOOR_PIN);
  door.setDropoff(dropoff);
}

void loop() {
}