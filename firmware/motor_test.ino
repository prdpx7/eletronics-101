#define AIN1 27
#define AIN2 26
#define PWMA 14

#define PWM_FREQ 5000
#define PWM_RES  8

void setup() {
  Serial.begin(115200);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  ledcAttach(PWMA, PWM_FREQ, PWM_RES);
  Serial.println("Motor test ready");
}

void forward(uint8_t speed) {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  ledcWrite(PWMA, speed);
}

void backward(uint8_t speed) {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  ledcWrite(PWMA, speed);
}

void coast() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  ledcWrite(PWMA, 0);
}

void brake() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, HIGH);
  ledcWrite(PWMA, 0);
}

void loop() {
  Serial.println("Forward half speed");
  forward(128);
  delay(2000);

  Serial.println("Forward full speed");
  forward(255);
  delay(2000);

  Serial.println("Brake");
  brake();
  delay(1500);

  Serial.println("Backward half speed");
  backward(128);
  delay(2000);

  Serial.println("Backward full speed");
  backward(255);
  delay(2000);

  Serial.println("Coast stop");
  coast();
  delay(2000);
}
