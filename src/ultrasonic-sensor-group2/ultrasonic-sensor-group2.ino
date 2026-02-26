#define FIRMWARE_VERSION "v.0.2"

#define UART_BAUDRATE 9600 //At which speed the information is sent through the Arduino

#define ECHO_PIN 2 //Echo pin from ultrasonic sensor
#define TRIG_PIN 7 //Trig pin to ultrasonic sensor
#define TRIG_PULSE_LENGTH 10
#define PULSE_IN_TIMEOUT 30000UL

//#define USE_FLOAT


#ifdef USE_FLOAT
float PulseDuration;
float Distance;

#else
unsigned long PulseDuration;
unsigned long Distance;

#endif

void setup() {
  //Initiate serial
  Serial.begin(UART_BAUDRATE);
  //Initiate pins
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
}

void loop() {
  //Sending 10us trig pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(TRIG_PULSE_LENGTH);
  digitalWrite(TRIG_PIN, LOW);

  //Recieving echo pulse
  #ifdef USE_FLOAT
  PulseDuration = (float)pulseIn(ECHO_PIN, HIGH, PULSE_IN_TIMEOUT);
  #else
  PulseDuration = pulseIn(ECHO_PIN, HIGH, PULSE_IN_TIMEOUT);
  #endif
  //Distance = (PulseDuration/10000)*171;
  Distance = (PulseDuration*171)/10000;
  
  Serial.print("Pulse:");
  Serial.print(PulseDuration);
  Serial.print(" Distance:");
  Serial.print(Distance);
  Serial.print("cm\n\r");

  delay(500);
}
