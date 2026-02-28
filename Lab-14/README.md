# Lab-14: ESP32 MQTT Weather Station

## 📋 Overview

This lab implements a complete IoT weather station using ESP32, multiple sensors (BME280 and DHT11), and MQTT protocol for real-time data publishing and remote control capabilities.

### Features

✅ **Dual Sensor Setup**

- BME280: Temperature, Humidity, and Barometric Pressure (I2C)
- DHT11: Temperature and Humidity (Digital)

✅ **MQTT Communication**

- Publishes sensor data to individual topics
- Publishes combined JSON data
- Subscribes to control topics for LED management
- Configuration topics for remote settings

✅ **Remote Control**

- LED control via MQTT (ON/OFF/Toggle/Blink)
- Configurable publish interval
- Real-time status updates

✅ **Robust Connection Handling**

- Auto-reconnect for WiFi and MQTT
- Connection status monitoring
- Error handling and recovery

---

## 🔌 Hardware Requirements

### Components

| Component               | Quantity | Purpose                         |
| ----------------------- | -------- | ------------------------------- |
| ESP32 Development Board | 1        | Main microcontroller            |
| BME280 Sensor Module    | 1        | Temperature, humidity, pressure |
| DHT11 Sensor            | 1        | Temperature, humidity           |
| LED                     | 1        | Status indicator / Control test |
| 220Ω Resistor           | 1        | LED current limiting            |
| Breadboard              | 1        | Circuit assembly                |
| Jumper Wires            | Several  | Connections                     |

### Pin Configuration

```
ESP32 Pin    →    Component
─────────────────────────────
GPIO 21 (SDA) →   BME280 SDA
GPIO 22 (SCL) →   BME280 SCL
3.3V          →   BME280 VCC & DHT11 VCC
GND           →   BME280 GND & DHT11 GND

GPIO 15       →   DHT11 DATA

GPIO 12       →   LED + (via 220Ω resistor)
GND           →   LED -
```

---

## 📦 Required Libraries

Install these libraries via Arduino IDE Library Manager:

1. **WiFi** (Built-in with ESP32)
2. **PubSubClient** by Nick O'Leary

   ```
   Sketch → Include Library → Manage Libraries → Search "PubSubClient"
   ```

3. **Adafruit BME280** Library

   ```
   Search "Adafruit BME280" and install along with dependencies
   ```

4. **DHT sensor library** by Adafruit

   ```
   Search "DHT sensor library" and install along with dependencies
   ```

5. **ArduinoJson** by Benoit Blanchon
   ```
   Search "ArduinoJson" (install version 6.x)
   ```

---

## ⚙️ Configuration

### 1. WiFi Setup

Edit these lines in the sketch:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 2. MQTT Broker Setup

**Option A: Use Public Broker (Default)**

```cpp
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
```

**Option B: Use Private Broker**

```cpp
const char* mqtt_server = "your.mqtt.broker.com";
const int mqtt_port = 1883;
const char* mqtt_user = "your_username";
const char* mqtt_password = "your_password";
```

### Popular Public MQTT Brokers

| Broker            | Address            | Port | Authentication |
| ----------------- | ------------------ | ---- | -------------- |
| HiveMQ            | broker.hivemq.com  | 1883 | None           |
| Eclipse Mosquitto | test.mosquitto.org | 1883 | None           |
| EMQX              | broker.emqx.io     | 1883 | None           |

---

## 🚀 Getting Started

### Step 1: Hardware Assembly

1. Connect BME280 sensor to ESP32 via I2C:
   - BME280 VCC → ESP32 3.3V
   - BME280 GND → ESP32 GND
   - BME280 SDA → ESP32 GPIO 21
   - BME280 SCL → ESP32 GPIO 22

2. Connect DHT11 sensor:
   - DHT11 VCC → ESP32 3.3V
   - DHT11 GND → ESP32 GND
   - DHT11 DATA → ESP32 GPIO 15

3. Connect LED:
   - LED Anode (+) → 220Ω resistor → ESP32 GPIO 12
   - LED Cathode (-) → ESP32 GND

### Step 2: Software Setup

1. Open `ESP32_MQTT_Weather_Station.ino` in Arduino IDE
2. Select Board: **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
3. Configure your WiFi credentials
4. Configure MQTT broker settings
5. Upload the sketch to ESP32

### Step 3: Monitor Serial Output

Open Serial Monitor (115200 baud) to see:

- WiFi connection status
- MQTT connection status
- Sensor readings
- MQTT message logs

Expected output:

