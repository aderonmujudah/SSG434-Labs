/*
 * Component Test Sketches
 * 
 * Use these simple test programs to verify each component works
 * before running the full project.
 */

// ============================================================================
// TEST 1: LED Blink Test
// ============================================================================
// Tests: Both LEDs are wired correctly and working
// Expected: LEDs should blink alternately

/*
void setup() {
  pinMode(2, OUTPUT);  // LED1
  pinMode(4, OUTPUT);  // LED2
}

void loop() {
  digitalWrite(2, HIGH);  // LED1 ON
  digitalWrite(4, LOW);   // LED2 OFF
  delay(500);
  
  digitalWrite(2, LOW);   // LED1 OFF
  digitalWrite(4, HIGH);  // LED2 ON
  delay(500);
}
*/

// ============================================================================
// TEST 2: DHT11 Sensor Test
// ============================================================================
// Tests: DHT11 sensor is wired correctly and reading data
// Expected: Temperature and humidity values in Serial Monitor

/*
#include "DHT.h"

#define DHT_PIN 15
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("DHT11 Test Starting...");
  dht.begin();
  delay(2000);  // DHT11 needs 2 seconds to stabilize
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("ERROR: Failed to read from DHT sensor!");
    Serial.println("Check wiring and pull-up resistor");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C  |  Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }
  
  delay(2000);
}
*/

// ============================================================================
// TEST 3: Bluetooth Test (Echo)
// ============================================================================
// Tests: Bluetooth is working and can send/receive data
// Expected: Whatever you send from phone echoes back

/*
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Test"); // Bluetooth device name
  Serial.println("Bluetooth Test Started");
  Serial.println("Connect with 'ESP32_Test'");
}

void loop() {
  // Echo from Bluetooth to Serial Monitor
  if (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.write(c);
    SerialBT.write(c);  // Echo back
  }
  
  // Echo from Serial Monitor to Bluetooth
  if (Serial.available()) {
    char c = Serial.read();
    SerialBT.write(c);
  }
}
*/

// ============================================================================
// TEST 4: LED Control via Bluetooth Test
// ============================================================================
// Tests: Bluetooth + LED control
// Expected: Send '1' to turn LED1 ON, '0' to turn OFF, '2' for LED2 ON, '3' for LED2 OFF

/*
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);  // LED1
  pinMode(4, OUTPUT);  // LED2
  
  SerialBT.begin("ESP32_LED_Test");
  Serial.println("Bluetooth LED Test");
  Serial.println("Send: 1=LED1 ON, 0=LED1 OFF, 2=LED2 ON, 3=LED2 OFF");
}

void loop() {
  if (SerialBT.available()) {
    char c = SerialBT.read();
    
    switch(c) {
      case '1':
        digitalWrite(2, HIGH);
        SerialBT.println("LED1 ON");
        Serial.println("LED1 ON");
        break;
      case '0':
        digitalWrite(2, LOW);
        SerialBT.println("LED1 OFF");
        Serial.println("LED1 OFF");
        break;
      case '2':
        digitalWrite(4, HIGH);
        SerialBT.println("LED2 ON");
        Serial.println("LED2 ON");
        break;
      case '3':
        digitalWrite(4, LOW);
        SerialBT.println("LED2 OFF");
        Serial.println("LED2 OFF");
        break;
      default:
        SerialBT.println("Unknown command");
        break;
    }
  }
}
*/

// ============================================================================
// TEST 5: Full Component Test (No Command Parser)
// ============================================================================
// Tests: All components working together
// Expected: LEDs blink, sensor data prints to Serial and Bluetooth every 3 seconds

/*
#include "BluetoothSerial.h"
#include "DHT.h"

#define LED1_PIN 2
#define LED2_PIN 4
#define DHT_PIN 15
#define DHT_TYPE DHT11

BluetoothSerial SerialBT;
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  
  dht.begin();
  SerialBT.begin("ESP32_Full_Test");
  
  Serial.println("Full Component Test");
  SerialBT.println("Full Component Test Started");
  delay(2000);
}

void loop() {
  // Blink LEDs alternately
  digitalWrite(LED1_PIN, HIGH);
  digitalWrite(LED2_PIN, LOW);
  delay(500);
  
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, HIGH);
  delay(500);
  
  // Read and send sensor data every 3 seconds
  static unsigned long lastRead = 0;
  if (millis() - lastRead >= 3000) {
    lastRead = millis();
    
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    if (!isnan(humidity) && !isnan(temperature)) {
      String data = "Temp: " + String(temperature, 1) + "°C, Humidity: " + String(humidity, 1) + "%";
      Serial.println(data);
      SerialBT.println(data);
    } else {
      Serial.println("Sensor Error");
      SerialBT.println("Sensor Error");
    }
  }
}
*/

// ============================================================================
// HOW TO USE THESE TESTS
// ============================================================================
/*
1. Choose the test you want to run
2. Remove the comment markers (* / and / *) around that test
3. Make sure only ONE test is uncommented at a time
4. Upload to your ESP32
5. Open Serial Monitor (115200 baud) to see results
6. For Bluetooth tests, also connect via Bluetooth app

TESTING ORDER (Recommended):
1. Start with TEST 1 (LED Blink) - simplest hardware test
2. Then TEST 2 (DHT11) - verify sensor works
3. Then TEST 3 (Bluetooth Echo) - verify Bluetooth works
4. Then TEST 4 (Bluetooth LED) - combine Bluetooth and LEDs
5. Finally TEST 5 (Full Test) - all components together
6. If all pass, upload the main project code!

TROUBLESHOOTING:
- If TEST 1 fails: Check LED polarity and resistors
- If TEST 2 fails: Check DHT11 wiring and pull-up resistor
- If TEST 3 fails: Check Bluetooth pairing and app
- If TEST 4 fails: Retest TEST 1 and TEST 3 individually
- If TEST 5 fails: Test each component individually first
*/

// ============================================================================
// EMPTY SETUP FOR COPY-PASTE
// ============================================================================

void setup() {
  // Uncomment one test above
}

void loop() {
  // Uncomment one test above
}
