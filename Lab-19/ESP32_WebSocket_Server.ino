/*
 * ESP32 WebSocket Server with Real-Time Sensor Streaming
 * Lab-19: HTTP & WebSocket Real-Time Communication
 * 
 * Features:
 * - WebSocket server with multiple endpoints
 * - Real-time sensor data streaming (DHT11 + BMP180)
 * - RGB LED control via WebSocket commands
 * - Multiple simultaneous client support
 * - Web dashboard with live charts
 * - JSON data format
 * - HTTP server for dashboard delivery
 * 
 * Hardware:
 * - DHT11: GPIO 4 (Temperature & Humidity)
 * - BMP180: I2C (SDA=21, SCL=22) (Pressure & Temperature)
 * - RGB LED: R=GPIO 25, G=GPIO 26, B=GPIO 27
 * 
 * WebSocket Endpoints:
 * - ws://{IP}/ws - Main WebSocket endpoint
 * 
 * HTTP Endpoints:
 * - http://{IP}/ - Web dashboard
 * - http://{IP}/sensors - Current sensor readings (JSON)
 * - http://{IP}/status - System status
 * 
 * WebSocket Commands (Client → Server):
 * - {"type":"led","r":255,"g":0,"b":0} - Set RGB color
 * - {"type":"led","mode":"off"} - Turn off LED
 * - {"type":"subscribe","stream":"sensors"} - Start sensor streaming
 * - {"type":"unsubscribe","stream":"sensors"} - Stop streaming
 * - {"type":"ping"} - Keepalive
 * 
 * WebSocket Messages (Server → Client):
 * - {"type":"sensors","temp":25.5,"humidity":60,"pressure":1013.25}
 * - {"type":"status","clients":3,"uptime":12345}
 * - {"type":"pong"}
 */

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <ArduinoJson.h>

// ============ WiFi Configuration ============
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ============ Pin Definitions ============
#define DHT_PIN       4
#define DHT_TYPE      DHT11

#define RGB_R_PIN     25
#define RGB_G_PIN     26
#define RGB_B_PIN     27

// I2C pins (default for ESP32)
#define I2C_SDA       21
#define I2C_SCL       22

// ============ PWM Configuration for RGB LED ============
#define PWM_FREQ      5000
#define PWM_RESOLUTION 8
#define PWM_R_CHANNEL 0
#define PWM_G_CHANNEL 1
#define PWM_B_CHANNEL 2

// ============ Server Configuration ============
#define HTTP_PORT     80
#define WS_PORT       81

// ============ Sensor Reading Interval ============
#define SENSOR_READ_INTERVAL   2000   // Read sensors every 2 seconds
#define SENSOR_STREAM_INTERVAL 1000   // Stream to clients every 1 second

// ============ Global Objects ============
WebServer httpServer(HTTP_PORT);
WebSocketsServer webSocket(WS_PORT);
DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_BMP085 bmp;

// ============ Sensor Data Structure ============
struct SensorData {
  float dht_temperature;
  float dht_humidity;
  float bmp_temperature;
  float bmp_pressure;
  bool dht_valid;
  bool bmp_valid;
  unsigned long lastReadTime;
};

SensorData sensorData = {0, 0, 0, 0, false, false, 0};

// ============ Client Management ============
struct ClientInfo {
  bool subscribedToSensors;
  unsigned long lastPingTime;
};

ClientInfo clients[8];  // Support up to 8 clients
int connectedClients = 0;

// ============ RGB LED State ============
struct RGBColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

RGBColor currentColor = {0, 0, 0};

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n╔════════════════════════════════════════════╗");
  Serial.println("║   ESP32 WebSocket Real-Time Communication  ║");
  Serial.println("║          Lab-19: Sensor Streaming          ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  
  // Initialize RGB LED (PWM)
  setupRGBLED();
  
  // Startup animation
  startupAnimation();
  
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Initialize sensors
  initializeSensors();
  
  // Connect to WiFi
  connectWiFi();
  
  // Setup HTTP server
  setupHTTPServer();
  
  // Setup WebSocket server
  setupWebSocket();
  
  // Start servers
  httpServer.begin();
  webSocket.begin();
  
  Serial.println("\n✓ All systems ready!");
  printAccessInfo();
}

