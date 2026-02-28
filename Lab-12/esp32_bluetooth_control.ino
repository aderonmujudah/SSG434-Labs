/*
 * ESP32 Bluetooth Classic (BR/EDR) Controller
 * 
 * Features:
 * - Control 2 LEDs via Bluetooth commands
 * - Read DHT11 temperature and humidity sensor
 * - Command parser for incoming Bluetooth data
 * - Auto-send sensor data at configurable intervals
 * 
 * Hardware Connections:
 * - LED1: GPIO 2
 * - LED2: GPIO 4
 * - DHT11: GPIO 15
 * 
 * Commands:
 * - LED1:ON / LED1:OFF  - Control LED 1
 * - LED2:ON / LED2:OFF  - Control LED 2
 * - GET:SENSOR          - Request sensor data immediately
 * - AUTO:ON             - Enable auto-send sensor data (every 5 seconds)
 * - AUTO:OFF            - Disable auto-send sensor data
 * - STATUS              - Get all status information
 */

#include "BluetoothSerial.h"
#include "DHT.h"

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Pin definitions
#define LED1_PIN 12
#define LED2_PIN 13
#define DHT_PIN 15

// DHT sensor type
#define DHT_TYPE DHT11

// Auto-send interval (milliseconds)
#define AUTO_SEND_INTERVAL 5000

// Initialize Bluetooth Serial
BluetoothSerial SerialBT;

// Initialize DHT sensor
DHT dht(DHT_PIN, DHT_TYPE);

// State variables
bool led1State = false;
bool led2State = false;
bool autoSendEnabled = false;
unsigned long lastAutoSendTime = 0;

// Buffer for incoming commands
String inputBuffer = "";

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  
  // Initialize LED pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  
  // Initialize DHT sensor
  dht.begin();
  
  // Initialize Bluetooth
  SerialBT.begin("ESP32_BT_Controller"); // Bluetooth device name
  Serial.println("Bluetooth device started, you can pair it with your phone!");
  Serial.println("Device Name: ESP32_BT_Controller");
  
  // Send welcome message
  delay(1000);
  SerialBT.println("ESP32 Bluetooth Controller Ready!");
  SerialBT.println("Available Commands:");
  SerialBT.println("- LED1:ON / LED1:OFF");
  SerialBT.println("- LED2:ON / LED2:OFF");
  SerialBT.println("- GET:SENSOR");
  SerialBT.println("- AUTO:ON / AUTO:OFF");
  SerialBT.println("- STATUS");
}

void loop() {
  // Read incoming Bluetooth data
  while (SerialBT.available()) {
    char c = SerialBT.read();
    
    // Check for end of command (newline or carriage return)
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        processCommand(inputBuffer);
        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
    }
  }
  
  // Auto-send sensor data if enabled
  if (autoSendEnabled) {
    unsigned long currentTime = millis();
    if (currentTime - lastAutoSendTime >= AUTO_SEND_INTERVAL) {
      lastAutoSendTime = currentTime;
      sendSensorData();
    }
  }
  
  delay(10); // Small delay to prevent watchdog issues
}

// Command parser function
void processCommand(String command) {
  // Remove leading/trailing whitespace
  command.trim();
  
  // Convert to uppercase for case-insensitive comparison
  command.toUpperCase();
  
  Serial.println("Received command: " + command);
  
  // LED1 control
  if (command == "LED1:ON") {
    led1State = true;
    digitalWrite(LED1_PIN, HIGH);
    SerialBT.println("OK: LED1 turned ON");
    Serial.println("LED1 turned ON");
  }
  else if (command == "LED1:OFF") {
    led1State = false;
    digitalWrite(LED1_PIN, LOW);
    SerialBT.println("OK: LED1 turned OFF");
    Serial.println("LED1 turned OFF");
  }
  
  // LED2 control
  else if (command == "LED2:ON") {
    led2State = true;
    digitalWrite(LED2_PIN, HIGH);
    SerialBT.println("OK: LED2 turned ON");
    Serial.println("LED2 turned ON");
  }
  else if (command == "LED2:OFF") {
    led2State = false;
    digitalWrite(LED2_PIN, LOW);
    SerialBT.println("OK: LED2 turned OFF");
    Serial.println("LED2 turned OFF");
  }
  
  // Get sensor data
  else if (command == "GET:SENSOR") {
    sendSensorData();
  }
  
  // Auto-send control
  else if (command == "AUTO:ON") {
    autoSendEnabled = true;
    lastAutoSendTime = millis();
    SerialBT.println("OK: Auto-send enabled (5s interval)");
    Serial.println("Auto-send enabled");
  }
  else if (command == "AUTO:OFF") {
    autoSendEnabled = false;
    SerialBT.println("OK: Auto-send disabled");
    Serial.println("Auto-send disabled");
  }
  
  // Status request
  else if (command == "STATUS") {
    sendStatus();
  }
  
  // Unknown command
  else {
    SerialBT.println("ERROR: Unknown command - " + command);
    Serial.println("Unknown command: " + command);
  }
}

// Send sensor data function
void sendSensorData() {
  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Check if readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    SerialBT.println("ERROR: Failed to read from DHT sensor!");
    Serial.println("Failed to read DHT sensor");
    return;
  }
  
  // Send data in JSON-like format for easy parsing
  String sensorData = "SENSOR:{\"temp\":" + String(temperature, 1) + 
                      ",\"humidity\":" + String(humidity, 1) + 
                      ",\"unit\":\"C\"}";
  
  SerialBT.println(sensorData);
  Serial.println(sensorData);
}

// Send status function
void sendStatus() {
  // Read current sensor values
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Build status message
  String statusMsg = "STATUS:\n";
  statusMsg += "- LED1: " + String(led1State ? "ON" : "OFF") + "\n";
  statusMsg += "- LED2: " + String(led2State ? "ON" : "OFF") + "\n";
  statusMsg += "- Auto-send: " + String(autoSendEnabled ? "ENABLED" : "DISABLED") + "\n";
  
  if (!isnan(temperature) && !isnan(humidity)) {
    statusMsg += "- Temperature: " + String(temperature, 1) + " C\n";
    statusMsg += "- Humidity: " + String(humidity, 1) + " %";
  } else {
    statusMsg += "- Sensor: ERROR";
  }
  
  SerialBT.println(statusMsg);
  Serial.println(statusMsg);
}
