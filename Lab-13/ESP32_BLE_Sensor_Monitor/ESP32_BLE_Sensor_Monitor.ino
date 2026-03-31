/*
 * ESP32 BLE Sensor Monitoring System
 * SSG434 Lab 13
 * 
 * Features:
 * - BLE communication with mobile devices
 * - DHT11 temperature and humidity monitoring
 * - LED control via BLE
 * - Automatic sensor data notifications
 * - Device information service
 * 
 * Hardware Connections:
 * - LED: GPIO 12
 * - DHT11: GPIO 15
 */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <DHT.h>

// Hardware Pin Definitions
#define LED_PIN 12
#define DHT_PIN 15
#define DHT_TYPE DHT22


// BLE Service UUIDs
#define DEVICE_INFO_SERVICE_UUID        "180A"  // Standard Device Information Service
#define DEVICE_NAME_CHAR_UUID           "2A00"  // Device Name
#define MANUFACTURER_CHAR_UUID          "2A29"  // Manufacturer Name

#define SENSOR_SERVICE_UUID             "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define TEMPERATURE_CHAR_UUID           "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define HUMIDITY_CHAR_UUID              "beb5483e-36e1-4688-b7f5-ea07361b26a9"

#define CONTROL_SERVICE_UUID            "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define LED_CONTROL_CHAR_UUID           "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define LED_STATUS_CHAR_UUID            "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

// Global Variables
DHT dht(DHT_PIN, DHT_TYPE);
BLEServer *pServer = NULL;
BLECharacteristic *pTemperatureCharacteristic = NULL;
BLECharacteristic *pHumidityCharacteristic = NULL;
BLECharacteristic *pLedControlCharacteristic = NULL;
BLECharacteristic *pLedStatusCharacteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;
bool ledState = false;
float lastTemperature = 0;
float lastHumidity = 0;

unsigned long lastSensorUpdate = 0;
const long sensorUpdateInterval = 2000;  // Update every 2 seconds

// Server Callbacks
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Client Connected!");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Client Disconnected!");
    }
};

// LED Control Callback
class LedControlCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      uint8_t* data = pCharacteristic->getData();
      size_t len = pCharacteristic->getValue().length();
      
      if (len > 0) {
        Serial.print("LED Control received: ");
        
        // Convert to Arduino String for easier comparison
        String value = "";
        for (int i = 0; i < len; i++) {
          value += (char)data[i];
        }
        
        Serial.println(value);
        
        // Check command
        if (value == "1" || value == "ON" || value == "on") {
          ledState = true;
          digitalWrite(LED_PIN, HIGH);
          Serial.println("LED ON");
        } 
        else if (value == "0" || value == "OFF" || value == "off") {
          ledState = false;
          digitalWrite(LED_PIN, LOW);
          Serial.println("LED OFF");
        }
        else if (value == "TOGGLE" || value == "toggle") {
          ledState = !ledState;
          digitalWrite(LED_PIN, ledState ? HIGH : LOW);
          Serial.println(ledState ? "LED ON (toggled)" : "LED OFF (toggled)");
        }
        
        // Update LED status characteristic
        String statusMsg = ledState ? "ON" : "OFF";
        pLedStatusCharacteristic->setValue(statusMsg.c_str());
        pLedStatusCharacteristic->notify();
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32 BLE Sensor Monitor ===");
  
  // Initialize Hardware
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  dht.begin();
  Serial.println("DHT11 sensor initialized");
  
  // Create BLE Device
  BLEDevice::init("ESP32_Sensor_Monitor");
  
  // Create BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  // ============ Device Information Service ============
  BLEService *pDeviceInfoService = pServer->createService(DEVICE_INFO_SERVICE_UUID);
  
  BLECharacteristic *pDeviceNameChar = pDeviceInfoService->createCharacteristic(
    DEVICE_NAME_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ
  );
  pDeviceNameChar->setValue("ESP32_Sensor_Monitor");
  
  BLECharacteristic *pManufacturerChar = pDeviceInfoService->createCharacteristic(
    MANUFACTURER_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ
  );
  pManufacturerChar->setValue("SSG434 Labs");
  
  pDeviceInfoService->start();
  Serial.println("Device Info Service started");
  
  // ============ Sensor Service ============
  BLEService *pSensorService = pServer->createService(SENSOR_SERVICE_UUID);
  
  // Temperature Characteristic with Notify
  pTemperatureCharacteristic = pSensorService->createCharacteristic(
    TEMPERATURE_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | 
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pTemperatureCharacteristic->addDescriptor(new BLE2902());
  pTemperatureCharacteristic->setValue("0.0");
  
  // Humidity Characteristic with Notify
  pHumidityCharacteristic = pSensorService->createCharacteristic(
    HUMIDITY_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | 
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pHumidityCharacteristic->addDescriptor(new BLE2902());
  pHumidityCharacteristic->setValue("0.0");
  
  pSensorService->start();
  Serial.println("Sensor Service started");
  
  // ============ Control Service ============
  BLEService *pControlService = pServer->createService(CONTROL_SERVICE_UUID);
  
  // LED Control Characteristic (Write)
  pLedControlCharacteristic = pControlService->createCharacteristic(
    LED_CONTROL_CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  pLedControlCharacteristic->setCallbacks(new LedControlCallbacks());
  
  // LED Status Characteristic (Read + Notify)
  pLedStatusCharacteristic = pControlService->createCharacteristic(
    LED_STATUS_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | 
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pLedStatusCharacteristic->addDescriptor(new BLE2902());
  pLedStatusCharacteristic->setValue("OFF");
  
  pControlService->start();
  Serial.println("Control Service started");
  
  // Start Advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SENSOR_SERVICE_UUID);
  pAdvertising->addServiceUUID(CONTROL_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("\n✓ BLE Advertising started");
  Serial.println("Waiting for client connection...");
  Serial.println("\nDevice Name: ESP32_Sensor_Monitor");
  Serial.println("Ready to accept connections!\n");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Read and update sensor data
  if (currentMillis - lastSensorUpdate >= sensorUpdateInterval) {
    lastSensorUpdate = currentMillis;
    
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    
    // Check if readings are valid
    if (!isnan(temperature) && !isnan(humidity)) {
      // Update only if values changed significantly (reduce unnecessary notifications)
      if (abs(temperature - lastTemperature) > 0.1 || 
          abs(humidity - lastHumidity) > 0.1) {
        
        lastTemperature = temperature;
        lastHumidity = humidity;
        
        // Print to serial
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print("°C | Humidity: ");
        Serial.print(humidity);
        Serial.println("%");
        
        // If client is connected, update characteristics and notify
        if (deviceConnected) {
          // Update temperature
          char tempStr[8];
          dtostrf(temperature, 4, 1, tempStr);
          pTemperatureCharacteristic->setValue(tempStr);
          pTemperatureCharacteristic->notify();
          
          // Update humidity
          char humStr[8];
          dtostrf(humidity, 4, 1, humStr);
          pHumidityCharacteristic->setValue(humStr);
          pHumidityCharacteristic->notify();
        }
      }
    } else {
      Serial.println("Failed to read from DHT sensor!");
    }
  }
  
  // Handle connection/disconnection
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);
    pServer->startAdvertising();
    Serial.println("Restarting advertising...");
    oldDeviceConnected = deviceConnected;
  }
  
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
  
  delay(10);
}