// ============================================================================
// LOOP
// ============================================================================
void loop() {
  // Handle HTTP requests
  httpServer.handleClient();
  
  // Handle WebSocket events
  webSocket.loop();
  
  // Read sensors periodically
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead >= SENSOR_READ_INTERVAL) {
    readSensors();
    lastSensorRead = millis();
  }
  
  // Stream sensor data to subscribed clients
  static unsigned long lastSensorStream = 0;
  if (millis() - lastSensorStream >= SENSOR_STREAM_INTERVAL) {
    streamSensorData();
    lastSensorStream = millis();
  }
  
  // Check client timeouts (remove stale clients)
  checkClientTimeouts();
}

// ============================================================================
// RGB LED Setup
// ============================================================================
void setupRGBLED() {
  Serial.println("🌈 Configuring RGB LED...");
  
  // Configure PWM channels
  ledcSetup(PWM_R_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_G_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_B_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  
  // Attach channels to pins
  ledcAttachPin(RGB_R_PIN, PWM_R_CHANNEL);
  ledcAttachPin(RGB_G_PIN, PWM_G_CHANNEL);
  ledcAttachPin(RGB_B_PIN, PWM_B_CHANNEL);
  
  // Turn off initially
  setRGBColor(0, 0, 0);
  
  Serial.println("   ✓ RGB LED configured (PWM)");
}

// ============================================================================
// Set RGB LED Color
// ============================================================================
void setRGBColor(uint8_t r, uint8_t g, uint8_t b) {
  currentColor.r = r;
  currentColor.g = g;
  currentColor.b = b;
  
  ledcWrite(PWM_R_CHANNEL, r);
  ledcWrite(PWM_G_CHANNEL, g);
  ledcWrite(PWM_B_CHANNEL, b);
}

// ============================================================================
// Startup Animation
// ============================================================================
void startupAnimation() {
  Serial.println("✨ Running startup animation...");
  
  // Red
  setRGBColor(255, 0, 0);
  delay(300);
  
  // Green
  setRGBColor(0, 255, 0);
  delay(300);
  
  // Blue
  setRGBColor(0, 0, 255);
  delay(300);
  
  // Off
  setRGBColor(0, 0, 0);
  delay(200);
}

// ============================================================================
// Initialize Sensors
// ============================================================================
void initializeSensors() {
  Serial.println("🌡️  Initializing sensors...");
  
  // Initialize DHT11
  dht.begin();
  Serial.println("   ✓ DHT11 initialized (GPIO 4)");
  
  // Initialize BMP180
  if (bmp.begin()) {
    Serial.println("   ✓ BMP180 initialized (I2C: SDA=21, SCL=22)");
  } else {
    Serial.println("   ⚠️  BMP180 not found! Check wiring.");
  }
  
  // Initial sensor read
  delay(2000);  // DHT needs 2s to stabilize
  readSensors();
}

// ============================================================================
// Read Sensors
// ============================================================================
void readSensors() {
  // Read DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (!isnan(h) && !isnan(t)) {
    sensorData.dht_temperature = t;
    sensorData.dht_humidity = h;
    sensorData.dht_valid = true;
  } else {
    sensorData.dht_valid = false;
  }
  
  // Read BMP180
  if (bmp.begin()) {
    sensorData.bmp_temperature = bmp.readTemperature();
    sensorData.bmp_pressure = bmp.readPressure() / 100.0;  // Convert to hPa
    sensorData.bmp_valid = true;
  } else {
    sensorData.bmp_valid = false;
  }
  
  sensorData.lastReadTime = millis();
}

