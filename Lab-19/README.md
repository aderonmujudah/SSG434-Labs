# Lab-19: ESP32 WebSocket Real-Time Communication

## 🎯 Objectives

Learn real-time web communication with ESP32 using WebSocket protocol and sensor streaming:

1. ✅ Understand HTTP vs WebSocket protocols
2. ✅ Create WebSocket server with multiple endpoints
3. ✅ Stream sensor data in real-time (DHT11 + BMP180)
4. ✅ Implement bi-directional communication
5. ✅ Control RGB LED remotely via WebSocket
6. ✅ Support multiple simultaneous clients
7. ✅ Build interactive web dashboard with live charts

---

## 📋 Hardware Requirements

### Components

- ESP32 Development Board (1x)
- **DHT11** Temperature & Humidity Sensor (1x)
- **BMP180** Barometric Pressure Sensor (1x)
- **RGB LED** - Common Cathode (1x)
- Resistors: 220Ω (3x) - For RGB LED
- 10kΩ Resistor (1x) - DHT11 pull-up
- Breadboard
- Jumper Wires

### Libraries Required

```cpp
#include <WiFi.h>              // Built-in
#include <WebServer.h>         // Built-in
#include <WebSocketsServer.h>  // arduinoWebSockets by Markus Sattler
#include <DHT.h>               // DHT sensor library by Adafruit
#include <Wire.h>              // Built-in (I2C)
#include <Adafruit_BMP085.h>   // Adafruit BMP085 Library
#include <ArduinoJson.h>       // ArduinoJson by Benoit Blanchon
```

**Install via Arduino Library Manager:**

1. WebSockets by Markus Sattler
2. DHT sensor library by Adafruit
3. Adafruit BMP085 Library (works with BMP180)
4. ArduinoJson (v6.x)

---

## 🔌 Pin Configuration

| Component           | Pin     | Notes                |
| ------------------- | ------- | -------------------- |
| **DHT11 Data**      | GPIO 4  | 10kΩ pull-up to 3.3V |
| **BMP180 SDA**      | GPIO 21 | Default I2C SDA      |
| **BMP180 SCL**      | GPIO 22 | Default I2C SCL      |
| **RGB LED - Red**   | GPIO 25 | 220Ω resistor to LED |
| **RGB LED - Green** | GPIO 26 | 220Ω resistor to LED |
| **RGB LED - Blue**  | GPIO 27 | 220Ω resistor to LED |
| **RGB Common**      | GND     | Common Cathode       |

---

## 🛠️ Wiring Diagram

```
ESP32 Connections:

DHT11 Sensor:
    ESP32          DHT11
    ─────          ─────
    3.3V    ──→    VCC (Pin 1)
    GPIO 4  ──→    DATA (Pin 2)  ──[ 10kΩ ]──→ 3.3V
    GND     ──→    GND (Pin 4)
    (Pin 3 not connected)

BMP180 Sensor (I2C):
    ESP32          BMP180
    ─────          ──────
    3.3V    ──→    VCC
    GND     ──→    GND
    GPIO 21 ──→    SDA
    GPIO 22 ──→    SCL

RGB LED (Common Cathode):
    ESP32          RGB LED
    ─────          ───────
    GPIO 25 ──[ 220Ω ]──→ R (Red Anode)
    GPIO 26 ──[ 220Ω ]──→ G (Green Anode)
    GPIO 27 ──[ 220Ω ]──→ B (Blue Anode)
    GND     ────────────→ Common Cathode (-)

Full Circuit:

         ESP32-WROOM-32
    ┌─────────────────────────┐
    │                         │
    │  3.3V ●─────┬───┬───────┼──→ DHT11 VCC
    │             │   │       │    BMP180 VCC
    │             │   │       │
    │  GND  ●─────┼───┼───┬───┼──→ DHT11 GND
    │             │   │   │   │    BMP180 GND
    │             │   │   │   │    RGB Common (-)
    │             │   │   │   │
    │  GPIO 4  ●──┼───┼───┼───┼──→ DHT11 DATA
    │             │   │   │   │
    │  GPIO 21 ●──┼───┼───┼───┼──→ BMP180 SDA
    │             │   │   │   │
    │  GPIO 22 ●──┼───┼───┼───┼──→ BMP180 SCL
    │             │   │   │   │
    │  GPIO 25 ●──┼───┼───┼───┼──[ 220Ω ]──→ RGB Red
    │             │   │   │   │
    │  GPIO 26 ●──┼───┼───┼───┼──[ 220Ω ]──→ RGB Green
    │             │   │   │   │
    │  GPIO 27 ●──┼───┼───┼───┼──[ 220Ω ]──→ RGB Blue
    │             │   │   │   │
    └─────────────┴───┴───┴───┘
                  │   │   │
                  └───┴───┴──→ Common Ground Bus
```

