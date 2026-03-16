#define FRIMWARE_VERSION v.0.1

#include <math.h>

#define UART_BAUDRATE 9600
#define INT_PIN 2

#define START_MSG 255
#define END_MSG 254
#define ACK_MSG 253

#define RANGE_DEGREES 180

byte Arr[];

void setup() {
  Serial.begin(UART_BAUDRATE);
  pinMode(INT_PIN, INPUT);
  pinMode(LED_BUILT_IN, OUTPUT);
  attachInterrupt(dgitalToInterrupt(INT_PIN),ISR_Handler,HIGH);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWite(LED_BUILTIN, LOW);
  delay(100);
}

void ISR_Handler() {
  Read_Serial();
  // Maths on the array
}

byte Read_Serial() {
  int Index;
  byte Msg = 0;
  if (Serial.Available() && Serial.read() == START_MSG) {
    while (true) {
      while (!Serial.available()); // can be dangerous if gets stuck
      Msg = Serial.read();
      if (Msg == END_MSG) {
        Serial.write(ACK_MSG);
        return true;
      }
      else {
        Arr[Index] = Msg;
        Index++;
      }
    }
  }
}