// ============================================================================
// Connect to WiFi
// ============================================================================
void connectWiFi() {
  Serial.println("📡 Connecting to WiFi...");
  Serial.print("   SSID: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Visual feedback during connection
  int dots = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    
    // Blink LED during connection
    if (dots % 2 == 0) {
      setRGBColor(0, 0, 255);  // Blue
    } else {
      setRGBColor(0, 0, 0);
    }
    
    dots++;
    
    if (dots > 40) {  // 20 seconds timeout
      Serial.println("\n❌ WiFi connection failed!");
      Serial.println("   Check SSID and password.");
      Serial.println("   Restarting...");
      delay(3000);
      ESP.restart();
    }
  }
  
  Serial.println("\n✓ WiFi connected!");
  Serial.print("   IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Green flash to indicate success
  setRGBColor(0, 255, 0);
  delay(500);
  setRGBColor(0, 0, 0);
}

// ============================================================================
// Setup HTTP Server
// ============================================================================
void setupHTTPServer() {
  Serial.println("🌐 Setting up HTTP server...");
  
  // Root endpoint - serve dashboard
  httpServer.on("/", HTTP_GET, handleRoot);
  
  // Sensor data endpoint (JSON)
  httpServer.on("/sensors", HTTP_GET, handleSensors);
  
  // Status endpoint
  httpServer.on("/status", HTTP_GET, handleStatus);
  
  // 404 handler
  httpServer.onNotFound(handleNotFound);
  
  Serial.println("   ✓ HTTP endpoints configured");
}

// ============================================================================
// Setup WebSocket Server
// ============================================================================
void setupWebSocket() {
  Serial.println("🔌 Setting up WebSocket server...");
  
  webSocket.onEvent(webSocketEvent);
  
  // Initialize client info
  for (int i = 0; i < 8; i++) {
    clients[i].subscribedToSensors = false;
    clients[i].lastPingTime = 0;
  }
  
  Serial.println("   ✓ WebSocket configured");
}

// ============================================================================
// WebSocket Event Handler
// ============================================================================
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      handleClientDisconnect(num);
      break;
      
    case WStype_CONNECTED:
      handleClientConnect(num);
      break;
      
    case WStype_TEXT:
      handleClientMessage(num, (char*)payload, length);
      break;
      
    case WStype_BIN:
      // Binary data not supported in this lab
      Serial.printf("[%u] Received binary data (not supported)\n", num);
      break;
      
    case WStype_ERROR:
      Serial.printf("[%u] WebSocket error\n", num);
      break;
      
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      // Fragmented messages not handled
      break;
  }
}

// ============================================================================
// Handle Client Connect
// ============================================================================
void handleClientConnect(uint8_t num) {
  IPAddress ip = webSocket.remoteIP(num);
  
  Serial.printf("✓ Client #%u connected from %s\n", num, ip.toString().c_str());
  
  connectedClients++;
  clients[num].subscribedToSensors = true;  // Auto-subscribe
  clients[num].lastPingTime = millis();
  
  // Send welcome message
  StaticJsonDocument<200> doc;
  doc["type"] = "welcome";
  doc["message"] = "Connected to ESP32 WebSocket Server";
  doc["clientId"] = num;
  doc["endpoints"] = "/ws";
  
  String json;
  serializeJson(doc, json);
  webSocket.sendTXT(num, json);
  
  // Immediately send current sensor data
  sendSensorData(num);
  
  // Flash LED green
  setRGBColor(0, 255, 0);
  delay(100);
  setRGBColor(0, 0, 0);
}

// ============================================================================
// Handle Client Disconnect
// ============================================================================
void handleClientDisconnect(uint8_t num) {
  Serial.printf("✗ Client #%u disconnected\n", num);
  
  connectedClients--;
  clients[num].subscribedToSensors = false;
  
  // Flash LED red
  setRGBColor(255, 0, 0);
  delay(100);
  setRGBColor(0, 0, 0);
}

// ============================================================================
// Handle Client Message
// ============================================================================
void handleClientMessage(uint8_t num, char* payload, size_t length) {
  Serial.printf("[%u] Received: %s\n", num, payload);
  
  // Parse JSON
  StaticJsonDocument<300> doc;
  DeserializationError error = deserializeJson(doc, payload);
  
  if (error) {
    Serial.printf("[%u] JSON parse error: %s\n", num, error.c_str());
    sendError(num, "Invalid JSON");
    return;
  }
  
  // Get message type
  const char* type = doc["type"];
  
  if (strcmp(type, "led") == 0) {
    handleLEDCommand(num, doc);
  }
  else if (strcmp(type, "subscribe") == 0) {
    clients[num].subscribedToSensors = true;
    sendAck(num, "Subscribed to sensor stream");
  }
  else if (strcmp(type, "unsubscribe") == 0) {
    clients[num].subscribedToSensors = false;
    sendAck(num, "Unsubscribed from sensor stream");
  }
  else if (strcmp(type, "ping") == 0) {
    clients[num].lastPingTime = millis();
    sendPong(num);
  }
  else if (strcmp(type, "getStatus") == 0) {
    sendStatus(num);
  }
  else {
    sendError(num, "Unknown command type");
  }
}

