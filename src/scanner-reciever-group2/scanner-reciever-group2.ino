#define FRIMWARE_VERSION v.0.1

#include <math.h>

#define UART_BAUDRATE 9600
#define INT_PIN 2
#define SERIAL_TIMEOUT 100

#define START_MSG 255
#define END_MSG 254
#define ACK_MSG 253
#define UNREC_MSG 252

#define RANGE_DEGREES 180

#define BLINK_DELAY 700

byte Arr[180];

void setup() {
  Serial.begin(UART_BAUDRATE);
  pinMode(INT_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), ISR_Handler, RISING);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(BLINK_DELAY);
  digitalWrite(LED_BUILTIN, LOW);
  delay(BLINK_DELAY);
}

void ISR_Handler() {
  if (!Read_Serial()) {
    Serial.write(UNREC_MSG);
  }
  else {
    Serial.print("Arr: {");
    for (int i = 0;i < RANGE_DEGREES; i++){
      Serial.print(Arr[i]);
      Serial.print(",");
    }
    Serial.println("}");
  }
  // Maths on the array
}

bool Read_Serial() {
  int Index = 0;
  bool Serial_Status = true;
  byte Msg = 0;
  unsigned long t0 = millis();
  t0 = millis();
  Serial.println(t0);
  while(!Serial.available()) {
    Serial.println(millis() - t0);
    if (millis() - t0 >= SERIAL_TIMEOUT) {
      return false;
    }
  }
  Serial.println("1");
  if (Serial.read() == START_MSG) {
    while (Serial_Status) {
      t0 = millis();
      while (!Serial.available()) {
        if (millis() - t0 >= SERIAL_TIMEOUT) {
          return false;
        }
      }; // can be dangerous if gets stuck
      Msg = Serial.read();
      if (Msg == END_MSG) {
        Serial.write(ACK_MSG);
        Serial_Status = false;
        return true;
      }
      else {
        Arr[Index] = Msg;
        Index++;
      }
    }
  }
  return false;
}