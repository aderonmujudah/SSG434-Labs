/*
 * ESP32 OTA (Over-The-Air) Firmware Update
 * Lab-15 - SSG434
 * 
 * TEST FIRMWARE VERSION 2.0.0
 * 
 * Changes from v1.0.0:
 * - Updated version number to 2.0.0
 * - Modified LED blink pattern (faster heartbeat)
 * - Added rainbow LED effect on startup
 * - Enhanced status messages
 * - New feature: LED patterns
 * 
 * Use this firmware to test OTA update functionality
 * 
 * Hardware:
 * - ESP32 Development Board
 * - Built-in LED on GPIO 2
 * 
 * Version: 2.0.0
 * Last Updated: 2026-02-28
 */

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <Update.h>

// ============ Firmware Version ============
#define FIRMWARE_VERSION "2.0.0"  // ← VERSION UPDATED!
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

// ============ WiFi Configuration ============
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ============ OTA Configuration ============
const char* ota_hostname = "ESP32-OTA";
const char* ota_password = "admin123";

// ============ Hardware Configuration ============
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif
#define LED_PIN LED_BUILTIN

// ============ Global Objects ============
WebServer server(80);

// ============ Status Variables ============
unsigned long lastBlink = 0;
int blinkInterval = 500;  // ← FASTER BLINK! (was 1000)
bool ledState = false;
String updateStatus = "Ready";
int updateProgress = 0;

