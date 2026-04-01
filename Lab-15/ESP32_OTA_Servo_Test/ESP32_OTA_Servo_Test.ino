/*
 * ESP32 OTA Servo Test
 * Lab-15 - SSG434
 *
 * Purpose:
 * - Minimal OTA-capable firmware that oscillates a servo on GPIO 5.
 * - Keeps web OTA endpoint so you can upload new firmware again.
 */

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <Update.h>
#include <ESP32Servo.h>

// ============ Firmware Version ============
#define FIRMWARE_VERSION "servo-test-1.0.0"

// ============ WiFi Configuration ============
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ============ OTA Configuration ============
const char* ota_hostname = "ESP32-OTA";
const char* ota_password = "admin123";

// ============ Servo Configuration ============
#define SERVO_PIN 5
const int servoMinUs = 500;
const int servoMaxUs = 2400;

WebServer server(80);
Servo servo;

unsigned long lastMove = 0;
int angle = 0;
int step = 5;

const char* indexHTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 OTA Servo Test</title>
  <style>
    body { font-family: Arial, sans-serif; margin: 40px; }
    .card { max-width: 520px; margin: 0 auto; padding: 24px; border: 1px solid #ddd; border-radius: 12px; }
    h1 { font-size: 22px; margin-bottom: 8px; }
    .muted { color: #666; font-size: 13px; margin-bottom: 16px; }
    input[type="file"] { width: 100%; margin: 12px 0; }
    button { width: 100%; padding: 12px; font-size: 15px; }
    .status { margin-top: 12px; font-size: 14px; }
  </style>
</head>
<body>
  <div class="card">
    <h1>ESP32 OTA Servo Test</h1>
    <div class="muted">Upload a new .bin to update firmware.</div>
    <form method="POST" action="/update" enctype="multipart/form-data">
      <input type="file" name="update" accept=".bin" required>
      <button type="submit">Upload & Update</button>
    </form>
    <div class="status">Version: servo-test-1.0.0</div>
  </div>
</body>
</html>
)rawliteral";

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  MDNS.begin(ota_hostname);
}

void setupArduinoOTA() {
  ArduinoOTA.setHostname(ota_hostname);
  ArduinoOTA.setPassword(ota_password);
  ArduinoOTA.begin();
}

void setupWebServer() {
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", indexHTML);
  });

  server.on("/update", HTTP_POST,
    []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "Update Failed" : "Update Success! Rebooting...");
      delay(1000);
      ESP.restart();
    },
    []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Update.begin(UPDATE_SIZE_UNKNOWN);
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        Update.write(upload.buf, upload.currentSize);
      } else if (upload.status == UPLOAD_FILE_END) {
        Update.end(true);
      }
    }
  );

  server.begin();
}

void setup() {
  Serial.begin(115200);
  setupWiFi();
  setupArduinoOTA();
  setupWebServer();

  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, servoMinUs, servoMaxUs);
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  unsigned long now = millis();
  if (now - lastMove >= 50) {
    lastMove = now;
    angle += step;
    if (angle >= 180 || angle <= 0) {
      step = -step;
    }
    servo.write(angle);
  }
}
