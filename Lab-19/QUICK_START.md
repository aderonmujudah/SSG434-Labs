# Lab-19 Quick Start Guide

## ⚡ 15-Minute Setup

Get ESP32 WebSocket server with sensor streaming running in 15 minutes!

---

## 🎯 What You'll Build

- Real-time sensor dashboard
- DHT11 (temperature & humidity) streaming
- BMP180 (pressure & temperature) streaming
- Remote RGB LED control
- Live charts updating every second

---

## 📦 What You Need

### Hardware (5 components)

- ESP32 board
- DHT11 sensor
- BMP180 sensor (I2C)
- RGB LED (common cathode)
- 3x 220Ω + 1x 10kΩ resistors

### Software

- Arduino IDE with libraries:
  - WebSockets (Markus Sattler)
  - DHT sensor library (Adafruit)
  - Adafruit BMP085
  - ArduinoJson

---

## 🔧 Step 1: Install Libraries (3 minutes)

**Arduino IDE → Tools → Manage Libraries:**

1. Search "**WebSockets**" → Install by Markus Sattler
2. Search "**DHT sensor**" → Install by Adafruit
3. Search "**Adafruit BMP085**" → Install
4. Search "**ArduinoJson**" → Install (v6.x)

✓ All installed? Continue!

---

## 🔌 Step 2: Wiring (5 minutes)

### Quick Wiring Guide

```
DHT11 (3 pins):
  VCC  → ESP32 3.3V
  DATA → ESP32 GPIO 4 + 10kΩ resistor to 3.3V
  GND  → ESP32 GND

BMP180 (4 pins):
  VCC → ESP32 3.3V
  GND → ESP32 GND
  SDA → ESP32 GPIO 21
  SCL → ESP32 GPIO 22

RGB LED (4 pins):
  Red   → 220Ω → ESP32 GPIO 25
  Green → 220Ω → ESP32 GPIO 26
  Blue  → 220Ω → ESP32 GPIO 27
  Common(-) → ESP32 GND
```

### Visual Reference

```
ESP32 Pinout:

    3.3V ●──┬──→ DHT11 VCC
            ├──→ BMP180 VCC
            └──→ 10kΩ → GPIO 4

    GPIO 4 ●────→ DHT11 DATA

    GPIO 21 ●───→ BMP180 SDA
    GPIO 22 ●───→ BMP180 SCL

    GPIO 25 ●─[ 220Ω ]──→ RGB Red
    GPIO 26 ●─[ 220Ω ]──→ RGB Green
    GPIO 27 ●─[ 220Ω ]──→ RGB Blue

    GND ●───┬──→ DHT11 GND
            ├──→ BMP180 GND
            └──→ RGB Common (-)
```

**Critical:**

- ⚠️ 10kΩ pull-up on DHT11 data line
- ⚠️ 220Ω resistors on EACH RGB pin
- ⚠️ RGB LED must be Common Cathode

---

## 📝 Step 3: Configure WiFi (1 minute)

**Edit firmware line 18-19:**

```cpp
const char* ssid = "YourWiFiName";      // ← Change this
const char* password = "YourPassword";   //← Change this
```

**Save file!**

---

## 📤 Step 4: Upload Firmware (2 minutes)

1. Open `ESP32_WebSocket_Server.ino`
2. Select **Board:** ESP32 Dev Module
3. Select **Port:** Your ESP32's COM port
4. Click **Upload** ⬆️
5. Wait for "Done uploading"

---

## 🖥️ Step 5: Get IP Address (1 minute)

1. Open **Serial Monitor** (115200 baud)
2. Press ESP32 **EN button** (reset)
3. Find IP address in output:

```
✓ WiFi connected!
   IP Address: 192.168.1.100    ← YOUR IP HERE

╔════════════════════════════════════════════╗
║           Access Information               ║
║ IP Address:  192.168.1.100                 ║
╚════════════════════════════════════════════╝
```

**Write down your IP address!** (Example: 192.168.1.100)

---

## 🌐 Step 6: Open Dashboard (3 minutes)

### Method 1: Quick Test

Open browser → `http://192.168.1.100/` (use your IP)

Should see simple welcome page ✓

---

### Method 2: Full Dashboard (Recommended)

