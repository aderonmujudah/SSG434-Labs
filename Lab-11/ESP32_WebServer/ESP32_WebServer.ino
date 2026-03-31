/*
 * ESP32 MQTT Analog Sensor Station
 * Lab-14 - SSG434
 * 
 * Features:
 * - HW-495 analog hall sensor (magnetic field) on ADC
 * - HW-485 microphone module (sound intensity) on ADC
 * - LED control on pin 12
 * - WiFi connectivity
 * - MQTT pub/sub for sensor data and control
 * - Configuration topics for remote settings
 * - Combined JSON data for both analog sensors
 * 
 * Hardware Connections:
 * - HW-495 Hall: AO -> GPIO 34 (ADC), VCC -> 3.3V, GND -> GND
 * - HW-485 Mic: AO -> GPIO 35 (ADC), VCC -> 3.3V, GND -> GND
 * - LED: GPIO 12
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ============ WiFi Configuration ============
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ============ MQTT Configuration ============
const char* mqtt_server = "broker.hivemq.com";  // Free public broker
const int mqtt_port = 1883;
const char* mqtt_client_id = "ESP32_AnalogSensor_";
const char* mqtt_user = "";  // Leave empty for public brokers
const char* mqtt_password = "";

// ============ MQTT Topics ============
// Publish topics (sensor data)
const char* topic_hall_raw = "esp32/sensors/hall/raw";
const char* topic_hall_voltage = "esp32/sensors/hall/voltage";
const char* topic_sound_raw = "esp32/sensors/sound/raw";
const char* topic_sound_level = "esp32/sensors/sound/level";
const char* topic_sound_peak = "esp32/sensors/sound/peak_to_peak";
const char* topic_status = "esp32/sensors/status";
const char* topic_all_data = "esp32/sensors/all";

// Subscribe topics (control)
const char* topic_led_control = "esp32/sensors/led/control";
const char* topic_config = "esp32/sensors/config";
const char* topic_interval = "esp32/sensors/config/interval";

// ============ Hardware Pin Definitions ============
#define LED_PIN 12
#define HALL_PIN 34
#define MIC_PIN 35

// ============ Global Objects ============
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ============ Configuration Variables ============
unsigned long publishInterval = 10000;  // Default: publish every 10 seconds
unsigned long lastPublish = 0;
bool ledState = false;
const int adcMaxValue = 4095;
const float adcReferenceVoltage = 3.3f;
const int soundSampleCount = 200;
const int soundSampleDelayUs = 200;

// ============ Function Declarations ============
void setupWiFi();
void reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void publishSensorData();
void publishAllData();
void handleLEDControl(String command);
void handleConfigUpdate(String config);

// ============================================
// Setup Function
// ============================================
void setup() {
  Serial.begin(115200);
  Serial.println("\n\n=== ESP32 MQTT Analog Sensor Station ===");
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Initialize ADC for analog sensors
  analogReadResolution(12);
  analogSetPinAttenuation(HALL_PIN, ADC_11db);
  analogSetPinAttenuation(MIC_PIN, ADC_11db);
  Serial.println("ADC initialized for hall sensor and microphone");
  
  // Connect to WiFi
  setupWiFi();
  
  // Setup MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setKeepAlive(60);
  
  // Generate unique client ID
  String clientId = String(mqtt_client_id) + String(random(0xffff), HEX);
  Serial.print("MQTT Client ID: ");
  Serial.println(clientId);
  
  // Initial LED blink to show ready
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
  
  Serial.println("Setup complete!");
  Serial.println("=====================================\n");
}

// ============================================
// Main Loop
// ============================================
void loop() {
  // Maintain WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    setupWiFi();
  }
  
  // Maintain MQTT connection
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
  
  // Publish sensor data at specified interval
  unsigned long currentMillis = millis();
  if (currentMillis - lastPublish >= publishInterval) {
    lastPublish = currentMillis;
    publishSensorData();
    publishAllData();
  }
}

// ============================================
// WiFi Setup
// ============================================
void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\nWiFi connection failed!");
  }
}

// ============================================
// MQTT Reconnect
// ============================================
void reconnectMQTT() {
  int attempts = 0;
  while (!mqttClient.connected() && attempts < 3) {
    Serial.print("Connecting to MQTT broker... ");
    
    String clientId = String(mqtt_client_id) + String(random(0xffff), HEX);
    
    if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("Connected!");
      
      // Subscribe to control topics
      mqttClient.subscribe(topic_led_control);
      mqttClient.subscribe(topic_config);
      mqttClient.subscribe(topic_interval);
      
      Serial.println("Subscribed to control topics:");
      Serial.println("  - " + String(topic_led_control));
      Serial.println("  - " + String(topic_config));
      Serial.println("  - " + String(topic_interval));
      
      // Publish online status
      mqttClient.publish(topic_status, "{\"status\":\"online\",\"device\":\"ESP32_Analog_Sensor_Station\"}");
      
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" - Retrying in 5 seconds");
      delay(5000);
    }
    attempts++;
  }
}

// ============================================
// MQTT Callback - Handle incoming messages
// ============================================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  
  // Convert payload to string
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Payload: ");
  Serial.println(message);
  
  // Handle LED control
  if (strcmp(topic, topic_led_control) == 0) {
    handleLEDControl(message);
  }
  
  // Handle configuration updates
  else if (strcmp(topic, topic_config) == 0) {
    handleConfigUpdate(message);
  }
  
  // Handle interval configuration
  else if (strcmp(topic, topic_interval) == 0) {
    int newInterval = message.toInt();
    if (newInterval >= 1000 && newInterval <= 300000) {  // 1 sec to 5 min
      publishInterval = newInterval;
      Serial.print("Publish interval updated to: ");
      Serial.print(publishInterval);
      Serial.println(" ms");
      
      String response = "{\"interval\":" + String(publishInterval) + ",\"status\":\"updated\"}";
      mqttClient.publish(topic_status, response.c_str());
    }
  }
}

// ============================================
// Handle LED Control Commands
// ============================================
void handleLEDControl(String command) {
  command.toLowerCase();
  command.trim();
  
  if (command == "on" || command == "1" || command == "true") {
    digitalWrite(LED_PIN, HIGH);
    ledState = true;
    Serial.println("LED turned ON");
    mqttClient.publish(topic_status, "{\"led\":\"on\"}");
  } 
  else if (command == "off" || command == "0" || command == "false") {
    digitalWrite(LED_PIN, LOW);
    ledState = false;
    Serial.println("LED turned OFF");
    mqttClient.publish(topic_status, "{\"led\":\"off\"}");
  }
  else if (command == "toggle") {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    Serial.print("LED toggled to: ");
    Serial.println(ledState ? "ON" : "OFF");
    mqttClient.publish(topic_status, ledState ? "{\"led\":\"on\"}" : "{\"led\":\"off\"}");
  }
  else if (command.startsWith("blink")) {
    // Blink command: "blink,5,500" = blink 5 times with 500ms interval
    int comma1 = command.indexOf(',');
    int comma2 = command.indexOf(',', comma1 + 1);
    
    int times = 3;  // Default
    int interval = 500;  // Default
    
    if (comma1 > 0) {
      times = command.substring(comma1 + 1, comma2 > 0 ? comma2 : command.length()).toInt();
    }
    if (comma2 > 0) {
      interval = command.substring(comma2 + 1).toInt();
    }
    
    Serial.print("Blinking LED ");
    Serial.print(times);
    Serial.print(" times with ");
    Serial.print(interval);
    Serial.println("ms interval");
    
    for (int i = 0; i < times; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(interval);
      digitalWrite(LED_PIN, LOW);
      delay(interval);
    }
    
    mqttClient.publish(topic_status, "{\"led\":\"blink_complete\"}");
  }
}

// ============================================
// Handle Configuration Updates
// ============================================
void handleConfigUpdate(String config) {
  // Parse JSON configuration
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, config);
  
  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }
  
  // Update interval if provided
  if (doc.containsKey("interval")) {
    int newInterval = doc["interval"];
    if (newInterval >= 1000 && newInterval <= 300000) {
      publishInterval = newInterval;
      Serial.print("Interval updated to: ");
      Serial.println(publishInterval);
    }
  }
  
  // Update LED state if provided
  if (doc.containsKey("led")) {
    String ledCmd = doc["led"].as<String>();
    handleLEDControl(ledCmd);
  }
  
  mqttClient.publish(topic_status, "{\"config\":\"updated\"}");
}

// ============================================
// Publish Sensor Data to Individual Topics
// ============================================
void publishSensorData() {
  Serial.println("\n--- Publishing Sensor Data ---");
  
  int hallRaw = analogRead(HALL_PIN);
  float hallVoltage = (static_cast<float>(hallRaw) / adcMaxValue) * adcReferenceVoltage;

  int micRaw = 0;
  int micMin = adcMaxValue;
  int micMax = 0;
  long deviationSum = 0;
  for (int i = 0; i < soundSampleCount; i++) {
    micRaw = analogRead(MIC_PIN);
    if (micRaw < micMin) micMin = micRaw;
    if (micRaw > micMax) micMax = micRaw;
    deviationSum += abs(micRaw - (adcMaxValue / 2));
    delayMicroseconds(soundSampleDelayUs);
  }
  int micPeakToPeak = micMax - micMin;
  float micLevel = static_cast<float>(deviationSum) / soundSampleCount;

  char hallRawStr[12];
  char hallVoltStr[12];
  char micRawStr[12];
  char micLevelStr[12];
  char micPeakStr[12];

  snprintf(hallRawStr, sizeof(hallRawStr), "%d", hallRaw);
  dtostrf(hallVoltage, 6, 3, hallVoltStr);
  snprintf(micRawStr, sizeof(micRawStr), "%d", micRaw);
  dtostrf(micLevel, 6, 2, micLevelStr);
  snprintf(micPeakStr, sizeof(micPeakStr), "%d", micPeakToPeak);

  mqttClient.publish(topic_hall_raw, hallRawStr);
  mqttClient.publish(topic_hall_voltage, hallVoltStr);
  mqttClient.publish(topic_sound_raw, micRawStr);
  mqttClient.publish(topic_sound_level, micLevelStr);
  mqttClient.publish(topic_sound_peak, micPeakStr);

  Serial.print("Hall Raw: ");
  Serial.print(hallRawStr);
  Serial.print(" | Hall Voltage: ");
  Serial.print(hallVoltStr);
  Serial.println(" V");

  Serial.print("Mic Raw: ");
  Serial.print(micRawStr);
  Serial.print(" | Sound Level: ");
  Serial.print(micLevelStr);
  Serial.print(" | Peak-to-Peak: ");
  Serial.println(micPeakStr);
  
  Serial.println("------------------------------\n");
}

// ============================================
// Publish All Data as JSON
// ============================================
void publishAllData() {
  StaticJsonDocument<512> doc;
  
  // Device info
  doc["device"] = "ESP32_Analog_Sensor_Station";
  doc["timestamp"] = millis();
  doc["uptime"] = millis() / 1000;
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["led_state"] = ledState;
  
  // Hall sensor data
  int hallRaw = analogRead(HALL_PIN);
  float hallVoltage = (static_cast<float>(hallRaw) / adcMaxValue) * adcReferenceVoltage;
  JsonObject hall_data = doc.createNestedObject("hall");
  hall_data["raw"] = hallRaw;
  hall_data["voltage"] = hallVoltage;

  // Microphone data
  int micRaw = 0;
  int micMin = adcMaxValue;
  int micMax = 0;
  long deviationSum = 0;
  for (int i = 0; i < soundSampleCount; i++) {
    micRaw = analogRead(MIC_PIN);
    if (micRaw < micMin) micMin = micRaw;
    if (micRaw > micMax) micMax = micRaw;
    deviationSum += abs(micRaw - (adcMaxValue / 2));
    delayMicroseconds(soundSampleDelayUs);
  }
  int micPeakToPeak = micMax - micMin;
  float micLevel = static_cast<float>(deviationSum) / soundSampleCount;
  JsonObject mic_data = doc.createNestedObject("sound");
  mic_data["raw"] = micRaw;
  mic_data["level"] = micLevel;
  mic_data["peak_to_peak"] = micPeakToPeak;
  
  // Serialize and publish
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  mqttClient.publish(topic_all_data, jsonBuffer);
  
  Serial.println("Combined JSON published:");
  Serial.println(jsonBuffer);
  Serial.println();
}