### Important Notes

**DHT11:**

- Requires 10kΩ pull-up resistor on DATA line
- Power from 3.3V (not 5V for ESP32 compatibility)
- Minimum 2-second interval between readings

**BMP180:**

- I2C address: 0x77 (default)
- Compatible with 3.3V logic
- SDA/SCL need pull-up resistors (usually on breakout board)

**RGB LED:**

- Verify Common Cathode (not Common Anode)
- 220Ω resistors prevent LED burnout
- PWM used for color mixing

---

## 🌐 WiFi Configuration

Edit in firmware:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

**Replace with your actual WiFi credentials!**

---

## 📡 Communication Architecture

### HTTP Server (Port 80)

**Purpose:** Serve web dashboard and provide REST API

**Endpoints:**
| Endpoint | Method | Description | Response |
|----------|--------|-------------|----------|
| `/` | GET | Serve main dashboard | HTML |
| `/sensors` | GET | Current sensor readings | JSON |
| `/status` | GET | System information | JSON |

**Example `/sensors` Response:**

```json
{
  "dht": {
    "temperature": 25.3,
    "humidity": 60.2,
    "valid": true
  },
  "bmp": {
    "temperature": 25.1,
    "pressure": 1013.25,
    "valid": true
  },
  "timestamp": 12345678
}
```

---

### WebSocket Server (Port 81)

**Purpose:** Real-time bi-directional communication

**Endpoint:** `ws://{ESP32_IP}:81/ws`

**Features:**

- ✅ Real-time sensor data streaming (1-second intervals)
- ✅ Remote LED control
- ✅ Multiple simultaneous clients (up to 8)
- ✅ JSON message format
- ✅ Auto-subscribe on connect
- ✅ Keepalive ping/pong

---

## 📨 WebSocket Message Protocol

### Client → Server (Commands)

#### 1. LED Control - Set Color

```json
{
  "type": "led",
  "r": 255,
  "g": 0,
  "b": 0
}
```

**Effect:** Sets RGB LED to specified color (0-255 per channel)

---

#### 2. LED Control - Turn Off

```json
{
  "type": "led",
  "mode": "off"
}
```

**Effect:** Turns off RGB LED (all channels to 0)

---

#### 3. Subscribe to Sensor Stream

```json
{
  "type": "subscribe",
  "stream": "sensors"
}
```

**Effect:** Starts receiving sensor data every 1 second

---

#### 4. Unsubscribe from Sensor Stream

```json
{
  "type": "unsubscribe",
  "stream": "sensors"
}
```

**Effect:** Stops receiving sensor data

---

#### 5. Ping (Keepalive)

```json
{
  "type": "ping"
}
```

**Effect:** Server responds with `pong`

---

#### 6. Get Status

```json
{
  "type": "getStatus"
}
```

**Effect:** Server sends system status

---

### Server → Client (Responses)

#### 1. Welcome Message (on connect)

```json
{
  "type": "welcome",
  "message": "Connected to ESP32 WebSocket Server",
  "clientId": 0,
  "endpoints": "/ws"
}
```

---

#### 2. Sensor Data (streamed every 1s)

```json
{
  "type": "sensors",
  "timestamp": 12345678,
  "dht": {
    "temperature": 25.3,
    "humidity": 60.2,
    "valid": true
  },
  "bmp": {
    "temperature": 25.1,
    "pressure": 1013.25,
    "valid": true
  }
}
```