```
=== ESP32 MQTT Weather Station ===
Initializing BME280... OK
Initializing DHT11... OK
Connecting to WiFi: YourNetwork
WiFi connected!
IP address: 192.168.1.x
Connecting to MQTT broker... Connected!
Subscribed to control topics:
  - esp32/weather/led/control
  - esp32/weather/config
  - esp32/weather/config/interval
```

---

## 📡 MQTT Topics

### Published Topics (Sensor Data)

| Topic                              | Description        | Data Format | Example               |
| ---------------------------------- | ------------------ | ----------- | --------------------- |
| `esp32/weather/bme280/temperature` | BME280 temperature | Float (°C)  | `23.45`               |
| `esp32/weather/bme280/humidity`    | BME280 humidity    | Float (%)   | `65.30`               |
| `esp32/weather/bme280/pressure`    | BME280 pressure    | Float (hPa) | `1013.25`             |
| `esp32/weather/dht11/temperature`  | DHT11 temperature  | Float (°C)  | `24.00`               |
| `esp32/weather/dht11/humidity`     | DHT11 humidity     | Float (%)   | `60.00`               |
| `esp32/weather/all`                | All data combined  | JSON        | See below             |
| `esp32/weather/status`             | Device status      | JSON        | `{"status":"online"}` |

### Subscribed Topics (Control)

| Topic                           | Description             | Command Format | Examples                             |
| ------------------------------- | ----------------------- | -------------- | ------------------------------------ |
| `esp32/weather/led/control`     | LED control             | Text           | `on`, `off`, `toggle`, `blink,5,500` |
| `esp32/weather/config`          | General config          | JSON           | `{"interval":5000,"led":"on"}`       |
| `esp32/weather/config/interval` | Update publish interval | Integer (ms)   | `15000`                              |

### Combined JSON Data Format

```json
{
  "device": "ESP32_Weather_Station",
  "timestamp": 12345678,
  "uptime": 12345,
  "wifi_rssi": -45,
  "led_state": true,
  "bme280": {
    "temperature": 23.45,
    "humidity": 65.3,
    "pressure": 1013.25
  },
  "dht11": {
    "temperature": 24.0,
    "humidity": 60.0
  }
}
```

---

## 🎮 Control Commands

### LED Control Commands

#### Basic Commands

```bash
# Turn LED ON
Publish to: esp32/weather/led/control
Message: on

# Turn LED OFF
Publish to: esp32/weather/led/control
Message: off

# Toggle LED state
Publish to: esp32/weather/led/control
Message: toggle
```

#### Blink Command

```bash
# Blink LED (default: 3 times, 500ms interval)
Message: blink

# Blink 5 times with 300ms interval
Message: blink,5,300

# Format: blink,<times>,<interval_ms>
```

### Configuration Commands

#### Update Publish Interval

```bash
# Set interval to 5 seconds
Publish to: esp32/weather/config/interval
Message: 5000

# Set interval to 30 seconds
Message: 30000

# Valid range: 1000ms (1s) to 300000ms (5min)
```

#### Update Multiple Settings

```bash
# Update both interval and LED state
Publish to: esp32/weather/config
Message: {"interval":15000,"led":"on"}
```

---

## 🧪 Testing with MQTT Clients

### Option 1: MQTT Explorer (GUI - Recommended)

1. Download from: http://mqtt-explorer.com/
2. Connect to your broker
3. Navigate to `esp32/weather/` topics
4. View real-time sensor data
5. Publish control commands

### Option 2: MQTT.fx (GUI)

1. Download from: https://mqttfx.jensd.de/
2. Configure broker connection
3. Subscribe to `esp32/weather/#` (all topics)
4. Publish to control topics

### Option 3: Mosquitto CLI (Command Line)

```bash
# Subscribe to all topics
mosquitto_sub -h broker.hivemq.com -t "esp32/weather/#" -v

# Subscribe to specific sensor
mosquitto_sub -h broker.hivemq.com -t "esp32/weather/bme280/temperature"

# Control LED - Turn ON
mosquitto_pub -h broker.hivemq.com -t "esp32/weather/led/control" -m "on"

# Control LED - Turn OFF
mosquitto_pub -h broker.hivemq.com -t "esp32/weather/led/control" -m "off"

# Set publish interval to 5 seconds
mosquitto_pub -h broker.hivemq.com -t "esp32/weather/config/interval" -m "5000"

# Blink LED
mosquitto_pub -h broker.hivemq.com -t "esp32/weather/led/control" -m "blink,3,500"
```

### Option 4: Python MQTT Client

