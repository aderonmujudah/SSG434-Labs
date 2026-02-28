# Lab-15: ESP32 OTA (Over-The-Air) Firmware Updates

## 📋 Overview

This lab demonstrates how to implement and use **Over-The-Air (OTA)** firmware updates for ESP32, eliminating the need for physical USB connections after initial deployment. Two OTA methods are implemented:

1. **Arduino IDE OTA** - Update directly from Arduino IDE over WiFi
2. **Web-based OTA** - Upload firmware through a beautiful web interface

### Key Features

✅ **Dual OTA Methods**

- Arduino IDE wireless upload
- Web browser firmware upload

✅ **Version Tracking**

- Firmware version display
- Build date and time tracking
- Version comparison between updates

✅ **Update Status Monitoring**

- Real-time progress tracking
- Visual LED feedback during updates
- Web interface progress bar

✅ **Safety Features**

- Password-protected Arduino OTA
- Update validation
- Automatic device reboot after successful update
- Error handling and recovery

✅ **Web Interface**

- Modern, responsive design
- Device information dashboard
- Firmware upload with drag-and-drop
- LED control (ON/OFF/Pattern)
- Real-time status updates

---

## 🔌 Hardware Requirements

| Component               | Quantity | Purpose              |
| ----------------------- | -------- | -------------------- |
| ESP32 Development Board | 1        | Main controller      |
| LED                     | 1        | Status indicator     |
| 220Ω Resistor           | 1        | LED current limiting |
| Breadboard              | 1        | Circuit assembly     |
| Jumper Wires            | Few      | Connections          |

### Pin Configuration

```
ESP32 Pin    →    Component
─────────────────────────────
GPIO 12       →   LED + (via 220Ω resistor)
GND           →   LED -

Built-in LED  →   GPIO 2 (Heartbeat indicator)
```

---

## 📦 Required Libraries

All libraries are built into the ESP32 Arduino core:

- **WiFi.h** - WiFi connectivity
- **ESPmDNS.h** - mDNS hostname resolution
- **WiFiUdp.h** - UDP communication for OTA
- **ArduinoOTA.h** - Arduino IDE OTA functionality
- **WebServer.h** - HTTP web server
- **Update.h** - Firmware update handling

No additional library installation required! ✅

---

## ⚙️ Initial Setup

### Step 1: Hardware Connection

```
ESP32 GPIO 12 → [220Ω Resistor] → LED + (Anode)
                                   LED - (Cathode) → GND
```

### Step 2: Configure WiFi

Open `ESP32_OTA_Firmware.ino` and update:

```cpp
const char* ssid = "YOUR_WIFI_SSID";        // ← Your WiFi name
const char* password = "YOUR_WIFI_PASSWORD"; // ← Your WiFi password
```

### Step 3: Configure OTA Settings (Optional)

```cpp
const char* ota_hostname = "ESP32-OTA";    // ← mDNS hostname
const char* ota_password = "admin123";     // ← OTA password
```

### Step 4: Upload Initial Firmware

1. Connect ESP32 via USB
2. Select **Tools → Board → ESP32 Dev Module**
3. Select correct COM port
4. Click **Upload** (or press Ctrl+U)
5. Wait for upload to complete

### Step 5: Find Your ESP32's IP Address

Open Serial Monitor (115200 baud) after upload:

```
╔═══════════════════════════════════════╗
║   ESP32 OTA Firmware Update System   ║
╚═══════════════════════════════════════╝

Firmware Version: 1.0.0
Build Date: Feb 28 2026 10:30:00

Connecting to WiFi: MyNetwork
.....
✅ WiFi Connected!
   IP Address: 192.168.1.100    ← Note this IP!
   MAC Address: 24:0A:C4:XX:XX:XX
   RSSI: -45 dBm

✅ System Ready!
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
📡 Web Interface: http://192.168.1.100
🔧 Arduino OTA: ESP32-OTA
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

---

## 🌐 Method 1: Web-Based OTA Update

### Access Web Interface

1. Open browser and go to: `http://192.168.1.100` (use your ESP32's IP)
2. You'll see a beautiful dashboard showing:
   - Current firmware version
   - Build date and time
   - Device hostname and IP address
   - MAC address
   - System uptime
   - Free memory

### Prepare Firmware Binary

Before uploading, you need to export the compiled binary:

#### Method A: Export from Arduino IDE

1. Open `ESP32_OTA_Firmware_v2.ino` (the test firmware)
2. Update WiFi credentials to match your network
3. Go to **Sketch → Export Compiled Binary** (or Ctrl+Alt+S)
4. Wait for compilation to complete
5. Binary will be saved in the sketch folder as:
   ```
   ESP32_OTA_Firmware_v2.ino.esp32.bin
   ```