---

#### 3. LED State Broadcast

```json
{
  "type": "ledState",
  "r": 255,
  "g": 128,
  "b": 0
}
```

**Note:** Sent to ALL clients when any client changes LED

---

#### 4. Acknowledgment

```json
{
  "type": "ack",
  "message": "LED set to RGB(255, 0, 0)"
}
```

---

#### 5. Error

```json
{
  "type": "error",
  "message": "Invalid JSON"
}
```

---

#### 6. Pong Response

```json
{
  "type": "pong",
  "timestamp": 12345678
}
```

---

#### 7. Status Response

```json
{
  "type": "status",
  "uptime": 3600,
  "clients": 3,
  "freeHeap": 245760,
  "wifiRSSI": -45,
  "ipAddress": "192.168.1.100"
}
```

---

## 🚀 Getting Started

### Step 1: Install Libraries

Open Arduino IDE → Tools → Manage Libraries, then install:

1. **WebSockets** by Markus Sattler
2. **DHT sensor library** by Adafruit
3. **Adafruit BMP085 Library**
4. **ArduinoJson** (v6.x)

---

### Step 2: Wire Hardware

Follow wiring diagram above. Critical:

- ✅ DHT11: 10kΩ pull-up on data line
- ✅ BMP180: I2C connections (SDA=21, SCL=22)
- ✅ RGB LED: 220Ω resistors on each channel
- ✅ Common ground for all components

---

### Step 3: Configure WiFi

Edit firmware:

```cpp
const char* ssid = "YourWiFiName";
const char* password = "YourWiFiPassword";
```

---

### Step 4: Upload Firmware

1. Open `ESP32_WebSocket_Server.ino`
2. Select Board: ESP32 Dev Module
3. Select Port
4. Upload ✓

---

### Step 5: Get ESP32 IP Address

Open Serial Monitor (115200 baud):

```
╔════════════════════════════════════════════╗
║   ESP32 WebSocket Real-Time Communication  ║
╚════════════════════════════════════════════╝

✓ WiFi connected!
   IP Address: 192.168.1.100

╔════════════════════════════════════════════╗
║           Access Information               ║
╠════════════════════════════════════════════╣
║ IP Address:  192.168.1.100                 ║
║                                            ║
║ HTTP Endpoints:                            ║
║   Dashboard:  http://192.168.1.100/       ║
║   Sensors:    http://192.168.1.100/sensors║
║   Status:     http://192.168.1.100/status ║
║                                            ║
║ WebSocket:                                 ║
║   Endpoint:   ws://192.168.1.100:81/ws    ║
╚════════════════════════════════════════════╝
```

**Note your ESP32's IP address!**

---

### Step 6: Access Dashboard

**Option A: Built-in Dashboard**

- Open browser: `http://{ESP32_IP}/`
- Basic info page

**Option B: Full Dashboard (Recommended)**

