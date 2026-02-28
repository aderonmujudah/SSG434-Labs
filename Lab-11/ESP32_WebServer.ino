// ===== CONFIGURATION - EDIT THESE VALUES =====
// Pin Connections
#define LED1_PIN    // GPIO pin for LED 1
#define LED2_PIN    // GPIO pin for LED 2
#define DHT_PIN     // GPIO pin for DHT11 sensor

// Access Point Credentials
const char* ssid = "";      // Your Access Point name
const char* password = "";  // Your Access Point password (min 8 chars, leave empty for open network)

// ============================================

#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

#define DHTTYPE DHT11

DHT dht(DHT_PIN, DHTTYPE);
WebServer server(80);



bool led1State = false;
bool led2State = false;

// HTML content
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Control Panel</title>
    <link rel="stylesheet" href="/style.css">
</head>
<body>
    <div class="container">
        <h1>ESP32 Control Panel</h1>
        
        <div class="section">
            <h2>LED Controls</h2>
            <div class="led-controls">
                <div class="led-control">
                    <span class="led-label">LED 1</span>
                    <button id="led1Btn" class="btn btn-off" onclick="toggleLED(1)">OFF</button>
                </div>
                <div class="led-control">
                    <span class="led-label">LED 2</span>
                    <button id="led2Btn" class="btn btn-off" onclick="toggleLED(2)">OFF</button>
                </div>
            </div>
        </div>

        <div class="section">
            <h2>Sensor Data</h2>
            <div class="sensor-data">
                <div class="sensor-card">
                    <div class="sensor-icon">🌡️</div>
                    <div class="sensor-value" id="temperature">--</div>
                    <div class="sensor-unit">°C</div>
                    <div class="sensor-label">Temperature</div>
                </div>
                <div class="sensor-card">
                    <div class="sensor-icon">💧</div>
                    <div class="sensor-value" id="humidity">--</div>
                    <div class="sensor-unit">%</div>
                    <div class="sensor-label">Humidity</div>
                </div>
            </div>
            <div class="last-update">Last update: <span id="lastUpdate">Never</span></div>
        </div>

        <div class="status">
            <span class="status-indicator" id="statusIndicator"></span>
            <span id="statusText">Connected</span>
        </div>
    </div>
    <script src="/script.js"></script>
</body>
</html>
)rawliteral";

// CSS content
const char* cssContent = R"rawliteral(
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    min-height: 100vh;
    display: flex;
    justify-content: center;
    align-items: center;
    padding: 20px;
}

.container {
    background: white;
    border-radius: 20px;
    padding: 40px;
    max-width: 600px;
    width: 100%;
    box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
}

h1 {
    color: #333;
    text-align: center;
    margin-bottom: 30px;
    font-size: 2em;
}

h2 {
    color: #667eea;
    margin-bottom: 20px;
    font-size: 1.5em;
}

.section {
    margin-bottom: 40px;
}

.led-controls {
    display: flex;
    flex-direction: column;
    gap: 15px;
}

.led-control {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 15px;
    background: #f8f9fa;
    border-radius: 10px;
}

.led-label {
    font-size: 1.1em;
    font-weight: 600;
    color: #333;
}

.btn {
    padding: 10px 30px;
    border: none;
    border-radius: 25px;
    font-size: 1em;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.3s ease;
    min-width: 80px;
}

.btn-off {
    background: #e0e0e0;
    color: #666;
}

.btn-off:hover {
    background: #d0d0d0;
}

.btn-on {
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: white;
    box-shadow: 0 4px 15px rgba(102, 126, 234, 0.4);
}

.btn-on:hover {
    transform: translateY(-2px);
    box-shadow: 0 6px 20px rgba(102, 126, 234, 0.6);
}

.sensor-data {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    gap: 20px;
    margin-bottom: 15px;
}

.sensor-card {
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: white;
    padding: 25px;
    border-radius: 15px;
    text-align: center;
    box-shadow: 0 4px 15px rgba(102, 126, 234, 0.3);
}

.sensor-icon {
    font-size: 2.5em;
    margin-bottom: 10px;
}

.sensor-value {
    font-size: 2.5em;
    font-weight: bold;
    margin: 10px 0;
}

.sensor-unit {
    font-size: 1.2em;
    opacity: 0.9;
}

.sensor-label {
    font-size: 1em;
    margin-top: 10px;
    opacity: 0.9;
}

.last-update {
    text-align: center;
    color: #666;
    font-size: 0.9em;
    font-style: italic;
}

.status {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 8px;
    padding: 15px;
    background: #f8f9fa;
    border-radius: 10px;
    margin-top: 20px;
}

.status-indicator {
    width: 12px;
    height: 12px;
    border-radius: 50%;
    background: #4caf50;
    animation: pulse 2s infinite;
}

@keyframes pulse {
    0%, 100% {
        opacity: 1;
    }
    50% {
        opacity: 0.5;
    }
}

#statusText {
    color: #666;
    font-weight: 500;
}

