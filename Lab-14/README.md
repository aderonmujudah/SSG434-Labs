# Lab-14: ESP32 MQTT Analog Sensor Station

## 📋 Overview

This lab implements an analog sensor station using ESP32, a hall sensor (HW-495), a microphone module (HW-485), and MQTT for real-time data publishing and remote control capabilities.

### Features

✅ **Dual Analog Sensor Setup**

- HW-495: Magnetic field (Analog)
- HW-485: Sound intensity (Analog)

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

| Component                | Quantity | Purpose                         |
| ------------------------ | -------- | ------------------------------- |
| ESP32 Development Board  | 1        | Main microcontroller            |
| HW-495 Hall Sensor       | 1        | Magnetic field (analog)         |
| HW-485 Microphone Module | 1        | Sound intensity (analog)        |
| LED                      | 1        | Status indicator / Control test |
| 220Ω Resistor            | 1        | LED current limiting            |
| Breadboard               | 1        | Circuit assembly                |
| Jumper Wires             | Several  | Connections                     |

### Pin Configuration

```
ESP32 Pin    →    Component
─────────────────────────────
3.3V          →   HW-495 VCC & HW-485 VCC
GND           →   HW-495 GND & HW-485 GND

GPIO 34       →   HW-495 AO (Analog Out)
GPIO 35       →   HW-485 AO (Analog Out)

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

3. **ArduinoJson** by Benoit Blanchon
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

1. Connect HW-495 hall sensor:
   - HW-495 VCC → ESP32 3.3V
   - HW-495 GND → ESP32 GND
   - HW-495 AO → ESP32 GPIO 34

2. Connect HW-485 microphone module:
   - HW-485 VCC → ESP32 3.3V
   - HW-485 GND → ESP32 GND
   - HW-485 AO → ESP32 GPIO 35

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
=== ESP32 MQTT Analog Sensor Station ===
ADC initialized for hall sensor and microphone
Connecting to WiFi: YourNetwork
WiFi connected!
IP address: 192.168.1.x
Connecting to MQTT broker... Connected!
Subscribed to control topics:
   - esp32/sensors/led/control
   - esp32/sensors/config
   - esp32/sensors/config/interval
```

---

## 📡 MQTT Topics

### Published Topics (Sensor Data)

| Topic                              | Description        | Data Format | Example               |
| ---------------------------------- | ------------------ | ----------- | --------------------- |
| `esp32/sensors/hall/raw`           | Hall raw ADC       | Integer     | `2310`                |
| `esp32/sensors/hall/voltage`       | Hall voltage       | Float (V)   | `1.86`                |
| `esp32/sensors/sound/raw`          | Mic raw ADC        | Integer     | `2075`                |
| `esp32/sensors/sound/level`        | Sound level        | Float       | `18.40`               |
| `esp32/sensors/sound/peak_to_peak` | Sound peak-to-peak | Integer     | `210`                 |
| `esp32/sensors/all`                | All data combined  | JSON        | See below             |
| `esp32/sensors/status`             | Device status      | JSON        | `{"status":"online"}` |

### Subscribed Topics (Control)

| Topic                           | Description             | Command Format | Examples                             |
| ------------------------------- | ----------------------- | -------------- | ------------------------------------ |
| `esp32/sensors/led/control`     | LED control             | Text           | `on`, `off`, `toggle`, `blink,5,500` |
| `esp32/sensors/config`          | General config          | JSON           | `{"interval":5000,"led":"on"}`       |
| `esp32/sensors/config/interval` | Update publish interval | Integer (ms)   | `15000`                              |

### Combined JSON Data Format

Note: `sound.level` is the average absolute deviation of ADC samples, so larger values indicate louder sound.

```json
{
  "device": "ESP32_Analog_Sensor_Station",
  "timestamp": 12345678,
  "uptime": 12345,
  "wifi_rssi": -45,
  "led_state": true,
  "hall": {
    "raw": 2310,
    "voltage": 1.86
  },
  "sound": {
    "raw": 2075,
    "level": 18.4,
    "peak_to_peak": 210
  }
}
```

---

## 🎮 Control Commands

### LED Control Commands

#### Basic Commands

```bash
# Turn LED ON
Publish to: esp32/sensors/led/control
Message: on

# Turn LED OFF
Publish to: esp32/sensors/led/control
Message: off

# Toggle LED state
Publish to: esp32/sensors/led/control
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
Publish to: esp32/sensors/config/interval
Message: 5000

# Set interval to 30 seconds
Message: 30000

# Valid range: 1000ms (1s) to 300000ms (5min)
```

#### Update Multiple Settings

```bash
# Update both interval and LED state
Publish to: esp32/sensors/config
Message: {"interval":15000,"led":"on"}
```

---

## 🧪 Testing with MQTT Clients

### Option 1: MQTT Explorer (GUI - Recommended)

1. Download from: http://mqtt-explorer.com/
2. Connect to your broker
3. Navigate to `esp32/sensors/` topics
4. View real-time sensor data
5. Publish control commands

### Option 2: MQTT.fx (GUI)

1. Download from: https://mqttfx.jensd.de/
2. Configure broker connection
3. Subscribe to `esp32/sensors/#` (all topics)
4. Publish to control topics

### Option 3: Mosquitto CLI (Command Line)

```bash
# Subscribe to all topics
mosquitto_sub -h broker.hivemq.com -t "esp32/sensors/#" -v

# Subscribe to specific sensor
mosquitto_sub -h broker.hivemq.com -t "esp32/sensors/hall/raw"

# Control LED - Turn ON
mosquitto_pub -h broker.hivemq.com -t "esp32/sensors/led/control" -m "on"

# Control LED - Turn OFF
mosquitto_pub -h broker.hivemq.com -t "esp32/sensors/led/control" -m "off"

# Set publish interval to 5 seconds
mosquitto_pub -h broker.hivemq.com -t "esp32/sensors/config/interval" -m "5000"

# Blink LED
mosquitto_pub -h broker.hivemq.com -t "esp32/sensors/led/control" -m "blink,3,500"
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
client.subscribe("esp32/sensors/#")

# Start listening
client.loop_start()

# Control LED
client.publish("esp32/sensors/led/control", "on")

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

**Problem:** Hall or microphone values are stuck

- ✅ Verify sensor power (3.3V and GND)
- ✅ Confirm AO pins on GPIO 34 (hall) and GPIO 35 (mic)
- ✅ Ensure sensors share common ground with ESP32
- ✅ Test with Serial Monitor to verify changing values

**Problem:** Microphone readings are noisy

- ✅ Keep analog wires short
- ✅ Use a stable 3.3V supply
- ✅ Avoid placing the mic near high-noise sources

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
- Create gauges for hall voltage and sound level
- Add controls for LED
- Setup charts for historical data

### 2. Home Assistant Integration

```yaml
sensor:
  - platform: mqtt
   name: "ESP32 Hall Voltage"
   state_topic: "esp32/sensors/hall/voltage"
   unit_of_measurement: "V"
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
- ✅ ADC configuration and analog sensor reading
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
- [ESP32 ADC Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html)
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