1. **Edit `dashboard.html`** (line ~250):

   ```javascript
   // OLD:
   const host = window.location.hostname;

   // NEW (replace with your ESP32 IP):
   const host = "192.168.1.100";
   ```

2. **Save file**

3. **Open `dashboard.html` in browser**
   - Double-click file, or
   - File → Open in browser

4. **Should see:**
   - ✓ Connected (green status)
   - Sensor values updating
   - Live charts
   - RGB LED control

---

## 🎮 Quick Tests

### Test 1: Sensor Data (30 seconds)

**Check dashboard:**

- DHT11 Temperature: 20-30°C ✓
- DHT11 Humidity: 30-70% ✓
- BMP180 Temperature: 20-30°C ✓
- BMP180 Pressure: 950-1050 hPa ✓
- Status: "Valid" (green) ✓

**All green?** Sensors working! 🎉

**Any red "Invalid"?** Check wiring:

- DHT11: Verify 10kΩ pull-up resistor
- BMP180: Check I2C connections (SDA=21, SCL=22)

---

### Test 2: Live Updates (30 seconds)

**Watch charts:**

- Should update every 1 second ✓
- Line moves from right to left ✓
- Shows last 30 seconds of data ✓

**Test interaction:**

- Breathe on DHT11 → Temperature & humidity increase ✓
- Place hand near BMP180 → Temperature increases ✓

---

### Test 3: RGB LED Control (1 minute)

**Try preset colors:**

1. Click **🔴 Red** button
   - LED turns RED ✓
   - Preview box turns red ✓

2. Click **🟢 Green** button
   - LED turns GREEN ✓

3. Click **🔵 Blue** button
   - LED turns BLUE ✓

4. Click **✕ Turn Off**
   - LED turns OFF ✓

**Custom color:**

1. Drag **Red slider** to 255
2. Drag **Green slider** to 255
3. Drag **Blue slider** to 0
4. Click **✓ Apply Color**
   - LED turns YELLOW ✓

---

### Test 4: Multiple Clients (1 minute)

1. Open dashboard in **second browser tab**
2. Change LED color in tab 1
   - Tab 2 sliders update automatically ✓
   - Physical LED changes ✓
3. System Info shows "Clients: 2" ✓

All working? **You're done!** 🎉

---

## 🐛 Quick Troubleshooting

### Problem: WiFi Won't Connect

**Serial Monitor shows:**

```
❌ WiFi connection failed!
```

**Fix:**

1. Check SSID/password spelling
2. Ensure 2.4 GHz WiFi (not 5 GHz)
3. Move ESP32 closer to router
4. Try phone hotspot

---

### Problem: Dashboard Shows "Disconnected"

**Fix:**

1. Check ESP32 IP is correct in dashboard.html
2. Verify ESP32 is powered and running
3. Ping ESP32: `ping 192.168.1.100`
4. Try different browser (Chrome/Firefox)

---

### Problem: Sensors Show "Invalid"

**DHT11 Invalid:**

- ✓ Check 10kΩ pull-up resistor (DATA to 3.3V)
- ✓ Verify GPIO 4 connection
- ✓ Wait 2 seconds after power-on

**BMP180 Invalid:**

- ✓ Check I2C connections (SDA=21, SCL=22)
- ✓ Verify 3.3V power
- ✓ Try different BMP180 module

---

### Problem: RGB LED Doesn't Work

**Fix:**

1. **Verify LED type:** Common Cathode (not Anode)
2. **Test:** Common pin → GND, any color pin → 3.3V (should light)
3. **Check resistors:** 220Ω on EACH color channel
4. **Verify pins:** R=25, G=26, B=27

**Quick LED test:**

```cpp
// Add to loop() temporarily:
setRGBColor(255, 0, 0);  // Red
delay(1000);
setRGBColor(0, 255, 0);  // Green
delay(1000);
setRGBColor(0, 0, 255);  // Blue
delay(1000);
```

---

## 📊 Expected Results

### Good Sensor Readings

| Sensor | Parameter   | Typical Range |
| ------ | ----------- | ------------- |
| DHT11  | Temperature | 20-30°C       |
| DHT11  | Humidity    | 30-70%        |
| BMP180 | Temperature | 20-30°C       |
| BMP180 | Pressure    | 950-1050 hPa  |

**Outside range?** Check environment or sensor calibration.

---

