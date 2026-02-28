/*
 * ESP32 MQTT Weather Station
 * Lab-14 - SSG434
 * 
 * Features:
 * - BME280 sensor (Temperature, Humidity, Pressure) via I2C
 * - DHT11 sensor (Temperature, Humidity) on pin 15
 * - LED control on pin 12
 * - WiFi connectivity
 * - MQTT pub/sub for sensor data and control
 * - Configuration topics for remote settings
 * 
 * Hardware Connections:
 * - BME280: SCL -> GPIO 22, SDA -> GPIO 21
 * - DHT11: DATA -> GPIO 15
 * - LED: GPIO 12
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DHT.h>
#include <ArduinoJson.h>

// ============ WiFi Configuration ============
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ============ MQTT Configuration ============
const char* mqtt_server = "broker.hivemq.com";  // Free public broker
const int mqtt_port = 1883;
const char* mqtt_client_id = "ESP32_WeatherStation_";
const char* mqtt_user = "";  // Leave empty for public brokers
const char* mqtt_password = "";

// ============ MQTT Topics ============
// Publish topics (sensor data)
const char* topic_bme_temp = "esp32/weather/bme280/temperature";
const char* topic_bme_humidity = "esp32/weather/bme280/humidity";
const char* topic_bme_pressure = "esp32/weather/bme280/pressure";
const char* topic_dht_temp = "esp32/weather/dht11/temperature";
const char* topic_dht_humidity = "esp32/weather/dht11/humidity";
const char* topic_status = "esp32/weather/status";
const char* topic_all_data = "esp32/weather/all";

// Subscribe topics (control)
const char* topic_led_control = "esp32/weather/led/control";
const char* topic_config = "esp32/weather/config";
const char* topic_interval = "esp32/weather/config/interval";

// ============ Hardware Pin Definitions ============
#define DHT_PIN 15
#define DHT_TYPE DHT11
#define LED_PIN 12

// BME280 uses default I2C pins
#define I2C_SDA 21
#define I2C_SCL 22

// ============ Global Objects ============
WiFiClient espClient;
PubSubClient mqttClient(espClient);
Adafruit_BME280 bme;
DHT dht(DHT_PIN, DHT_TYPE);

// ============ Configuration Variables ============
unsigned long publishInterval = 10000;  // Default: publish every 10 seconds
unsigned long lastPublish = 0;
bool ledState = false;
bool bmeAvailable = false;
bool dhtAvailable = false;

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
  Serial.println("\n\n=== ESP32 MQTT Weather Station ===");
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Initialize I2C for BME280
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Initialize BME280
  Serial.print("Initializing BME280... ");
  if (bme.begin(0x76)) {  // Try address 0x76
    bmeAvailable = true;
    Serial.println("OK");
  } else if (bme.begin(0x77)) {  // Try address 0x77
    bmeAvailable = true;
    Serial.println("OK (at 0x77)");
  } else {
    Serial.println("FAILED - Check wiring!");
  }
  
  // Configure BME280 settings
  if (bmeAvailable) {
    bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                    Adafruit_BME280::SAMPLING_X2,  // Temperature
                    Adafruit_BME280::SAMPLING_X16, // Pressure
                    Adafruit_BME280::SAMPLING_X1,  // Humidity
                    Adafruit_BME280::FILTER_X16,
                    Adafruit_BME280::STANDBY_MS_0_5);
  }
  
  // Initialize DHT11
  Serial.print("Initializing DHT11... ");
  dht.begin();
  delay(2000);  // DHT needs time to stabilize
  float testTemp = dht.readTemperature();
  if (!isnan(testTemp)) {
    dhtAvailable = true;
    Serial.println("OK");
  } else {
    Serial.println("FAILED - Check wiring!");
  }
  
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
      mqttClient.publish(topic_status, "{\"status\":\"online\",\"device\":\"ESP32_Weather_Station\"}");
      
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
  
  // Read and publish BME280 data
  if (bmeAvailable) {
    float bmeTemp = bme.readTemperature();
    float bmeHumidity = bme.readHumidity();
    float bmePressure = bme.readPressure() / 100.0F;  // Convert to hPa
    
    if (!isnan(bmeTemp)) {
      char tempStr[10];
      dtostrf(bmeTemp, 6, 2, tempStr);
      mqttClient.publish(topic_bme_temp, tempStr);
      Serial.print("BME280 Temp: ");
      Serial.print(tempStr);
      Serial.println(" °C");
    }
    
    if (!isnan(bmeHumidity)) {
      char humStr[10];
      dtostrf(bmeHumidity, 6, 2, humStr);
      mqttClient.publish(topic_bme_humidity, humStr);
      Serial.print("BME280 Humidity: ");
      Serial.print(humStr);
      Serial.println(" %");
    }
    
    if (!isnan(bmePressure)) {
      char pressStr[10];
      dtostrf(bmePressure, 7, 2, pressStr);
      mqttClient.publish(topic_bme_pressure, pressStr);
      Serial.print("BME280 Pressure: ");
      Serial.print(pressStr);
      Serial.println(" hPa");
    }
  }
  
  // Read and publish DHT11 data
  if (dhtAvailable) {
    float dhtTemp = dht.readTemperature();
    float dhtHumidity = dht.readHumidity();
    
    if (!isnan(dhtTemp)) {
      char tempStr[10];
      dtostrf(dhtTemp, 6, 2, tempStr);
      mqttClient.publish(topic_dht_temp, tempStr);
      Serial.print("DHT11 Temp: ");
      Serial.print(tempStr);
      Serial.println(" °C");
    }
    
    if (!isnan(dhtHumidity)) {
      char humStr[10];
      dtostrf(dhtHumidity, 6, 2, humStr);
      mqttClient.publish(topic_dht_humidity, humStr);
      Serial.print("DHT11 Humidity: ");
      Serial.print(humStr);
      Serial.println(" %");
    }
  }
  
  Serial.println("------------------------------\n");
}

// ============================================
// Publish All Data as JSON
// ============================================
void publishAllData() {
  StaticJsonDocument<512> doc;
  
  // Device info
  doc["device"] = "ESP32_Weather_Station";
  doc["timestamp"] = millis();
  doc["uptime"] = millis() / 1000;
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["led_state"] = ledState;
  
  // BME280 data
  if (bmeAvailable) {
    JsonObject bme_data = doc.createNestedObject("bme280");
    bme_data["temperature"] = bme.readTemperature();
    bme_data["humidity"] = bme.readHumidity();
    bme_data["pressure"] = bme.readPressure() / 100.0F;
  }
  
  // DHT11 data
  if (dhtAvailable) {
    JsonObject dht_data = doc.createNestedObject("dht11");
    dht_data["temperature"] = dht.readTemperature();
    dht_data["humidity"] = dht.readHumidity();
  }
  
  // Serialize and publish
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  mqttClient.publish(topic_all_data, jsonBuffer);
  
  Serial.println("Combined JSON published:");
  Serial.println(jsonBuffer);
  Serial.println();
}