1. Copy `dashboard.html` to your computer
2. Edit line ~250: Replace `{IP}` with your ESP32 IP
   ```javascript
   wsUrl = `ws://192.168.1.100:81/ws`; // Update this!
   ```
3. Open `dashboard.html` in browser

**Dashboard Features:**

- 📊 Live sensor charts (30-second history)
- 🌈 RGB LED color picker
- ⚙️ System information
- 📋 Message log
- 📱 Responsive design (mobile-friendly)

---

## 🎮 Using the Dashboard

### Sensor Monitoring

**Real-Time Data Display:**

- DHT11 Temperature (°C)
- DHT11 Humidity (%)
- BMP180 Temperature (°C)
- BMP180 Pressure (hPa)

**Charts:**

- Last 30 data points
- Updates every 1 second
- Auto-scrolls with new data

**Status Indicators:**

- 🟢 Valid - Sensor reading successfully
- 🔴 Invalid - Sensor disconnected or error

---

### RGB LED Control

**Method 1: Sliders**

1. Adjust R, G, B sliders (0-255)
2. Click "✓ Apply Color"
3. LED changes immediately

**Method 2: Preset Colors**

- Click 🔴 Red, 🟢 Green, or 🔵 Blue buttons
- Instant color change

**Method 3: Turn Off**

- Click "✕ Turn Off" button
- LED turns off

**Live Preview:**

- Color preview box shows selected color
- Updates as you adjust sliders

---

### System Information

**Displayed Data:**

- **Uptime:** Time since ESP32 boot
- **Clients:** Number of connected WebSocket clients
- **Free Heap:** Available RAM (KB/MB)
- **WiFi RSSI:** Signal strength (dBm)
- **IP Address:** ESP32 network address

---

### Message Log

**Features:**

- Shows all WebSocket events
- Timestamps on each message
- Auto-scrolls to latest
- Limited to last 50 messages
- "Clear Log" button

**Message Types:**

- ✓ Success/acknowledgment
- ❌ Errors
- 🔌 Connection events
- 📊 Data updates

---

## 🧪 Testing Procedures

### Test 1: HTTP Endpoints

**Goal:** Verify HTTP server responds correctly

**Steps:**

1. Open browser to `http://{ESP32_IP}/`
   - Should show welcome page ✓
2. Navigate to `http://{ESP32_IP}/sensors`
   - Should show JSON sensor data ✓
3. Navigate to `http://{ESP32_IP}/status`
   - Should show system status JSON ✓

**Expected Results:**

```json
// /sensors
{
  "dht": {"temperature": 25.3, "humidity": 60.2, "valid": true},
  "bmp": {"temperature": 25.1, "pressure": 1013.25, "valid": true},
  "timestamp": 12345678
}

// /status
{
  "uptime": 3600,
  "clients": 1,
  "freeHeap": 245760,
  "wifiRSSI": -45,
  "ipAddress": "192.168.1.100",
  "ssid": "YourWiFiName"
}
```

---

### Test 2: WebSocket Connection

**Goal:** Verify WebSocket connects and exchanges messages

**Steps:**

1. Open `dashboard.html` in browser
2. Check connection status at top
3. Should show "✓ Connected" (green) ✓
4. Message log shows "✓ WebSocket connected!"
5. Sensor data appears within 2 seconds ✓

**Troubleshooting:**

- Red "✗ Disconnected": Check WebSocket URL in code
- Orange "⏳ Connecting...": Check ESP32 is running
- No data: Check sensors are wired correctly

---

### Test 3: Sensor Data Streaming

**Goal:** Verify real-time sensor data updates

**Steps:**

1. Open dashboard
2. Observe sensor values updating
3. Charts should update every 1 second ✓
4. Breathe on DHT11 sensor
   - Temperature should increase ✓
   - Humidity should increase ✓
5. Place hand near BMP180
   - Temperature should increase slightly ✓

**Expected Behavior:**

- Data updates every 1 second
- Charts show 30-second history
- Values are reasonable (temp: 20-30°C, humidity: 30-70%, pressure: 950-1050 hPa)

---

### Test 4: RGB LED Control

**Goal:** Verify remote LED control via WebSocket

**Steps:**

1. Open dashboard
2. Move sliders to R=255, G=0, B=0
3. Click "✓ Apply Color"
   - LED should turn RED ✓
4. Click 🟢 Green preset button
   - LED should turn GREEN immediately ✓
5. Adjust sliders to R=255, G=255, B=0
   - LED should turn YELLOW ✓
6. Click "✕ Turn Off"
   - LED should turn OFF ✓

**Verify:**

- Physical LED changes match dashboard preview
- Message log shows "LED set to RGB(...)"
- Changes are immediate (<100ms)

---

### Test 5: Multiple Clients

**Goal:** Verify multiple browsers can connect simultaneously

**Steps:**

1. Open dashboard in Browser 1
2. Open dashboard in Browser 2 (different tab or device)
3. System info should show "Clients: 2" ✓
4. Change LED color in Browser 1
   - LED changes physically ✓
   - Browser 2 sliders update automatically ✓
