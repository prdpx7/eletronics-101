// All 4 wheels test - both channels

// Channel A (left side)
#define AIN1 27
#define AIN2 26
#define PWMA 14

// Channel B (right side)
#define BIN1 25
#define BIN2 33
#define PWMB 12

#define STBY 32

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);
  digitalWrite(PWMA, HIGH);
  digitalWrite(PWMB, HIGH);

  Serial.println("All 4 wheels test ready");
}

void forward() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

void backward() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

void stop() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}

void loop() {
  Serial.println("ALL FORWARD");
  forward();
  delay(3000);

  Serial.println("STOP");
  stop();
  delay(2000);

  Serial.println("ALL BACKWARD");
  backward();
  delay(3000);

  Serial.println("STOP");
  stop();
  delay(2000);
}
