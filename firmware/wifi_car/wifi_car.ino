#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASS = "YOUR_PASSWORD";

// Channel A (left motors)
#define AIN1 27
#define AIN2 26
#define PWMA 14

// Channel B (right motors)
#define BIN1 25
#define BIN2 33
#define PWMB 12

#define STBY 32

#define WATCHDOG_MS 600

WebServer server(80);
unsigned long lastCmd = 0;
int currentSpeed = 255;

void setLeft(int dir, int spd) {
  switch (dir) {
    case 1:  digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);  break; // forward
    case -1: digitalWrite(AIN1, LOW);  digitalWrite(AIN2, HIGH); break; // backward
    default: digitalWrite(AIN1, LOW);  digitalWrite(AIN2, LOW);  break; // stop
  }
  analogWrite(PWMA, spd);
}

void setRight(int dir, int spd) {
  switch (dir) {
    case 1:  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);  break;
    case -1: digitalWrite(BIN1, LOW);  digitalWrite(BIN2, HIGH); break;
    default: digitalWrite(BIN1, LOW);  digitalWrite(BIN2, LOW);  break;
  }
  analogWrite(PWMB, spd);
}

void stopAll() {
  setLeft(0, 0);
  setRight(0, 0);
}

void sendOk(const char* msg) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", msg);
}

void handleDrive() {
  String action = server.arg("action");
  if (server.hasArg("speed")) currentSpeed = server.arg("speed").toInt();
  lastCmd = millis();

  if (action == "forward") {
    setLeft(1, currentSpeed);
    setRight(1, currentSpeed);
    sendOk("forward");
  } else if (action == "backward") {
    setLeft(-1, currentSpeed);
    setRight(-1, currentSpeed);
    sendOk("backward");
  } else if (action == "left") {
    setLeft(-1, currentSpeed);
    setRight(1, currentSpeed);
    sendOk("left");
  } else if (action == "right") {
    setLeft(1, currentSpeed);
    setRight(-1, currentSpeed);
    sendOk("right");
  } else {
    stopAll();
    sendOk("stop");
  }
}

void handleStop() {
  lastCmd = millis();
  stopAll();
  sendOk("stopped");
}

void handleStatus() {
  String json = "{\"uptime\":" + String(millis() / 1000) + ",\"speed\":" + String(currentSpeed) + "}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);
  stopAll();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("4wd-car")) {
    Serial.println("mDNS: http://4wd-car.local");
  }

  server.on("/drive", handleDrive);
  server.on("/stop", handleStop);
  server.on("/status", handleStatus);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  if (millis() - lastCmd > WATCHDOG_MS && lastCmd > 0) {
    stopAll();
  }
}
