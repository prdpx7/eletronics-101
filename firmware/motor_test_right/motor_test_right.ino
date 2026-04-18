// Right-side motor test - Channel B only
// Same pattern as left-side test, just different pins

#define BIN1 25
#define BIN2 33
#define PWMB 12
#define STBY 32

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);
  digitalWrite(PWMB, HIGH);

  Serial.println("Right motor test ready. STBY=HIGH, PWMB=HIGH");
}

void loop() {
  Serial.println("RIGHT FORWARD: BIN1=HIGH, BIN2=LOW");
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  delay(3000);

  Serial.println("RIGHT STOP: BIN1=LOW, BIN2=LOW");
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  delay(2000);

  Serial.println("RIGHT BACKWARD: BIN1=LOW, BIN2=HIGH");
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  delay(3000);

  Serial.println("RIGHT STOP: BIN1=LOW, BIN2=LOW");
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  delay(2000);
}