#### Method B: Compile and Locate

1. Click **Verify** (✓) in Arduino IDE
2. Note the temporary build path in the output (bottom of IDE)
3. Navigate to that folder
4. Look for `.bin` file

### Upload New Firmware

1. In the web interface, click **Choose File**
2. Select the `.bin` file (e.g., `ESP32_OTA_Firmware_v2.ino.esp32.bin`)
3. Click **Upload & Update Firmware**
4. Watch the progress bar (0% → 100%)
5. Wait for "Update successful! Device will reboot..."
6. ESP32 will automatically restart
7. Page will reload after reboot
8. **Version should now show 2.0.0** 🎉

### Update Process Flow

```
┌─────────────────┐
│ Select .bin file│
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Click Upload    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Upload Progress │ ← Progress bar updates
│   0% → 100%     │ ← LED blinks rapidly
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Verify & Flash  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Auto Reboot     │ ← Wait 3 seconds
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ New Version!    │ ✅ v2.0.0 running
└─────────────────┘
```

---

## 🔧 Method 2: Arduino IDE OTA Update

After the initial USB upload, you can update wirelessly from Arduino IDE.

### Prerequisites

- ESP32 must be on the same network as your computer
- First firmware already uploaded via USB
- OTA password configured (default: `admin123`)

### Steps to Update via Arduino IDE

1. **Open the new firmware** (e.g., `ESP32_OTA_Firmware_v2.ino`)

2. **Update WiFi credentials** (must match current network)

3. **Select Network Port:**
   - Go to **Tools → Port**
   - You should see a network port option:
     ```
     ESP32-OTA at 192.168.1.100
     ```
   - Select this network port

4. **Upload wirelessly:**
   - Click **Upload** button (→)
   - Enter OTA password when prompted: `admin123`
   - Watch progress in Arduino IDE output
   - LED will blink rapidly during update

5. **Monitor Serial output:**
   - After upload, connect via network port for Serial Monitor
   - Or reconnect USB cable if needed

### Troubleshooting Arduino OTA

**Problem:** Network port not appearing

**Solutions:**

- Wait 10-15 seconds after ESP32 boots
- Restart Arduino IDE
- Check ESP32 and computer are on same network
- Verify mDNS is working: `ping ESP32-OTA.local` (Linux/Mac)
- Check firewall isn't blocking port 3232

**Problem:** Upload fails with authentication error

**Solutions:**

- Verify OTA password: `admin123`
- Check password is correctly set in firmware
- Try restarting ESP32

---

## 📊 Version Tracking

### Version Numbering

The firmware uses semantic versioning:

```
Version: MAJOR.MINOR.PATCH
Example: 2.0.0

MAJOR: Incompatible API changes
MINOR: Add functionality (backwards-compatible)
PATCH: Bug fixes (backwards-compatible)
```

### Checking Current Version

#### Via Web Interface

- Open `http://[ESP32_IP]`
- Version displayed prominently at top

#### Via Serial Monitor

- Open Serial Monitor (115200 baud)
- Version shown at startup:
  ```
  Firmware Version: 2.0.0
  Build Date: Feb 28 2026
  Build Time: 10:30:00
  ```

#### Via API

- GET request to: `http://[ESP32_IP]/status`
- Returns JSON:
  ```json
  {
    "version": "2.0.0",
    "uptime": "5m 23s",
    "freeHeap": 234567,
    "updateStatus": "Ready",
    "updateProgress": 0
  }
  ```

### Updating Version Number

In your firmware sketch, modify:

```cpp
#define FIRMWARE_VERSION "3.0.0"  // ← Change version here
```

Always increment version when making changes!

---

## 💡 LED Status Indicators

### LED Behaviors

| LED Pattern             | Meaning                     |
| ----------------------- | --------------------------- |
| **Slow blink (1s)**     | Normal operation (v1.0.0)   |
| **Fast blink (500ms)**  | Normal operation (v2.0.0)   |
| **Rapid blink (100ms)** | OTA update in progress      |
| **3 quick blinks**      | WiFi connected successfully |
| **5 quick blinks**      | OTA update successful       |
| **Rainbow pattern**     | Startup sequence (v2.0.0)   |

### Built-in LED (GPIO 2)

Heartbeat indicator - blinks at current firmware's configured rate:

- v1.0.0: 1-second interval
- v2.0.0: 500ms interval (faster!)

---

## 🎨 Web Interface Features

### Dashboard Information

