//C code on Arduino UNO
#define MOT_L1_PIN 5  //Connected to IN2 pin to the servos 
#define MOT_L2_PIN 6  //Connected to IN1 pin to the servos 
#define MOT_R1_PIN 9  //Connected to IN3 pin to the servos 
#define MOT_R2_PIN 10 //Connected to IN4 pin to the servos  
#define ONTIME 1500

int leftServoSpeed = 233;
int rightServoSpeed = 200;

void setup() {
  pinMode(MOT_L1_PIN, OUTPUT);
  pinMode(MOT_L2_PIN, OUTPUT);
  pinMode(MOT_R1_PIN, OUTPUT);
  pinMode(MOT_R2_PIN, OUTPUT);

  digitalWrite(MOT_L1_PIN, LOW);
  digitalWrite(MOT_L2_PIN, LOW);
  digitalWrite(MOT_R1_PIN, LOW);
  digitalWrite(MOT_R2_PIN, LOW);
  
  Serial.begin(9600);
}

void set_motor_pwm(int pwm, int IN1_PIN, int IN2_PIN)
{
  if (pwm < 0) {  // reverse speeds
    analogWrite(IN1_PIN, -pwm);
    digitalWrite(IN2_PIN, LOW);

  } else { // stop or forward
    digitalWrite(IN1_PIN, LOW);
    analogWrite(IN2_PIN, pwm);
  }
}

void set_motor_currents(int pwm_A, int pwm_B)
{
  set_motor_pwm(pwm_A, MOT_L1_PIN, MOT_L2_PIN);
  set_motor_pwm(pwm_B, MOT_R1_PIN, MOT_R2_PIN);

  // Print a status message to the console.
  Serial.print("Set motor A PWM = ");
  Serial.print(pwm_A);
  Serial.print(" motor B PWM = ");
  Serial.println(pwm_B);
}

void spin_and_wait(int pwm_A, int pwm_B, int duration)
{
  set_motor_currents(pwm_A, pwm_B);
  delay(duration);
}

void loop()
{
  set_motor_currents(leftServoSpeed, rightServoSpeed); // sets speed of motors to value entered above for 0.5 sec and keeps repeating
}