5. Close Browser 1
   - System info shows "Clients: 1" ✓
   - Browser 2 continues working ✓

**Expected Results:**

- Up to 8 clients supported
- LED state syncs across all clients
- Sensor data streams to all clients
- Clients can disconnect/reconnect independently

---

### Test 6: Auto-Reconnect

**Goal:** Verify dashboard reconnects after ESP32 reset

**Steps:**

1. Open dashboard (connected)
2. Press ESP32 EN button (reset)
3. Dashboard shows "✗ Disconnected" (red)
4. Wait 3 seconds
5. Dashboard shows "⏳ Connecting..." (orange)
6. After ESP32 boots (~5s total):
   - Shows "✓ Connected" (green) ✓
   - Sensor data resumes streaming ✓

**Expected Behavior:**

- Dashboard automatically attempts reconnect
- No manual refresh needed
- Connection restored within 10 seconds

---

## 🐛 Troubleshooting

### Issue 1: WiFi Connection Failed

**Symptoms:**

```
❌ WiFi connection failed!
   Check SSID and password.
   Restarting...
```

**Solutions:**

1. **Verify credentials:**

   ```cpp
   const char* ssid = "YourWiFiName";      // Check spelling!
   const char* password = "YourPassword";   // Case-sensitive!
   ```

2. **Check WiFi network:**
   - Is router powered on?
   - Is ESP32 in range?
   - Is network 2.4 GHz (not 5 GHz)?

3. **Try different network:**
   - Use phone hotspot for testing
   - Ensure no special characters in password

---

### Issue 2: Sensors Not Detected

**Symptoms:**

- DHT11: All readings show "Invalid"
- BMP180: "⚠️ BMP180 not found! Check wiring."

**DHT11 Solutions:**

1. **Check wiring:**
   - VCC → 3.3V (not 5V!)
   - DATA → GPIO 4
   - GND → GND
   - 10kΩ pull-up from DATA to 3.3V

2. **Verify sensor orientation:**
   - Front (grille) faces you
   - Pin 1 (VCC) on left

3. **Wait 2 seconds:**
   - DHT11 needs warm-up time after power-on

**BMP180 Solutions:**

1. **Check I2C connections:**
   - SDA → GPIO 21
   - SCL → GPIO 22
   - VCC → 3.3V
   - GND → GND

2. **Test I2C address:**
   - Run I2C scanner sketch
   - Should find device at 0x77

3. **Verify breakout board:**
   - Some boards require pull-up resistors (usually included)

---

### Issue 3: WebSocket Won't Connect

**Symptoms:**

- Dashboard shows "✗ Disconnected" permanently
- Browser console shows connection error

**Solutions:**