- **Firmware Version** - Current running version
- **Build Date/Time** - When firmware was compiled
- **Hostname** - mDNS hostname (ESP32-OTA)
- **IP Address** - Current local IP
- **MAC Address** - Device MAC address
- **Uptime** - Time since last reboot
- **Free Heap** - Available memory (KB)

### LED Control

**v1.0.0 Features:**

- Turn LED ON
- Turn LED OFF

**v2.0.0 Features (NEW!):**

- Turn LED ON
- Turn LED OFF
- **Blink Pattern** - Creates a 5-blink sequence

### Firmware Upload

- Drag-and-drop support
- File type validation (.bin files only)
- Real-time upload progress
- Status messages with icons
- Automatic page reload after update

---

## 🔄 Testing OTA Updates

### Complete Test Procedure

#### 1. Upload Initial Firmware (v1.0.0)

```bash
# Via USB
Arduino IDE → Upload → ESP32_OTA_Firmware.ino
```

#### 2. Verify v1.0.0 Running

- Open web interface
- Check version shows: **v1.0.0**
- Observe LED blink rate: **1 second intervals**
- Test LED controls (ON/OFF only)

#### 3. Prepare v2.0.0 Binary

```bash
Arduino IDE → Open ESP32_OTA_Firmware_v2.ino
Sketch → Export Compiled Binary
```

#### 4. Update via Web OTA

- Web interface → Upload .bin file
- Watch progress bar
- Wait for automatic reboot

#### 5. Verify v2.0.0 Running

- Page reloads automatically
- Check version shows: **v2.0.0** (green banner!)
- Observe LED blink rate: **500ms intervals (faster!)**
- Test new LED control: **Blink Pattern** button
- Notice rainbow startup sequence in Serial Monitor

### Expected Differences v1.0.0 vs v2.0.0