@media (max-width: 480px) {
    .container {
        padding: 20px;
    }
    
    h1 {
        font-size: 1.5em;
    }
    
    .sensor-value {
        font-size: 2em;
    }
}
)rawliteral";

// JavaScript content
const char* jsContent = R"rawliteral(
let updateInterval;

// Initialize on page load
document.addEventListener('DOMContentLoaded', function() {
    updateSensorData();
    updateInterval = setInterval(updateSensorData, 2000); // Update every 2 seconds
    getLEDStates();
});

// Toggle LED function
function toggleLED(ledNumber) {
    fetch('/led' + ledNumber + '/toggle')
        .then(response => response.json())
        .then(data => {
            updateLEDButton(ledNumber, data.state);
        })
        .catch(error => {
            console.error('Error:', error);
            showStatus('Connection Error', false);
        });
}

// Update LED button appearance
function updateLEDButton(ledNumber, state) {
    const btn = document.getElementById('led' + ledNumber + 'Btn');
    if (state) {
        btn.textContent = 'ON';
        btn.className = 'btn btn-on';
    } else {
        btn.textContent = 'OFF';
        btn.className = 'btn btn-off';
    }
}

// Get current LED states
function getLEDStates() {
    fetch('/led/status')
        .then(response => response.json())
        .then(data => {
            updateLEDButton(1, data.led1);
            updateLEDButton(2, data.led2);
        })
        .catch(error => {
            console.error('Error:', error);
        });
}

// Update sensor data
function updateSensorData() {
    fetch('/sensor/data')
        .then(response => response.json())
        .then(data => {
            document.getElementById('temperature').textContent = data.temperature.toFixed(1);
            document.getElementById('humidity').textContent = data.humidity.toFixed(1);
            
            const now = new Date();
            const timeString = now.toLocaleTimeString();
            document.getElementById('lastUpdate').textContent = timeString;
            
            showStatus('Connected', true);
        })
        .catch(error => {
            console.error('Error:', error);
            document.getElementById('temperature').textContent = '--';
            document.getElementById('humidity').textContent = '--';
            showStatus('Connection Error', false);
        });
}

// Show connection status
function showStatus(message, isConnected) {
    const statusText = document.getElementById('statusText');
    const statusIndicator = document.getElementById('statusIndicator');
    
    statusText.textContent = message;
    
    if (isConnected) {
        statusIndicator.style.background = '#4caf50';
        statusIndicator.style.animation = 'pulse 2s infinite';
    } else {
        statusIndicator.style.background = '#f44336';
        statusIndicator.style.animation = 'none';
    }
}
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  // Initialize LED pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  
  // Initialize DHT sensor
  dht.begin();
  
  // Start Access Point
  Serial.println("\nStarting Access Point...");
  
  if (strlen(password) < 8 && strlen(password) > 0) {
    Serial.println("Error: Password must be at least 8 characters or empty for open network");
    return;
  }
  
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.println("Access Point Started");
  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.print("SSID: ");
  Serial.println(ssid);
  
  // Setup web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/style.css", HTTP_GET, handleCSS);
  server.on("/script.js", HTTP_GET, handleJS);
  server.on("/led1/toggle", HTTP_GET, handleLED1Toggle);
  server.on("/led2/toggle", HTTP_GET, handleLED2Toggle);
  server.on("/led/status", HTTP_GET, handleLEDStatus);
  server.on("/sensor/data", HTTP_GET, handleSensorData);
  
  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Connect to the WiFi network and navigate to: http://" + IP.toString());
}

void loop() {
  server.handleClient();
}

// Web server handlers
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleCSS() {
  server.send(200, "text/css", cssContent);
}

void handleJS() {
  server.send(200, "application/javascript", jsContent);
}

void handleLED1Toggle() {
  led1State = !led1State;
  digitalWrite(LED1_PIN, led1State ? HIGH : LOW);
  
  String json = "{\"led\":1,\"state\":" + String(led1State ? "true" : "false") + "}";
  server.send(200, "application/json", json);
  
  Serial.println("LED 1: " + String(led1State ? "ON" : "OFF"));
}

void handleLED2Toggle() {
  led2State = !led2State;
  digitalWrite(LED2_PIN, led2State ? HIGH : LOW);
  
  String json = "{\"led\":2,\"state\":" + String(led2State ? "true" : "false") + "}";
  server.send(200, "application/json", json);
  
  Serial.println("LED 2: " + String(led2State ? "ON" : "OFF"));
}

void handleLEDStatus() {
  String json = "{\"led1\":" + String(led1State ? "true" : "false") + 
                ",\"led2\":" + String(led2State ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

void handleSensorData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  if (isnan(humidity) || isnan(temperature)) {
    String json = "{\"temperature\":0,\"humidity\":0,\"error\":\"Failed to read from DHT sensor\"}";
    server.send(500, "application/json", json);
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  String json = "{\"temperature\":" + String(temperature) + 
                ",\"humidity\":" + String(humidity) + "}";
  server.send(200, "application/json", json);
}
