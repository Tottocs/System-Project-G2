#define FIRMWARE_VERSION "v.0.1"

#define UART_BAUDRATE 9600 //At which speed the information is sent through the Arduino

#define ECHO_PIN 2 //Echo pin from ultrasonic sensor
#define TRIG_PIN 4 //Trig pin to ultrasonic sensor
#define TRIG_PULSE_LENGTH 10

long PulseDuration;
int Distance;

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
  PulseDuration = pulseIn(ECHO_PIN, HIGH);
  Distance = (PulseDuration*0.0343)/2;
  Serial.print("Distance:");
  Serial.print(Distance);
  Serial.print("cm\n\r");

  delay(100);
}