```python
import paho.mqtt.client as mqtt

def on_message(client, userdata, message):
    print(f"Topic: {message.topic}")
    print(f"Message: {message.payload.decode()}\n")

client = mqtt.Client()
client.on_message = on_message
client.connect("broker.hivemq.com", 1883)
client.subscribe("esp32/weather/#")

# Start listening
client.loop_start()

# Control LED
client.publish("esp32/weather/led/control", "on")

# Keep running
input("Press Enter to stop...\n")
client.loop_stop()
```

---

## 🔍 Troubleshooting

### WiFi Issues

**Problem:** WiFi not connecting

- ✅ Verify SSID and password are correct
- ✅ Check if ESP32 is within WiFi range
- ✅ Ensure 2.4GHz WiFi (ESP32 doesn't support 5GHz)
- ✅ Check router firewall settings

### MQTT Issues

**Problem:** MQTT connection fails

- ✅ Verify broker address and port
- ✅ Check if broker requires authentication
- ✅ Test broker with MQTT client (MQTT Explorer)
- ✅ Ensure firewall allows port 1883

**Problem:** Not receiving messages

- ✅ Verify topic names match exactly (case-sensitive)
- ✅ Check MQTT client is subscribed to correct topics
- ✅ Monitor Serial output for publish confirmations

### Sensor Issues

**Problem:** BME280 initialization failed

- ✅ Check I2C wiring (SDA, SCL)
- ✅ Verify sensor address (0x76 or 0x77)
- ✅ Try I2C scanner sketch to detect address
- ✅ Check power supply (3.3V, not 5V)

**Problem:** DHT11 returns NaN

- ✅ Check data pin connection (GPIO 15)
- ✅ Verify power connections
- ✅ Wait 2 seconds after initialization
- ✅ Consider replacing sensor if persistent

### LED Issues

**Problem:** LED not responding

- ✅ Check LED polarity (+ to resistor, - to GND)
- ✅ Verify resistor value (220Ω recommended)
- ✅ Test with simple LED blink sketch
- ✅ Check GPIO 12 connection

---

## 📊 Monitoring Dashboard Ideas

### 1. Node-RED Dashboard

- Import sensor data via MQTT
- Create gauges for temperature, humidity, pressure
- Add controls for LED
- Setup charts for historical data

### 2. Home Assistant Integration

```yaml
sensor:
  - platform: mqtt
    name: "ESP32 Temperature"
    state_topic: "esp32/weather/bme280/temperature"
    unit_of_measurement: "°C"
```

### 3. Grafana + InfluxDB

- Use Telegraf MQTT consumer
- Store time-series data in InfluxDB
- Visualize with Grafana dashboards

---

## 🔐 Security Recommendations

For production use:

1. **Use TLS/SSL** for MQTT (port 8883)
2. **Set strong passwords** for MQTT authentication
3. **Use unique client IDs** (already implemented with random suffix)
4. **Restrict topic permissions** on MQTT broker
5. **Don't hardcode credentials** - use EEPROM or SPIFFS
6. **Implement OTA updates** for firmware

---

## 📚 Learning Objectives

By completing this lab, you will learn:

- ✅ ESP32 WiFi configuration and management
- ✅ MQTT protocol basics (pub/sub model)
- ✅ I2C communication with BME280
- ✅ Digital sensor reading (DHT11)
- ✅ JSON data formatting and parsing
- ✅ Remote device control via MQTT
- ✅ IoT system architecture
- ✅ Real-time data publishing
- ✅ Error handling and auto-reconnection

---

## 🎓 Extension Ideas

1. **Add more sensors**: Light sensor (LDR), motion sensor (PIR)
2. **Implement data logging**: Store readings to SD card
3. **Create web dashboard**: Use ESP32 as web server
4. **Add OTA updates**: Update firmware over WiFi
5. **Battery operation**: Add deep sleep mode for power saving
6. **Alert system**: Send notifications when thresholds exceeded
7. **Multi-device**: Connect multiple ESP32 stations
8. **Time synchronization**: Add NTP for accurate timestamps

---

## 📖 References

- [ESP32 Arduino Core Documentation](https://docs.espressif.com/projects/arduino-esp32/)
- [PubSubClient Library](https://pubsubclient.knolleary.net/)
- [BME280 Datasheet](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/)
- [MQTT Protocol Specification](https://mqtt.org/)
- [ArduinoJson Documentation](https://arduinojson.org/)

---

## 📄 License

This project is created for educational purposes as part of SSG434 coursework.

---

## 👨‍💻 Support

For issues or questions:

1. Check the Serial Monitor output
2. Review the troubleshooting section
3. Test with MQTT client tools
4. Verify hardware connections

**Happy IoT Development! 🚀**
