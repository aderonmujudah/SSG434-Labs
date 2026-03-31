# Quick Start Guide - ESP32 MQTT Analog Sensor Station

Get your analog sensor station up and running in **15 minutes**! ⚡

---

## 🎯 Prerequisites (5 minutes)

### 1. Install Arduino IDE

- Download from: https://www.arduino.cc/en/software
- Install ESP32 board support

### 2. Install Required Libraries

Open **Sketch → Include Library → Manage Libraries** and install:

| Library Name     | Author          |
| ---------------- | --------------- |
| **PubSubClient** | Nick O'Leary    |
| **ArduinoJson**  | Benoit Blanchon |

> **Tip:** Also install dependencies when prompted.

---

## 🔌 Hardware Connections (5 minutes)

### Quick Wiring Table

| Component  | Pin           | Connect To              | Notes          |
| ---------- | ------------- | ----------------------- | -------------- |
| **HW-495** | VCC           | ESP32 3.3V              | Analog hall    |
|            | GND           | ESP32 GND               |                |
|            | AO            | ESP32 GPIO 34           | ADC input      |
| **HW-485** | VCC           | ESP32 3.3V              | Microphone     |
|            | GND           | ESP32 GND               |                |
|            | AO            | ESP32 GPIO 35           | ADC input      |
| **LED**    | + (long leg)  | 220Ω resistor → GPIO 12 | With resistor! |
|            | - (short leg) | ESP32 GND               |                |

### Visual Quick Reference

```
ESP32          Component
─────          ─────────
3.3V    ────→  HW-495 VCC, HW-485 VCC
GND     ────→  HW-495 GND, HW-485 GND, LED -
GPIO 34 ────→  HW-495 AO
GPIO 35 ────→  HW-485 AO
GPIO 12 ────→  [220Ω] → LED +
```

---

## ⚙️ Configuration (2 minutes)

### 1. Open the Sketch

Open `ESP32_MQTT_Weather_Station.ino` in Arduino IDE

### 2. Update WiFi Credentials (Lines 23-24)

```cpp
const char* ssid = "YOUR_WIFI_SSID";        // ← Change this
const char* password = "YOUR_WIFI_PASSWORD"; // ← Change this
```

Replace with your actual WiFi name and password.

### 3. Choose MQTT Broker (Line 27)

**Option A:** Use default public broker (easiest for testing)

```cpp
const char* mqtt_server = "broker.hivemq.com";  // ← Leave as is
```

**Option B:** Use your own broker

```cpp
const char* mqtt_server = "your.broker.com";  // ← Change to your broker
```

---

## 🚀 Upload & Run (3 minutes)

### 1. Select Board

```
Tools → Board → ESP32 Arduino → ESP32 Dev Module
```

### 2. Select Port

```
Tools → Port → COM# (Windows) or /dev/ttyUSB# (Linux/Mac)
```

### 3. Upload

Click **Upload** button (→) or press `Ctrl+U`

Wait for "Done uploading" message.

### 4. Open Serial Monitor

```
Tools → Serial Monitor
Set baud rate to: 115200
```

### 5. Expected Output ✅

```
=== ESP32 MQTT Analog Sensor Station ===
ADC initialized for hall sensor and microphone

Connecting to WiFi: YourNetwork
.....
WiFi connected!
IP address: 192.168.1.50

Connecting to MQTT broker... Connected!
Subscribed to control topics

--- Publishing Sensor Data ---
Hall Raw: 2310 | Hall Voltage: 1.860 V
Mic Raw: 2075 | Sound Level: 18.40 | Peak-to-Peak: 210
------------------------------
```

---

## 🧪 Test It! (2 minutes)

### Method 1: MQTT Explorer (Recommended)

1. **Download:** http://mqtt-explorer.com/
2. **Configure:**
   - Host: `broker.hivemq.com`
   - Port: `1883`
3. **Connect**
4. **Navigate:** `esp32/sensors/`
5. **See:** Live sensor data! 📊

### Method 2: Command Line (Quick Test)

**Subscribe to sensor data:**

```bash
mosquitto_sub -h broker.hivemq.com -t "esp32/sensors/#" -v
```

**Control the LED:**

```bash
# Turn LED on
mosquitto_pub -h broker.hivemq.com -t "esp32/sensors/led/control" -m "on"

# Turn LED off
mosquitto_pub -h broker.hivemq.com -t "esp32/sensors/led/control" -m "off"

# Make it blink 5 times
mosquitto_pub -h broker.hivemq.com -t "esp32/sensors/led/control" -m "blink,5,500"
```

---

## 🎮 Control Commands

### LED Controls

| Command | Publish To                  | Message       |
| ------- | --------------------------- | ------------- |
| LED On  | `esp32/sensors/led/control` | `on`          |
| LED Off | `esp32/sensors/led/control` | `off`         |
| Toggle  | `esp32/sensors/led/control` | `toggle`      |
| Blink   | `esp32/sensors/led/control` | `blink,3,500` |

### Configuration

