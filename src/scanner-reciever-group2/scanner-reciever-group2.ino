#define FRIMWARE_VERSION v.0.1

#include <math.h>

#define UART_BAUDRATE 9600
#define INT_PIN 2
#define SERIAL_TIMEOUT 1000

#define START_MSG 255
#define END_MSG 254
#define ACK_MSG 253
#define UNREC_MSG 252

#define RANGE_DEGREES 180

#define BLINK_DELAY 700

//States
#define BLINK B0
#define STORE B1

byte Arr[180];
int State = B0; 


void setup() {
  Serial.begin(UART_BAUDRATE);
  pinMode(INT_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), ISR_Handler, RISING);
}

void loop() {
  // State machine
  switch (State) {
    case STORE: Store_Data();  break; 
    case BLINK: Blink();       break;
  default: 
    Blink();
  }
}

void Blink() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(BLINK_DELAY);
  digitalWrite(LED_BUILTIN, LOW);
  delay(BLINK_DELAY);
}
void ISR_Handler() {
  State = B1;
  // Maths on the array
}

void Store_Data() {
  if (!Read_Serial()) {
    Serial.println(UNREC_MSG);
  }
  else {
    Serial.print("Arr: {");
    for (int i = 0;i < RANGE_DEGREES; i++){
      Serial.print(Arr[i]);
      Serial.print(",");
    }
    Serial.println("}");
  }
  State = B0;
}

bool Read_Serial() {
  int Index = 0;
  byte Msg = 0;
  unsigned long t0 = millis();
  
  t0 = millis();
  while(true) {    
    if (Serial.available() > 0) {
      Msg = Serial.read();
      if (Msg == START_MSG) break;
    }
    if (millis() - t0 >= SERIAL_TIMEOUT) {
      Serial.println(millis() - t0);
      Serial.println("0"); 
      return false;
    }
  }
  
  while (true) {
    t0 = millis();
    while (true) {
      if (Serial.available() > 0) break;
      if (millis() - t0 >= SERIAL_TIMEOUT) {
        Serial.println("1"); 
        return false;
      }
    }
    Msg = Serial.read();
    //Serial.println(Index);
    // can be dangerous if gets stuck
    if (Msg == END_MSG) {
      Serial.write(ACK_MSG);
      return true;
    }
    if  (Index < RANGE_DEGREES) {
      Serial.print(Msg);
      Arr[Index] = Msg;
      Index++;
    }
    else {
      Serial.println("2");
      return false;
    }
  }
  
  Serial.println("3");
  return false;
}