1. **Check WebSocket URL:**

   ```javascript
   // In dashboard.html line ~250
   wsUrl = `ws://192.168.1.100:81/ws`; // Must match ESP32 IP!
   ```

2. **Verify ESP32 IP:**
   - Check Serial Monitor for actual IP
   - Ping ESP32 from computer: `ping 192.168.1.100`

3. **Check firewall:**
   - Some firewalls block WebSocket port 81
   - Try disabling temporarily

4. **Verify server running:**
   - Serial Monitor should show "✓ WebSocket configured"

5. **Browser compatibility:**
   - Use modern browser (Chrome, Firefox, Edge)
   - Disable browser extensions that block WebSockets

---

### Issue 4: RGB LED Not Working

**Symptoms:**

- LED doesn't light up
- Wrong colors displayed

**Solutions:**

1. **Verify LED type:**
   - Must be Common Cathode (not Common Anode)
   - Test: Connect common pin to GND, any color pin to 3.3V

2. **Check resistors:**
   - 220Ω resistors on EACH color channel
   - Without resistors, LED or ESP32 can be damaged

3. **Verify connections:**
   - GPIO 25 → 220Ω → Red anode
   - GPIO 26 → 220Ω → Green anode
   - GPIO 27 → 220Ω → Blue anode
   - Common cathode → GND

4. **Test individually:**

   ```cpp
   void loop() {
     setRGBColor(255, 0, 0);  // Red
     delay(1000);
     setRGBColor(0, 255, 0);  // Green
     delay(1000);
     setRGBColor(0, 0, 255);  // Blue
     delay(1000);
   }
   ```

5. **PWM conflict:**
   - Ensure no other code uses PWM channels 0, 1, 2

---

### Issue 5: Data Not Updating

**Symptoms:**

- Dashboard shows old data
- Charts don't update

**Solutions:**

1. **Check subscription:**
   - Clients are auto-subscribed on connect
   - Manual: Send `{"type":"subscribe","stream":"sensors"}`

2. **Verify sensor readings:**
   - Serial Monitor should show sensor values every 2 seconds
   - If sensors show "Invalid", see Issue 2

3. **Check network:**
   - Weak WiFi can cause packet loss
   - RSSI < -70 dBm is poor

4. **Browser console:**
   - Press F12 → Console tab
   - Look for JavaScript errors

---

### Issue 6: Multiple Clients Not Syncing

**Symptoms:**

- Client 1 changes LED, Client 2 doesn't update

**Solutions:**

1. **Check broadcast function:**
   - Should see `broadcastLEDState()` called in code
   - Sends to ALL connected clients

2. **Verify both connected:**
   - System info should show "Clients: 2"
   - Serial Monitor shows both connections

3. **Refresh browsers:**
   - Hard refresh: Ctrl+F5 (Windows) or Cmd+Shift+R (Mac)

---

## 📊 Performance Optimization

### Reduce Latency

**1. Increase WiFi signal strength:**

- Move ESP32 closer to router
- Use external antenna (if ESP32 supports)
- Target RSSI > -60 dBm

**2. Optimize message size:**

```cpp
// Bad - Large messages
doc["unnecessaryData"] = "lots of text...";

// Good - Minimal messages
doc["t"] = 25.3;  // Shorter keys
doc["h"] = 60.2;
```

**3. Reduce streaming rate:**

```cpp
// Instead of 1 second
#define SENSOR_STREAM_INTERVAL 2000  // 2 seconds
```

---

### Reduce Memory Usage

**1. Limit clients:**

```cpp
// Reduce from 8 to 4 if memory constrained
ClientInfo clients[4];
```

**2. Smaller chart history:**

```javascript
// In dashboard.html
const maxDataPoints = 20; // Instead of 30
```

**3. Monitor free heap:**

- Should stay > 200KB
- If drops below 100KB, reduce features

---

## 🎓 Advanced Features

### Feature 1: Add More Sensors

**Example: Add DS18B20 Temperature Sensor**

```cpp
#include <OneWire.h>
#include <DallasTemperature.h>

#define DS18B20_PIN 15
OneWire oneWire(DS18B20_PIN);
DallasTemperature ds18b20(&oneWire);

void setup() {
  // ... existing code ...
  ds18b20.begin();
}

void readSensors() {
  // ... existing DHT/BMP reads ...

  ds18b20.requestTemperatures();
  sensorData.ds18b20_temp = ds18b20.getTempCByIndex(0);
}

void sendSensorData(uint8_t num) {
  // ... existing code ...
  doc["ds18b20"]["temperature"] = sensorData.ds18b20_temp;
}
```

---

### Feature 2: Data Logging to SD Card

**Log sensor data:**

```cpp
#include <SD.h>
#include <SPI.h>

void setup() {
  SD.begin();
}

void logData() {
  File file = SD.open("/data.csv", FILE_APPEND);
  file.printf("%lu,%.1f,%.1f,%.1f\n",
    millis(),
    sensorData.dht_temperature,
    sensorData.dht_humidity,
    sensorData.bmp_pressure
  );
  file.close();
}
```

---

### Feature 3: MQTT Integration

**Send data to MQTT broker:**

```cpp
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqtt(espClient);

void setup() {
  mqtt.setServer("broker.hivemq.com", 1883);
  mqtt.connect("ESP32_Lab19");
}