| Command                | Publish To                      | Message |
| ---------------------- | ------------------------------- | ------- |
| Change interval to 5s  | `esp32/sensors/config/interval` | `5000`  |
| Change interval to 30s | `esp32/sensors/config/interval` | `30000` |

---

## 📡 MQTT Topics Reference

### Published (Sensor Data)

```
esp32/sensors/
├── hall/
│   ├── raw            (Integer, ADC)
│   └── voltage        (Float, V)
├── sound/
│   ├── raw            (Integer, ADC)
│   ├── level          (Float)
│   └── peak_to_peak   (Integer)
├── all                (JSON with all data)
└── status             (Device status)
```

### Subscribed (Control)

```
esp32/sensors/
├── led/control        (on/off/toggle/blink)
├── config             (JSON configuration)
└── config/interval    (Integer, milliseconds)
```

---

## ❗ Common Issues & Quick Fixes

### ❌ Issue: WiFi won't connect

**Fix:**

- Double-check WiFi name and password
- Ensure you're using 2.4 GHz WiFi (not 5 GHz)
- Move ESP32 closer to router

### ❌ Issue: Hall or mic readings not changing

**Fix:**

- Check AO connections (GPIO 34 for hall, GPIO 35 for mic)
- Ensure sensors share GND with ESP32
- Verify 3.3V power supply
- Check Serial Monitor for values

### ❌ Issue: LED not lighting

**Fix:**

- Check LED polarity: long leg (+) to resistor
- Verify 220Ω resistor is present
- Test LED with battery to confirm it works
- Check GPIO 12 connection

### ❌ Issue: MQTT not connecting

**Fix:**

- Verify broker address: `broker.hivemq.com`
- Check internet connection
- Ensure port 1883 is not blocked by firewall
- Try different public broker: `test.mosquitto.org`

### ❌ Issue: Upload fails

**Fix:**

- Press and hold BOOT button on ESP32 during upload
- Try lower upload speed: Tools → Upload Speed → 115200
- Check USB cable (must support data, not just power)
- Try different USB port

---

## 🚀 What's Next?

Now that it's working:

1. ✅ **Monitor data** in MQTT Explorer
2. ✅ **Test LED controls** via MQTT
3. ✅ **Change publish interval** dynamically
4. ✅ **View combined JSON** in `esp32/sensors/all` topic

### Want More?

- 📖 Read [README.md](README.md) for detailed feature list
- 🔌 Check [WIRING_GUIDE.md](WIRING_GUIDE.md) for troubleshooting
- ⚙️ See [CONFIGURATION_GUIDE.md](CONFIGURATION_GUIDE.md) for advanced setup
- 🏠 Integrate with Home Assistant or Node-RED
- 📊 Create Grafana dashboards
- 🔒 Setup private MQTT broker

---

## 📱 Test with Your Phone

### Android: IoT MQTT Panel

1. Install from Play Store
2. Add connection: `broker.hivemq.com:1883`
3. Add dashboard widgets for each topic

### iOS: MQTT Explorer or MQTTool

1. Install from App Store
2. Configure broker connection
3. Subscribe to `esp32/sensors/#`
4. Publish to control topics

---

## 💡 Quick Tips

- **LED blinks 3 times on startup** = System ready
- **Check Serial Monitor** for diagnostic info
- **WiFi RSSI shown** in Serial Monitor (-30 to -70 dBm is good)
- **Combined JSON published** every interval to `esp32/sensors/all`
- **Status updates** sent after control commands

---

## 🎓 Understanding the Data Flow

```
┌─────────┐
│ Sensors │ (HW-495, HW-485)
└────┬────┘
     │ Read every 10 seconds
     ▼
┌─────────┐
│  ESP32  │
└────┬────┘
     │ Publish via WiFi
     ▼
┌──────────────┐
│ MQTT Broker  │ (broker.hivemq.com)
└──────┬───────┘
       │ Subscribe
       ▼
┌───────────────┐
│ MQTT Clients  │ (MQTT Explorer, Phone App, etc.)
└───────────────┘
       │
       │ Publish Control Commands
       ▼
   Back to ESP32 → LED Control
```

---

## ✅ Success Checklist

You've successfully completed the quick start if:

- [ ] ESP32 connects to WiFi (sees IP address in Serial Monitor)
- [ ] MQTT connection established (sees "Connected!" message)
- [ ] Sensor data appears every 10 seconds
- [ ] LED blinks on startup
- [ ] Can see data in MQTT Explorer
- [ ] Can control LED via MQTT messages

---

## 🆘 Need Help?

1. **Check Serial Monitor first** - Most issues show error messages there
2. **Test each component separately** - Use test sketches for individual sensors
3. **Verify wiring** - 90% of issues are connection problems
4. **Check power** - Ensure stable 3.3V supply
5. **Review documentation** - README.md has detailed troubleshooting

---

## 🎉 Congratulations!

You now have a fully functional IoT analog sensor station!

**Next Steps:**

- Experiment with different MQTT commands
- Monitor data over time
- Add more sensors
- Create custom dashboards
- Integrate with smart home systems

**Enjoy your IoT journey! 🚀📡🌡️**