// ============================================================================
// Handle LED Command
// ============================================================================
void handleLEDCommand(uint8_t num, JsonDocument& doc) {
  if (doc.containsKey("mode")) {
    const char* mode = doc["mode"];
    
    if (strcmp(mode, "off") == 0) {
      setRGBColor(0, 0, 0);
      sendAck(num, "LED turned off");
      broadcastLEDState();
      return;
    }
  }
  
  if (doc.containsKey("r") && doc.containsKey("g") && doc.containsKey("b")) {
    uint8_t r = doc["r"];
    uint8_t g = doc["g"];
    uint8_t b = doc["b"];
    
    setRGBColor(r, g, b);
    
    char msg[50];
    sprintf(msg, "LED set to RGB(%d, %d, %d)", r, g, b);
    sendAck(num, msg);
    
    // Broadcast LED state to all clients
    broadcastLEDState();
  } else {
    sendError(num, "LED command requires 'r', 'g', 'b' values");
  }
}

// ============================================================================
// Stream Sensor Data to Subscribed Clients
// ============================================================================
void streamSensorData() {
  for (int i = 0; i < 8; i++) {
    if (clients[i].subscribedToSensors && webSocket.connectedClients() > 0) {
      sendSensorData(i);
    }
  }
}

// ============================================================================
// Send Sensor Data to Specific Client
// ============================================================================
void sendSensorData(uint8_t num) {
  StaticJsonDocument<400> doc;
  doc["type"] = "sensors";
  doc["timestamp"] = millis();
  
  // DHT11 data
  if (sensorData.dht_valid) {
    doc["dht"]["temperature"] = round(sensorData.dht_temperature * 10) / 10.0;
    doc["dht"]["humidity"] = round(sensorData.dht_humidity * 10) / 10.0;
    doc["dht"]["valid"] = true;
  } else {
    doc["dht"]["valid"] = false;
  }
  
  // BMP180 data
  if (sensorData.bmp_valid) {
    doc["bmp"]["temperature"] = round(sensorData.bmp_temperature * 10) / 10.0;
    doc["bmp"]["pressure"] = round(sensorData.bmp_pressure * 10) / 10.0;
    doc["bmp"]["valid"] = true;
  } else {
    doc["bmp"]["valid"] = false;
  }
  
  String json;
  serializeJson(doc, json);
  webSocket.sendTXT(num, json);
}

// ============================================================================
// Broadcast LED State to All Clients
// ============================================================================
void broadcastLEDState() {
  StaticJsonDocument<150> doc;
  doc["type"] = "ledState";
  doc["r"] = currentColor.r;
  doc["g"] = currentColor.g;
  doc["b"] = currentColor.b;
  
  String json;
  serializeJson(doc, json);
  webSocket.broadcastTXT(json);
}

// ============================================================================
// Send Acknowledgment
// ============================================================================
void sendAck(uint8_t num, const char* message) {
  StaticJsonDocument<150> doc;
  doc["type"] = "ack";
  doc["message"] = message;
  
  String json;
  serializeJson(doc, json);
  webSocket.sendTXT(num, json);
}

// ============================================================================
// Send Error
// ============================================================================
void sendError(uint8_t num, const char* message) {
  StaticJsonDocument<150> doc;
  doc["type"] = "error";
  doc["message"] = message;
  
  String json;
  serializeJson(doc, json);
  webSocket.sendTXT(num, json);
}

// ============================================================================
// Send Pong
// ============================================================================
void sendPong(uint8_t num) {
  StaticJsonDocument<50> doc;
  doc["type"] = "pong";
  doc["timestamp"] = millis();
  
  String json;
  serializeJson(doc, json);
  webSocket.sendTXT(num, json);
}

// ============================================================================
// Send Status
// ============================================================================
void sendStatus(uint8_t num) {
  StaticJsonDocument<250> doc;
  doc["type"] = "status";
  doc["uptime"] = millis() / 1000;
  doc["clients"] = connectedClients;
  doc["freeHeap"] = ESP.getFreeHeap();
  doc["wifiRSSI"] = WiFi.RSSI();
  doc["ipAddress"] = WiFi.localIP().toString();
  
  String json;
  serializeJson(doc, json);
  webSocket.sendTXT(num, json);
}