### System Performance

| Metric       | Expected         |
| ------------ | ---------------- |
| Update Rate  | 1 second         |
| WiFi RSSI    | > -70 dBm (good) |
| Free Heap    | > 200 KB         |
| Max Clients  | 8 simultaneous   |
| LED Response | < 100ms          |

---

## 🎯 Success Checklist

Complete setup verification:

```
□ All 4 libraries installed
□ Hardware wired (DHT11, BMP180, RGB LED)
□ WiFi credentials configured
□ Firmware uploaded successfully
□ ESP32 IP address obtained
□ Dashboard opens in browser
□ Connection status shows "✓ Connected"
□ All sensors show "Valid" (green)
□ Charts update every 1 second
□ RGB LED responds to controls
□ Multiple browser tabs work
□ LED state syncs across clients
```

**All checked?** Perfect! 🎉

---

## 💡 Quick Tips

### Tip 1: RSSI Indicator

WiFi signal strength:

- **> -50 dBm:** Excellent 📶📶📶📶
- **-50 to -60 dBm:** Good 📶📶📶
- **-60 to -70 dBm:** Fair 📶📶
- **< -70 dBm:** Weak 📶 (move closer!)

---

### Tip 2: Color Presets

**Common colors:**

- Red: (255, 0, 0)
- Green: (0, 255, 0)
- Blue: (0, 0, 255)
- Yellow: (255, 255, 0)
- Cyan: (0, 255, 255)
- Magenta: (255, 0, 255)
- White: (255, 255, 255)
- Orange: (255, 128, 0)
- Purple: (128, 0, 255)

---

### Tip 3: Sensor Placement

**DHT11:**

- ✓ Keep away from heat sources
- ✓ Allow air circulation
- ✓ Don't touch sensor directly
- ✓ Shield from direct sunlight

**BMP180:**

- ✓ Shield from wind/airflow
- ✓ Keep level for altitude readings
- ✓ Avoid vibration

---

### Tip 4: Browser Console

**Debug WebSocket issues:**

1. Press **F12** (open DevTools)
2. Click **Console** tab
3. Look for errors:
   - ✓ Should see: "WebSocket connected"
   - ❌ Errors? Check IP address

---

### Tip 5: Mobile Access

**Access from phone:**

1. Connect phone to **same WiFi** as ESP32
2. Open browser on phone
3. Navigate to `http://192.168.1.100/` (your IP)
4. Dashboard works on mobile! 📱

**Tip:** Bookmark for quick access

---

## 🚀 Next Experiments

### Easy Modifications

**1. Change update rate:**

```cpp
// In firmware, line ~56:
#define SENSOR_STREAM_INTERVAL 500  // Faster (0.5s)
// or
#define SENSOR_STREAM_INTERVAL 5000 // Slower (5s)
```

**2. Add temperature alert:**

```cpp
void checkAlerts() {
  if (sensorData.dht_temperature > 30.0) {
    setRGBColor(255, 0, 0);  // Red warning!
  } else {
    setRGBColor(0, 255, 0);  // Green OK
  }
}
```

**3. Change chart colors:**

```javascript
// In dashboard.html, line ~650:
borderColor: '#FF0000',  // Change to your color
```

---

## 📚 Learn More

**For deeper understanding:**

- [README.md](README.md) - Complete documentation
- [WEBSOCKET_CONCEPTS.md](WEBSOCKET_CONCEPTS.md) - Protocol theory
- [Chart.js Docs](https://www.chartjs.org/) - Customize charts

---

## ✅ Quick Reference

### Serial Commands

Monitor via Serial Monitor (115200 baud):

- See sensor readings every 2 seconds
- View WebSocket connections/disconnects
- Check system status

### HTTP Endpoints

- `http://{IP}/` - Dashboard
- `http://{IP}/sensors` - JSON sensor data
- `http://{IP}/status` - System info

### WebSocket Endpoint

- `ws://{IP}:81/ws` - WebSocket connection

---

**Total Time:** ~15 minutes  
**Difficulty:** ⭐⭐⭐☆☆ (Intermediate)  
**Achievement:** Real-time IoT dashboard! 🌐📊

---

**🎉 Congratulations! You've built a real-time IoT sensor dashboard!**

Open dashboard from any device on your network and watch live data streaming! 🚀