| Feature          | v1.0.0          | v2.0.0               |
| ---------------- | --------------- | -------------------- |
| Version Banner   | Purple gradient | Green gradient ✨    |
| LED Blink Rate   | 1000ms          | 500ms (faster!)      |
| LED Controls     | ON/OFF          | ON/OFF/**Blink**     |
| Startup Sequence | 3 quick blinks  | Rainbow pattern 🌈   |
| UI Theme         | Standard        | Enhanced with badges |
| Serial Messages  | Basic           | Emoji-enhanced 🎉    |

---

## 🛡️ Security Considerations

### Current Implementation

✅ **Password-protected Arduino OTA**

```cpp
const char* ota_password = "admin123";
```

❌ **Web OTA has NO authentication** (for learning purposes)

### Production Recommendations

1. **Add Web Authentication:**

```cpp
server.on("/update", HTTP_POST, []() {
    if(!server.authenticate("admin", "password")) {
        return server.requestAuthentication();
    }
    // ... rest of update code
});
```

2. **Use HTTPS (TLS/SSL):**

- Requires certificates
- Prevents man-in-the-middle attacks
- Encrypts firmware during transmission

3. **Implement Firmware Signing:**

- Verify firmware authenticity
- Prevents unauthorized firmware
- Use cryptographic signatures

4. **Change Default Passwords:**

```cpp
const char* ota_password = "YOUR_STRONG_PASSWORD";
```

5. **Restrict to Local Network:**

- Don't expose to internet without VPN
- Use firewall rules
- Consider MAC address filtering

---

## ⚠️ Common Issues & Solutions

### Issue 1: Web Interface Not Accessible

**Symptoms:** Can't open `http://192.168.1.100`

**Solutions:**

- ✅ Verify ESP32 is connected (check Serial Monitor)
- ✅ Confirm IP address is correct
- ✅ Ensure computer and ESP32 on same network
- ✅ Try pinging the IP: `ping 192.168.1.100`
- ✅ Check firewall isn't blocking connections
- ✅ Try accessing via hostname: `http://esp32-ota.local`

### Issue 2: OTA Update Fails

**Symptoms:** Upload starts but fails partway

**Solutions:**

- ✅ Check WiFi signal strength (RSSI > -70 dBm)
- ✅ Ensure stable power supply
- ✅ Verify .bin file is valid (recompile if needed)
- ✅ Check sufficient free memory
- ✅ Try uploading smaller firmware
- ✅ Disable other devices on network temporarily

### Issue 3: ESP32 Won't Reboot After Update

**Symptoms:** Update completes but device doesn't restart

**Solutions:**

- ✅ Wait 10-15 seconds
- ✅ Manually press reset button
- ✅ Power cycle the ESP32
- ✅ Check Serial Monitor for error messages

### Issue 4: Version Still Shows Old Number

**Symptoms:** Updated but version unchanged

**Solutions:**

- ✅ Verify you exported the correct sketch
- ✅ Check FIRMWARE_VERSION was actually changed
- ✅ Hard refresh browser (Ctrl+F5)
- ✅ Clear browser cache
- ✅ Try different browser

### Issue 5: Arduino IDE OTA Port Not Found

**Symptoms:** Network port doesn't appear

**Solutions:**

- ✅ Wait 20-30 seconds after ESP32 boots
- ✅ Restart Arduino IDE
- ✅ Verify mDNS works: `ping ESP32-OTA.local`
- ✅ Check both devices on same subnet
- ✅ Windows: Install Bonjour service
- ✅ Linux: Install avahi-daemon

### Issue 6: Upload Progress Stuck

**Symptoms:** Progress bar stops at X%

**Solutions:**

- ✅ Don't close browser during upload
- ✅ Check WiFi didn't disconnect
- ✅ Verify ESP32 still powered
- ✅ Wait 2-3 minutes (might be processing)
- ✅ If stuck >5 minutes, power cycle ESP32

---

## 📈 Advanced Features

### Custom Update Validation

Add version checking before updating:

```cpp
bool canUpdate(String newVersion) {
    // Parse versions and compare
    // Return true only if newVersion > currentVersion
    return true;  // Simplified
}
```

### Rollback on Failure

ESP32 supports partition rollback if update fails:

```cpp
#include "esp_ota_ops.h"

void setup() {
    esp_ota_mark_app_valid_cancel_rollback();
}
```

### Scheduled Updates

Check for updates periodically:

```cpp
void loop() {
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 3600000) {  // Check every hour
        checkForUpdates();
        lastCheck = millis();
    }
}
```

### Cloud-Based Updates

Integrate with cloud services:

- AWS IoT Device Management
- Azure IoT Hub
- Google Cloud IoT
- Custom update server

---

## 🎓 Learning Objectives

By completing this lab, you will understand:

✅ **OTA Update Methods**

- Arduino IDE OTA protocol
- HTTP-based firmware upload
- mDNS hostname resolution

✅ **Firmware Management**

- Version tracking and display
- Build date/time embedding
- Binary file generation

✅ **Web Server Development**

- HTTP server on ESP32
- File upload handling
- RESTful API endpoints
- Responsive HTML/CSS/JavaScript

✅ **Update Process**

- Flash memory partitioning
- Firmware validation
- Progress monitoring
- Error handling

✅ **Production Considerations**

- Security implications
- Rollback strategies
- User experience design
- Status feedback mechanisms

---

## 🚀 Extension Ideas

1. **HTTPS Support** - Add SSL/TLS encryption
2. **Update Server** - Create centralized firmware repository
3. **Automatic Updates** - Check and download new versions automatically
4. **A/B Partitioning** - Keep backup firmware for rollback
5. **Differential Updates** - Only send changed bytes
6. **Multi-Device Management** - Update multiple ESP32s simultaneously
7. **Update Scheduling** - Schedule updates for off-peak hours
8. **Changelog Display** - Show what's new in each version
9. **User Approval** - Require confirmation before applying updates
10. **Statistics Tracking** - Log update success/failure rates

---

## 📚 References

- [ESP32 Arduino OTA Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/ota.html)
- [ArduinoOTA Library](https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA)
- [ESP32 Update Library](https://github.com/espressif/arduino-esp32/tree/master/libraries/Update)
- [ESP-IDF OTA Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/ota.html)
- [Semantic Versioning](https://semver.org/)

---

## 📄 Files in This Lab

| File                        | Description                           |
| --------------------------- | ------------------------------------- |
| `ESP32_OTA_Firmware.ino`    | Initial firmware (v1.0.0)             |
| `ESP32_OTA_Firmware_v2.ino` | Updated firmware (v2.0.0) for testing |
| `README.md`                 | This comprehensive guide              |
| `QUICK_START.md`            | Fast setup guide                      |
| `WEB_OTA_GUIDE.md`          | Detailed web OTA instructions         |

---

## ✅ Success Criteria

You've successfully completed Lab-15 when:

- [ ] Initial firmware uploaded via USB
- [ ] Web interface accessible at ESP32's IP
- [ ] Version 1.0.0 confirmed running
- [ ] Firmware v2.0.0 binary exported
- [ ] Web OTA update performed successfully
- [ ] ESP32 automatically rebooted
- [ ] Version 2.0.0 confirmed running
- [ ] New features (faster blink, pattern) working
- [ ] Arduino IDE OTA tested (optional)
- [ ] Understanding of both OTA methods

---

**Congratulations! You've mastered ESP32 OTA updates! 🎉🚀**

Now you can update your ESP32 projects wirelessly from anywhere on your network!