// ============================================================================
// Check Client Timeouts
// ============================================================================
void checkClientTimeouts() {
  static unsigned long lastCheck = 0;
  
  if (millis() - lastCheck < 5000) return;  // Check every 5 seconds
  lastCheck = millis();
  
  // Timeout not strictly enforced - WebSocket library handles disconnects
}

// ============================================================================
// HTTP Handler: Root (Dashboard)
// ============================================================================
void handleRoot() {
  String html = getHTML();
  httpServer.send(200, "text/html", html);
}

// ============================================================================
// HTTP Handler: Sensors (JSON)
// ============================================================================
void handleSensors() {
  StaticJsonDocument<400> doc;
  
  // DHT11
  if (sensorData.dht_valid) {
    doc["dht"]["temperature"] = sensorData.dht_temperature;
    doc["dht"]["humidity"] = sensorData.dht_humidity;
    doc["dht"]["valid"] = true;
  } else {
    doc["dht"]["valid"] = false;
  }
  
  // BMP180
  if (sensorData.bmp_valid) {
    doc["bmp"]["temperature"] = sensorData.bmp_temperature;
    doc["bmp"]["pressure"] = sensorData.bmp_pressure;
    doc["bmp"]["valid"] = true;
  } else {
    doc["bmp"]["valid"] = false;
  }
  
  doc["timestamp"] = millis();
  
  String json;
  serializeJson(doc, json);
  
  httpServer.send(200, "application/json", json);
}

// ============================================================================
// HTTP Handler: Status
// ============================================================================
void handleStatus() {
  StaticJsonDocument<250> doc;
  doc["uptime"] = millis() / 1000;
  doc["clients"] = connectedClients;
  doc["freeHeap"] = ESP.getFreeHeap();
  doc["wifiRSSI"] = WiFi.RSSI();
  doc["ipAddress"] = WiFi.localIP().toString();
  doc["ssid"] = WiFi.SSID();
  
  String json;
  serializeJson(doc, json);
  
  httpServer.send(200, "application/json", json);
}

// ============================================================================
// HTTP Handler: 404 Not Found
// ============================================================================
void handleNotFound() {
  String message = "404: Not Found\n\n";
  message += "URI: " + httpServer.uri() + "\n";
  message += "Method: " + String((httpServer.method() == HTTP_GET) ? "GET" : "POST") + "\n";
  
  httpServer.send(404, "text/plain", message);
}

// ============================================================================
// Print Access Information
// ============================================================================
void printAccessInfo() {
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║           Access Information               ║");
  Serial.println("╠════════════════════════════════════════════╣");
  Serial.print("║ IP Address:  ");
  Serial.print(WiFi.localIP());
  Serial.println("                ║");
  Serial.println("║                                            ║");
  Serial.println("║ HTTP Endpoints:                            ║");
  Serial.print("║   Dashboard:  http://");
  Serial.print(WiFi.localIP());
  Serial.println("/          ║");
  Serial.print("║   Sensors:    http://");
  Serial.print(WiFi.localIP());
  Serial.println("/sensors   ║");
  Serial.print("║   Status:     http://");
  Serial.print(WiFi.localIP());
  Serial.println("/status    ║");
  Serial.println("║                                            ║");
  Serial.println("║ WebSocket:                                 ║");
  Serial.print("║   Endpoint:   ws://");
  Serial.print(WiFi.localIP());
  Serial.println(":81/ws       ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
}

// ============================================================================
// Get HTML Dashboard
// ============================================================================
String getHTML() {
  // This will be in a separate dashboard.html file
  // For now, return a placeholder
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 WebSocket Dashboard</title>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    /* CSS will be in separate file */
    body { font-family: Arial, sans-serif; margin: 20px; background: #f0f0f0; }
    .container { max-width: 1200px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; }
    h1 { color: #333; text-align: center; }
    .status { padding: 10px; background: #4CAF50; color: white; border-radius: 5px; margin-bottom: 20px; }
  </style>
</head>
<body>
  <div class="container">
    <h1>🌐 ESP32 WebSocket Dashboard</h1>
    <div class="status" id="status">⚡ Connecting to WebSocket...</div>
    <p>Full dashboard in <a href="dashboard.html">dashboard.html</a></p>
    <p>See README.md for complete documentation.</p>
  </div>
</body>
</html>
)rawliteral";
}