void publishSensors() {
  String json = createSensorJSON();
  mqtt.publish("esp32/sensors", json.c_str());
}
```

---

### Feature 4: Authentication

**Secure WebSocket with token:**

```cpp
void handleClientConnect(uint8_t num) {
  // Send auth challenge
  StaticJsonDocument<100> doc;
  doc["type"] = "authRequired";
  doc["challenge"] = generateToken();

  String json;
  serializeJson(doc, json);
  webSocket.sendTXT(num, json);
}

void handleClientMessage(uint8_t num, char* payload, size_t length) {
  StaticJsonDocument<300> doc;
  deserializeJson(doc, payload);

  if (strcmp(doc["type"], "auth") == 0) {
    if (validateToken(doc["token"])) {
      clients[num].authenticated = true;
    } else {
      webSocket.disconnect(num);
    }
  }
}
```

---

## 📈 Sensor Specifications

### DHT11

| Parameter            | Value                       |
| -------------------- | --------------------------- |
| Temperature Range    | 0-50°C                      |
| Temperature Accuracy | ±2°C                        |
| Humidity Range       | 20-80% RH                   |
| Humidity Accuracy    | ±5% RH                      |
| Sample Rate          | Max 1 Hz (1 reading/second) |
| Power                | 3-5.5V, 2.5mA active        |

**Best Practices:**

- Wait 2 seconds between readings
- Allow 1 second for sensor stabilization after power-on
- Use 10kΩ pull-up resistor on data line

---

### BMP180

| Parameter            | Value                       |
| -------------------- | --------------------------- |
| Pressure Range       | 300-1100 hPa                |
| Pressure Accuracy    | ±1 hPa                      |
| Temperature Range    | -40 to +85°C                |
| Temperature Accuracy | ±2°C                        |
| Altitude Calculation | Yes (derived from pressure) |
| Power                | 3.3V, 5µA standby           |

**Best Practices:**

- Shield from wind/airflow
- Allow 10ms between readings
- Calibrate at sea level for altitude measurements

---

## 🎯 Key Takeaways

1. **WebSocket ≠ HTTP**
   - HTTP: Request-response (client asks, server answers)
   - WebSocket: Bi-directional (both can send anytime)
   - WebSocket ideal for real-time updates

2. **JSON is standard for WebSocket messages**
   - Human-readable
   - Easy to parse
   - Cross-platform compatible

3. **Multiple clients require state management**
   - Track each client individually
   - Broadcast state changes to all
   - Handle connects/disconnects gracefully

4. **Sensor readings have physical limits**
   - DHT11: Max 1 reading/second
   - BMP180: ~10ms read time
   - Stream slower than you read

5. **PWM enables color mixing**
   - RGB LED creates millions of colors
   - 8-bit resolution (0-255 per channel)
   - 5kHz frequency is flicker-free

---

## 📚 Additional Resources

- [WebSocket Protocol RFC 6455](https://tools.ietf.org/html/rfc6455)
- [ArduinoWebSocket Library Docs](https://github.com/Links2004/arduinoWebSockets)
- [DHT11 Datasheet](https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf)
- [BMP180 Datasheet](https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf)
- [Chart.js Documentation](https://www.chartjs.org/docs/latest/)

---

## 📝 Summary

**This Lab Teaches:**

- ✅ WebSocket protocol implementation
- ✅ Real-time bi-directional communication
- ✅ Multi-sensor data acquisition (DHT11 + BMP180)
- ✅ RGB LED PWM control
- ✅ JSON message serialization
- ✅ Interactive web dashboard creation
- ✅ Multiple client handling
- ✅ Live data visualization with charts

**Next Steps:**

- Add more sensors (DS18B20, BME280, etc.)
- Implement data logging to SD card
- Add MQTT for IoT platform integration
- Create mobile app with WebSocket client
- Add authentication/security

**Need Help?**

- See [QUICK_START.md](QUICK_START.md) for rapid setup
- See [WEBSOCKET_CONCEPTS.md](WEBSOCKET_CONCEPTS.md) for protocol deep dive

---

**🎉 Achievement Unlocked: Real-Time IoT Master!** 🌐📊
