# Configuration Guide - ESP32 MQTT Weather Station

## 📋 Table of Contents

1. [Initial Setup](#initial-setup)
2. [WiFi Configuration](#wifi-configuration)
3. [MQTT Broker Options](#mqtt-broker-options)
4. [Testing Configuration](#testing-configuration)
5. [Advanced Configuration](#advanced-configuration)
6. [Performance Tuning](#performance-tuning)

---

## 🚀 Initial Setup

### Prerequisites Checklist

- [ ] Arduino IDE installed (version 1.8.x or 2.x)
- [ ] ESP32 board support installed
- [ ] All required libraries installed
- [ ] Hardware assembled and tested
- [ ] USB cable connected
- [ ] Serial Monitor ready (115200 baud)

### Installing ESP32 Board Support

**Arduino IDE 1.8.x:**

1. Go to **File → Preferences**
2. Add to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Go to **Tools → Board → Boards Manager**
4. Search for "esp32"
5. Install "ESP32 by Espressif Systems"

**Arduino IDE 2.x:**

1. Click on Boards Manager icon (left sidebar)
2. Search "esp32"
3. Install "esp32 by Espressif Systems"

### Board Selection

```
Tools → Board → ESP32 Arduino → ESP32 Dev Module

Configuration:
├── Board: "ESP32 Dev Module"
├── Upload Speed: "921600"
├── CPU Frequency: "240MHz (WiFi/BT)"
├── Flash Frequency: "80MHz"
├── Flash Mode: "QIO"
├── Flash Size: "4MB (32Mb)"
├── Partition Scheme: "Default 4MB with spiffs"
└── Port: [Select your COM port]
```

---

## 📡 WiFi Configuration

### Basic WiFi Setup

Open the sketch and locate these lines:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

**Replace with your credentials:**

```cpp
const char* ssid = "MyHomeNetwork";
const char* password = "MySecurePassword123";
```

### Finding Your WiFi SSID

**Windows:**

```cmd
netsh wlan show interfaces
```

Look for "SSID" field

**Linux/Mac:**

```bash
# macOS
/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -I | grep SSID

# Linux
iwgetid -r
```

**Mobile Device:**

- Go to WiFi settings
- Current network name is your SSID

### WiFi Requirements

| Requirement         | Specification                                          |
| ------------------- | ------------------------------------------------------ |
| **Frequency**       | 2.4 GHz (ESP32 doesn't support 5 GHz)                  |
| **Security**        | WPA/WPA2 (Open networks supported but not recommended) |
| **DHCP**            | Enabled (or see Static IP section)                     |
| **Signal Strength** | At least -70 dBm for stable connection                 |

### Static IP Configuration (Optional)

If you need a fixed IP address:

```cpp
// Add after WiFi.begin()
#include <WiFi.h>

// Define static IP configuration
IPAddress local_IP(192, 168, 1, 184);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

void setupWiFi() {
  // Configure static IP
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Static IP configuration failed!");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // ... rest of code
}
```

### Multiple WiFi Networks (Failover)

```cpp
struct WiFiNetwork {
  const char* ssid;
  const char* password;
};

WiFiNetwork networks[] = {
  {"HomeNetwork", "password1"},
  {"BackupNetwork", "password2"},
  {"MobileHotspot", "password3"}
};

void setupWiFi() {
  for (int i = 0; i < 3; i++) {
    Serial.print("Trying: ");
    Serial.println(networks[i].ssid);

    WiFi.begin(networks[i].ssid, networks[i].password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected!");
      return;
    }
  }
  Serial.println("\nAll networks failed!");
}
```

---

## 🌐 MQTT Broker Options

### Option 1: Public MQTT Brokers (Easiest)

#### HiveMQ Public Broker (Recommended for Testing)

```cpp
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_password = "";
```

**Pros:** No setup required, always available
**Cons:** No security, public access, not for production

#### Eclipse Mosquitto Test Server

```cpp
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
```

#### EMQX Public Broker

```cpp
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
```

**⚠️ Public Broker Warning:**

- Anyone can subscribe to your topics
- Use unique topic names (include device ID)
- Don't send sensitive data
- Suitable only for testing/learning

---

### Option 2: Cloud MQTT Services (Recommended for Production)

#### CloudMQTT (Free tier available)

1. Sign up at: https://www.cloudmqtt.com/
2. Create new instance
3. Get connection details:

```cpp
const char* mqtt_server = "m10.cloudmqtt.com";  // Your server
const int mqtt_port = 12345;                     // Your port
const char* mqtt_user = "your_username";         // Your user
const char* mqtt_password = "your_password";     // Your password
```

#### HiveMQ Cloud (Free tier: 100 connections)

1. Sign up at: https://www.hivemq.com/mqtt-cloud-broker/
2. Create cluster
3. Configure:

```cpp
const char* mqtt_server = "your-cluster.hivemq.cloud";
const int mqtt_port = 8883;  // TLS port
const char* mqtt_user = "your_username";
const char* mqtt_password = "your_password";
```

#### AWS IoT Core

```cpp
// Requires certificate-based authentication
// See AWS IoT documentation for setup
const char* mqtt_server = "your-endpoint.iot.region.amazonaws.com";
const int mqtt_port = 8883;
```

---

### Option 3: Self-Hosted Mosquitto Broker (Most Control)

#### Installing Mosquitto on Raspberry Pi / Linux

```bash
# Update package list
sudo apt update

# Install Mosquitto broker and clients
sudo apt install -y mosquitto mosquitto-clients

# Enable and start service
sudo systemctl enable mosquitto
sudo systemctl start mosquitto

# Check status
sudo systemctl status mosquitto
```

#### Basic Mosquitto Configuration

Create/edit `/etc/mosquitto/mosquitto.conf`:

```conf
# Listen on all interfaces
listener 1883
allow_anonymous true

# Persistence
persistence true
persistence_location /var/lib/mosquitto/

# Logging
log_dest file /var/log/mosquitto/mosquitto.log
log_type all
```

Restart Mosquitto:

```bash
sudo systemctl restart mosquitto
```

#### ESP32 Configuration

```cpp
const char* mqtt_server = "192.168.1.100";  // Raspberry Pi IP
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_password = "";
```

#### Adding Authentication

Create password file:

```bash
# Create user with password
sudo mosquitto_passwd -c /etc/mosquitto/passwd username

# Add more users
sudo mosquitto_passwd /etc/mosquitto/passwd another_user
```

Update `/etc/mosquitto/mosquitto.conf`:

```conf
listener 1883
allow_anonymous false
password_file /etc/mosquitto/passwd
```

ESP32 Configuration:

```cpp
const char* mqtt_user = "username";
const char* mqtt_password = "your_password";
```

---

### Option 4: Docker Mosquitto Broker

```bash
# Pull Mosquitto image
docker pull eclipse-mosquitto

# Run broker
docker run -d \
  --name mosquitto \
  -p 1883:1883 \
  -v mosquitto-data:/mosquitto/data \
  eclipse-mosquitto

# View logs
docker logs -f mosquitto
```

---

## 🔧 Testing Configuration

### Step 1: Upload and Monitor

1. **Upload sketch** to ESP32
2. **Open Serial Monitor** (115200 baud)
3. **Expected output:**

```
=== ESP32 MQTT Weather Station ===
Initializing BME280... OK
Initializing DHT11... OK

Connecting to WiFi: MyHomeNetwork
.....
WiFi connected!
IP address: 192.168.1.50
Signal strength (RSSI): -45 dBm
Connecting to MQTT broker... Connected!
Subscribed to control topics:
  - esp32/weather/led/control
  - esp32/weather/config
  - esp32/weather/config/interval
Setup complete!
=====================================

--- Publishing Sensor Data ---
BME280 Temp: 23.45 °C
BME280 Humidity: 65.30 %
BME280 Pressure: 1013.25 hPa
DHT11 Temp: 24.00 °C
DHT11 Humidity: 60.00 %
------------------------------

Combined JSON published:
{"device":"ESP32_Weather_Station","timestamp":12345,"uptime":12...}
```

### Step 2: Test with MQTT Client

#### Option A: MQTT Explorer (GUI - Recommended)

1. **Download:** http://mqtt-explorer.com/
2. **Configure Connection:**
   ```
   Name: ESP32 Weather Station
   Host: broker.hivemq.com
   Port: 1883
   Username: (leave empty for public broker)
   Password: (leave empty)
   ```
3. **Connect**
4. **Navigate to:** esp32/weather/
5. **You should see:**
   - bme280/temperature
   - bme280/humidity
   - bme280/pressure
   - dht11/temperature
   - dht11/humidity
   - all
   - status

#### Option B: Mosquitto Command Line

**Subscribe to all topics:**

```bash
mosquitto_sub -h broker.hivemq.com -t "esp32/weather/#" -v
```

**Expected output:**

```
esp32/weather/bme280/temperature 23.45
esp32/weather/bme280/humidity 65.30
esp32/weather/bme280/pressure 1013.25
esp32/weather/dht11/temperature 24.00
esp32/weather/dht11/humidity 60.00
```

**Test LED control:**

```bash
# Turn on
mosquitto_pub -h broker.hivemq.com -t "esp32/weather/led/control" -m "on"

# Turn off
mosquitto_pub -h broker.hivemq.com -t "esp32/weather/led/control" -m "off"

# Toggle
mosquitto_pub -h broker.hivemq.com -t "esp32/weather/led/control" -m "toggle"

# Blink
mosquitto_pub -h broker.hivemq.com -t "esp32/weather/led/control" -m "blink,5,300"
```

#### Option C: Python Test Script

Create `test_mqtt.py`:

```python
import paho.mqtt.client as mqtt
import time

BROKER = "broker.hivemq.com"
PORT = 1883

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe("esp32/weather/#")
    print("Subscribed to esp32/weather/#")

def on_message(client, userdata, msg):
    print(f"📩 {msg.topic}: {msg.payload.decode()}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

print(f"Connecting to {BROKER}...")
client.connect(BROKER, PORT, 60)

# Start background thread
client.loop_start()

print("\n🎮 LED Control Menu:")
print("1 = LED ON")
print("2 = LED OFF")
print("3 = LED Toggle")
print("4 = LED Blink")
print("5 = Change interval to 5 seconds")
print("q = Quit\n")

while True:
    cmd = input("Enter command: ")

    if cmd == "1":
        client.publish("esp32/weather/led/control", "on")
        print("✅ LED ON command sent")
    elif cmd == "2":
        client.publish("esp32/weather/led/control", "off")
        print("✅ LED OFF command sent")
    elif cmd == "3":
        client.publish("esp32/weather/led/control", "toggle")
        print("✅ LED Toggle command sent")
    elif cmd == "4":
        client.publish("esp32/weather/led/control", "blink,5,500")
        print("✅ LED Blink command sent")
    elif cmd == "5":
        client.publish("esp32/weather/config/interval", "5000")
        print("✅ Interval changed to 5 seconds")
    elif cmd == "q":
        break

    time.sleep(0.1)

client.loop_stop()
client.disconnect()
print("Disconnected")
```

Run:

```bash
pip install paho-mqtt
python test_mqtt.py
```

---

## ⚙️ Advanced Configuration

### Custom Topic Names

Make topics unique by adding device ID:

```cpp
String deviceId = String(ESP.getEfuseMac(), HEX);

String topic_bme_temp = "esp32/" + deviceId + "/bme280/temperature";
String topic_led_control = "esp32/" + deviceId + "/led/control";
// ... update all topics
```

### QoS (Quality of Service) Levels

```cpp
// In setup(), when subscribing:
mqttClient.subscribe(topic_led_control, 1);  // QoS 1

// When publishing:
mqttClient.publish(topic_bme_temp, tempStr, false, 0);  // QoS 0
mqttClient.publish(topic_status, statusJson, true, 1);  // QoS 1, retained
```

**QoS Levels:**

- **0:** At most once (fire and forget)
- **1:** At least once (acknowledged)
- **2:** Exactly once (slowest, most reliable)

### Retained Messages

```cpp
// Last value retained on broker for new subscribers
mqttClient.publish(topic_status, "online", true);  // Retained = true
```

### Last Will and Testament (LWT)

Add to MQTT connection:

```cpp
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    String clientId = String(mqtt_client_id) + String(random(0xffff), HEX);

    // Set Last Will - sent if ESP32 disconnects unexpectedly
    if (mqttClient.connect(clientId.c_str(),
                           mqtt_user,
                           mqtt_password,
                           topic_status,        // LWT topic
                           0,                   // QoS
                           true,                // Retain
                           "{\"status\":\"offline\"}")) {  // LWT message
      Serial.println("Connected with LWT!");
      // ... rest of code
    }
  }
}
```

### TLS/SSL Encryption

For secure connections (CloudMQTT, AWS IoT, etc.):

```cpp
#include <WiFiClientSecure.h>

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

void setup() {
  // Add root CA certificate
  secureClient.setCACert(root_ca);

  mqttClient.setServer(mqtt_server, 8883);  // TLS port
  // ... rest of setup
}
```

---

## 📊 Performance Tuning

### Optimize Publish Interval

Default is 10 seconds. Adjust based on needs:

```cpp
// For real-time monitoring
unsigned long publishInterval = 1000;  // 1 second

// For battery-powered (with deep sleep)
unsigned long publishInterval = 300000;  // 5 minutes

// For data logging
unsigned long publishInterval = 60000;  // 1 minute
```

### Buffer Sizes

For large JSON payloads, increase buffer (in PubSubClient.h):

```cpp
#define MQTT_MAX_PACKET_SIZE 512  // Default is 256
```

Or set in code:

```cpp
mqttClient.setBufferSize(512);
```

### Connection Keepalive

```cpp
mqttClient.setKeepAlive(60);  // Seconds (default: 15)
```

### WiFi Power Management

```cpp
// For battery operation
WiFi.setSleep(true);

// For always-on operation (better responsiveness)
WiFi.setSleep(false);
```

---

## 🔍 Troubleshooting Configuration

### Issue: WiFi Won't Connect

**Check:**

```cpp
Serial.print("WiFi Status: ");
Serial.println(WiFi.status());
// 0 = WL_IDLE_STATUS
// 3 = WL_CONNECTED
// 4 = WL_CONNECT_FAILED
```

**Solutions:**

- Verify SSID and password
- Ensure 2.4 GHz network
- Check router firewall settings
- Move ESP32 closer to router

### Issue: MQTT Connection Fails

**Check return code:**

```cpp
Serial.print("MQTT State: ");
Serial.println(mqttClient.state());
```

**Return codes:**

- `-4` : Connection timeout
- `-3` : Connection lost
- `-2` : Connect failed
- `-1` : Disconnected
- `0` : Connected
- `1` : Bad protocol
- `2` : Client ID rejected
- `3` : Server unavailable
- `4` : Bad credentials
- `5` : Not authorized

### Issue: Messages Not Publishing

**Enable debug:**

```cpp
#define MQTT_DEBUG
```

**Check:**

- Topic name spelling
- QoS level compatibility
- Network connectivity
- Buffer size for large payloads

---

## 💾 Saving Configuration to EEPROM

For credential management without hardcoding:

```cpp
#include <Preferences.h>

Preferences preferences;

void saveCredentials() {
  preferences.begin("mqtt", false);
  preferences.putString("ssid", "YourSSID");
  preferences.putString("pass", "YourPassword");
  preferences.putString("broker", "broker.hivemq.com");
  preferences.end();
}

void loadCredentials() {
  preferences.begin("mqtt", true);
  String ssid = preferences.getString("ssid", "");
  String pass = preferences.getString("pass", "");
  String broker = preferences.getString("broker", "");
  preferences.end();
}
```

---

## 📱 Integration Examples

### Home Assistant Configuration

`configuration.yaml`:

```yaml
mqtt:
  sensor:
    - name: "ESP32 Temperature"
      state_topic: "esp32/weather/bme280/temperature"
      unit_of_measurement: "°C"
      device_class: "temperature"

    - name: "ESP32 Humidity"
      state_topic: "esp32/weather/bme280/humidity"
      unit_of_measurement: "%"
      device_class: "humidity"

  switch:
    - name: "ESP32 LED"
      command_topic: "esp32/weather/led/control"
      payload_on: "on"
      payload_off: "off"
```

### Node-RED Flow

Import this JSON:

```json
[
  {
    "id": "mqtt-in",
    "type": "mqtt in",
    "topic": "esp32/weather/#",
    "broker": "broker_config"
  },
  { "id": "debug", "type": "debug", "name": "" }
]
```

---

## ✅ Configuration Checklist

Before deployment:

- [ ] WiFi credentials configured
- [ ] MQTT broker accessible
- [ ] Unique client ID set
- [ ] Topics don't conflict with other devices
- [ ] QoS levels appropriate
- [ ] Security configured (TLS if needed)
- [ ] Publish interval optimized
- [ ] Last Will Testament configured
- [ ] Connection retry logic tested
- [ ] Error handling verified

---

**Configuration Complete! Ready for Deployment! 🎉**