// ============ Web Pages (HTML) ============
const char* indexHTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 OTA Update v2.0</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
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
            padding: 40px;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            max-width: 600px;
            width: 100%;
        }
        h1 {
            color: #667eea;
            text-align: center;
            margin-bottom: 10px;
            font-size: 28px;
        }
        .subtitle {
            text-align: center;
            color: #666;
            margin-bottom: 30px;
            font-size: 14px;
        }
        .version-banner {
            background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
            color: white;
            padding: 15px;
            border-radius: 10px;
            text-align: center;
            margin-bottom: 20px;
            font-size: 24px;
            font-weight: bold;
            box-shadow: 0 5px 15px rgba(0,0,0,0.2);
        }
        .info-card {
            background: #f8f9fa;
            padding: 20px;
            border-radius: 10px;
            margin-bottom: 30px;
            border-left: 4px solid #667eea;
        }
        .info-row {
            display: flex;
            justify-content: space-between;
            padding: 10px 0;
            border-bottom: 1px solid #e0e0e0;
        }
        .info-row:last-child { border-bottom: none; }
        .info-label {
            font-weight: 600;
            color: #555;
        }
        .info-value {
            color: #333;
            font-family: 'Courier New', monospace;
        }
        .version {
            background: #11998e;
            color: white;
            padding: 5px 15px;
            border-radius: 20px;
            font-size: 14px;
            font-weight: bold;
        }
        .upload-section {
            background: #fff9e6;
            padding: 25px;
            border-radius: 10px;
            border: 2px dashed #ffc107;
        }
        .upload-section h3 {
            color: #ff9800;
            margin-bottom: 15px;
            font-size: 18px;
        }
        input[type="file"] {
            width: 100%;
            padding: 15px;
            border: 2px solid #ddd;
            border-radius: 8px;
            margin-bottom: 15px;
            font-size: 14px;
            cursor: pointer;
        }
        input[type="file"]:hover {
            border-color: #667eea;
        }
        .btn {
            width: 100%;
            padding: 15px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: bold;
            cursor: pointer;
            transition: transform 0.2s, box-shadow 0.2s;
        }
        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 20px rgba(102, 126, 234, 0.4);
        }
        .btn:disabled {
            background: #ccc;
            cursor: not-allowed;
            transform: none;
        }
        .progress-container {
            display: none;
            margin-top: 20px;
        }
        .progress-bar {
            width: 100%;
            height: 30px;
            background: #e0e0e0;
            border-radius: 15px;
            overflow: hidden;
            position: relative;
        }
        .progress-fill {
            height: 100%;
            background: linear-gradient(90deg, #11998e 0%, #38ef7d 100%);
            width: 0%;
            transition: width 0.3s;
            display: flex;
            align-items: center;
            justify-content: center;
            color: white;
            font-weight: bold;
            font-size: 14px;
        }
        .status-message {
            margin-top: 15px;
            padding: 15px;
            border-radius: 8px;
            text-align: center;
            font-weight: 600;
            display: none;
        }
        .status-success {
            background: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .status-error {
            background: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
        .status-warning {
            background: #fff3cd;
            color: #856404;
            border: 1px solid #ffeaa7;
        }
        .led-controls {
            margin-top: 30px;
            padding: 20px;
            background: #f0f7ff;
            border-radius: 10px;
        }
        .led-controls h3 {
            color: #667eea;
            margin-bottom: 15px;
            font-size: 18px;
        }
        .led-buttons {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 10px;
        }
        .led-btn {
            padding: 12px;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            font-weight: 600;
            transition: all 0.2s;
        }
        .led-on {
            background: #4caf50;
            color: white;
        }
        .led-off {
            background: #f44336;
            color: white;
        }
        .led-pattern {
            background: #ff9800;
            color: white;
        }
        .led-btn:hover {
            transform: scale(1.05);
            box-shadow: 0 3px 10px rgba(0,0,0,0.2);
        }
        .new-badge {
            background: #e74c3c;
            color: white;
            padding: 3px 8px;
            border-radius: 12px;
            font-size: 10px;
            margin-left: 5px;
            animation: pulse 2s infinite;
        }
        @keyframes pulse {
            0%, 100% { transform: scale(1); }
            50% { transform: scale(1.1); }
        }
        .footer {
            text-align: center;
            margin-top: 30px;
            color: #999;
            font-size: 12px;
        }
        @keyframes updating {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.5; }
        }
        .updating {
            animation: updating 1s infinite;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🚀 ESP32 OTA Update Center</h1>
        <p class="subtitle">Over-The-Air Firmware Update System</p>
        
        <div class="version-banner">
            ✨ VERSION 2.0.0 ✨
        </div>
        
        <div class="info-card">
            <div class="info-row">
                <span class="info-label">Firmware Version:</span>
                <span class="version">v%FIRMWARE_VERSION%</span>
            </div>
            <div class="info-row">
                <span class="info-label">Build Date:</span>
                <span class="info-value">%BUILD_DATE%</span>
            </div>
            <div class="info-row">
                <span class="info-label">Build Time:</span>
                <span class="info-value">%BUILD_TIME%</span>
            </div>
            <div class="info-row">
                <span class="info-label">Hostname:</span>
                <span class="info-value">%HOSTNAME%</span>
            </div>
            <div class="info-row">
                <span class="info-label">IP Address:</span>
                <span class="info-value">%LOCAL_IP%</span>
            </div>
            <div class="info-row">
                <span class="info-label">MAC Address:</span>
                <span class="info-value">%MAC_ADDRESS%</span>
            </div>
            <div class="info-row">
                <span class="info-label">Uptime:</span>
                <span class="info-value">%UPTIME%</span>
            </div>
            <div class="info-row">
                <span class="info-label">Free Heap:</span>
                <span class="info-value">%FREE_HEAP% KB</span>
            </div>
        </div>

        <div class="upload-section">
            <h3>📦 Upload New Firmware</h3>
            <form id="uploadForm" enctype="multipart/form-data">
                <input type="file" name="firmware" id="firmwareFile" accept=".bin" required>
                <button type="submit" class="btn" id="uploadBtn">Upload & Update Firmware</button>
            </form>
            
            <div class="progress-container" id="progressContainer">
                <div class="progress-bar">
                    <div class="progress-fill" id="progressFill">0%</div>
                </div>
            </div>
            
            <div class="status-message" id="statusMessage"></div>
        </div>

        <div class="led-controls">
            <h3>💡 LED Control <span class="new-badge">NEW</span></h3>
            <div class="led-buttons">
                <button class="led-btn led-on" onclick="controlLED('on')">Turn ON</button>
                <button class="led-btn led-off" onclick="controlLED('off')">Turn OFF</button>
                <button class="led-btn led-pattern" onclick="controlLED('blink')">Blink Pattern</button>
            </div>
        </div>

        <div class="footer">
            Lab-15 | SSG434 | ESP32 OTA System v2.0
        </div>
    </div>

    <script>
        document.getElementById('uploadForm').onsubmit = async function(e) {
            e.preventDefault();
            
            const fileInput = document.getElementById('firmwareFile');
            const file = fileInput.files[0];
            
            if (!file) {
                showStatus('Please select a firmware file', 'error');
                return;
            }
            
            if (!file.name.endsWith('.bin')) {
                showStatus('Invalid file type. Please select a .bin file', 'error');
                return;
            }
            
            const uploadBtn = document.getElementById('uploadBtn');
            const progressContainer = document.getElementById('progressContainer');
            const progressFill = document.getElementById('progressFill');
            
            uploadBtn.disabled = true;
            uploadBtn.classList.add('updating');
            progressContainer.style.display = 'block';
            
            showStatus('Uploading firmware... Please wait', 'warning');
            
            const formData = new FormData();
            formData.append('firmware', file);
            
            try {
                const xhr = new XMLHttpRequest();
                
                xhr.upload.addEventListener('progress', function(e) {
                    if (e.lengthComputable) {
                        const percentComplete = Math.round((e.loaded / e.total) * 100);
                        progressFill.style.width = percentComplete + '%';
                        progressFill.textContent = percentComplete + '%';
                    }
                });
                
                xhr.addEventListener('load', function() {
                    if (xhr.status === 200) {
                        progressFill.style.width = '100%';
                        progressFill.textContent = '100%';
                        showStatus('✅ Update successful! Device will reboot in 3 seconds...', 'success');
                        
                        setTimeout(function() {
                            showStatus('🔄 Rebooting... Page will reload automatically', 'warning');
                            setTimeout(function() {
                                window.location.reload();
                            }, 5000);
                        }, 3000);
                    } else {
                        showStatus('❌ Update failed: ' + xhr.responseText, 'error');
                        uploadBtn.disabled = false;
                        uploadBtn.classList.remove('updating');
                    }
                });
                
                xhr.addEventListener('error', function() {
                    showStatus('❌ Upload error occurred', 'error');
                    uploadBtn.disabled = false;
                    uploadBtn.classList.remove('updating');
                });
                
                xhr.open('POST', '/update');
                xhr.send(formData);
                
            } catch (error) {
                showStatus('❌ Error: ' + error.message, 'error');
                uploadBtn.disabled = false;
                uploadBtn.classList.remove('updating');
            }
        };
        
        function showStatus(message, type) {
            const statusDiv = document.getElementById('statusMessage');
            statusDiv.textContent = message;
            statusDiv.className = 'status-message status-' + type;
            statusDiv.style.display = 'block';
        }
        
        function controlLED(state) {
            fetch('/led?state=' + state)
                .then(response => response.text())
                .then(data => {
                    if (state === 'blink') {
                        showStatus('🎨 LED Blink Pattern Started!', 'success');
                    } else {
                        showStatus('LED turned ' + state.toUpperCase(), 'success');
                    }
                    setTimeout(() => {
                        document.getElementById('statusMessage').style.display = 'none';
                    }, 2000);
                })
                .catch(error => {
                    showStatus('Failed to control LED', 'error');
                });
        }
    </script>
</body>
</html>
)rawliteral";

// ============================================
// Setup Function
// ============================================
void setup() {
  Serial.begin(115200);
  Serial.println("\n\n");
  Serial.println("╔═══════════════════════════════════════╗");
  Serial.println("║   ESP32 OTA System - VERSION 2.0.0   ║");
  Serial.println("║         ✨ NEW FEATURES! ✨          ║");
  Serial.println("╚═══════════════════════════════════════╝");
  Serial.println();
  Serial.print("Firmware Version: ");
  Serial.println(FIRMWARE_VERSION);
  Serial.print("Build Date: ");
  Serial.print(BUILD_DATE);
  Serial.print(" ");
  Serial.println(BUILD_TIME);
  Serial.println();
  Serial.println("🎉 What's New in v2.0.0:");
  Serial.println("  • Faster LED heartbeat (500ms)");
  Serial.println("  • Rainbow startup sequence");
  Serial.println("  • LED blink pattern feature");
  Serial.println("  • Enhanced UI design");
  Serial.println();
  
    // Initialize built-in LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
  
  // Enhanced startup sequence
  rainbowStartup();
  
  // Connect to WiFi
  setupWiFi();
  
  // Setup Arduino IDE OTA
  setupArduinoOTA();
  
  // Setup Web Server
  setupWebServer();
  
  Serial.println("\n✅ System Ready!");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.print("📡 Web Interface: http://");
  Serial.println(WiFi.localIP());
  Serial.print("🔧 Arduino OTA: ");
  Serial.println(ota_hostname);
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
}

// ============================================
// Main Loop
// ============================================
void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  
  // Faster heartbeat LED
  unsigned long currentMillis = millis();
  if (currentMillis - lastBlink >= blinkInterval) {
    lastBlink = currentMillis;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}

// ============================================
// WiFi Setup
// ============================================
void setupWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected!");
    Serial.print("   IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("   MAC Address: ");
    Serial.println(WiFi.macAddress());
    Serial.print("   RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    
    // Success pattern
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
  } else {
    Serial.println("\n❌ WiFi Connection Failed!");
  }
}

// ============================================
// Setup Arduino IDE OTA
// ============================================
void setupArduinoOTA() {
  ArduinoOTA.setHostname(ota_hostname);
  ArduinoOTA.setPassword(ota_password);
  
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("\n╔═════════════════════════════════════╗");
    Serial.println("║   OTA UPDATE (IDE) - v2.0.0        ║");
    Serial.println("╚═════════════════════════════════════╝");
    Serial.print("Type: ");
    Serial.println(type);
    updateStatus = "Updating";
    blinkInterval = 100;
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\n✅ OTA Update Complete!");
    updateStatus = "Complete";
    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int percentage = (progress / (total / 100));
    updateProgress = percentage;
    Serial.printf("Progress: %u%% (%u/%u bytes)\r", percentage, progress, total);
    if (percentage % 10 == 0) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("\n❌ OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    updateStatus = "Failed";
    blinkInterval = 500;
  });
  
  ArduinoOTA.begin();
  Serial.println("✅ Arduino IDE OTA Initialized (v2.0)");
}

// ============================================
// Setup Web Server
// ============================================
void setupWebServer() {
  server.on("/", HTTP_GET, []() {
    String html = String(indexHTML);
    html.replace("%FIRMWARE_VERSION%", FIRMWARE_VERSION);
    html.replace("%BUILD_DATE%", BUILD_DATE);
    html.replace("%BUILD_TIME%", BUILD_TIME);
    html.replace("%HOSTNAME%", ota_hostname);
    html.replace("%LOCAL_IP%", WiFi.localIP().toString());
    html.replace("%MAC_ADDRESS%", WiFi.macAddress());
    html.replace("%UPTIME%", getUptime());
    html.replace("%FREE_HEAP%", String(ESP.getFreeHeap() / 1024));
    server.send(200, "text/html", html);
  });
  
  // Enhanced LED control with blink pattern
  server.on("/led", HTTP_GET, []() {
    if (server.hasArg("state")) {
      String state = server.arg("state");
      if (state == "on") {
        digitalWrite(LED_PIN, HIGH);
        server.send(200, "text/plain", "LED ON");
        Serial.println("💡 LED turned ON");
      } else if (state == "off") {
        digitalWrite(LED_PIN, LOW);
        server.send(200, "text/plain", "LED OFF");
        Serial.println("💡 LED turned OFF");
      } else if (state == "blink") {
        // NEW FEATURE: Blink pattern
        Serial.println("🎨 LED Blink Pattern Started");
        for (int i = 0; i < 5; i++) {
          digitalWrite(LED_PIN, HIGH);
          delay(200);
          digitalWrite(LED_PIN, LOW);
          delay(200);
        }
        server.send(200, "text/plain", "LED Blink Complete");
        Serial.println("✅ Blink Pattern Complete");
      }
    } else {
      server.send(400, "text/plain", "Missing state parameter");
    }
  });
  
  // Firmware update endpoint
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
        Serial.println("\n╔═════════════════════════════════════╗");
        Serial.println("║   OTA UPDATE (WEB) - v2.0.0        ║");
        Serial.println("╚═════════════════════════════════════╝");
        Serial.printf("Filename: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
          Update.printError(Serial);
        }
        updateStatus = "Uploading";
        blinkInterval = 100;
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
        int progress = (Update.progress() * 100) / Update.size();
        updateProgress = progress;
        Serial.printf("Progress: %d%%\r", progress);
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
          Serial.printf("\n✅ Update Success! Size: %u bytes\n", upload.totalSize);
          updateStatus = "Complete";
        } else {
          Update.printError(Serial);
          updateStatus = "Failed";
        }
      }
    }
  );
  
  server.on("/status", HTTP_GET, []() {
    String json = "{";
    json += "\"version\":\"" + String(FIRMWARE_VERSION) + "\",";
    json += "\"uptime\":\"" + getUptime() + "\",";
    json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"updateStatus\":\"" + updateStatus + "\",";
    json += "\"updateProgress\":" + String(updateProgress);
    json += "}";
    server.send(200, "application/json", json);
  });
  
  server.begin();
  Serial.println("✅ Web Server Started (v2.0 Enhanced)");
}

// ============================================
// Helper Functions
// ============================================

// NEW: Rainbow startup sequence
void rainbowStartup() {
  Serial.println("🌈 Rainbow Startup Sequence...");
    for (int i = 0; i < 10; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);
        delay(100);
    }
}

String getUptime() {
  unsigned long seconds = millis() / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  
  seconds %= 60;
  minutes %= 60;
  hours %= 24;
  
  String uptime = "";
  if (days > 0) uptime += String(days) + "d ";
  if (hours > 0) uptime += String(hours) + "h ";
  if (minutes > 0) uptime += String(minutes) + "m ";
  uptime += String(seconds) + "s";
  
  return uptime;
}
