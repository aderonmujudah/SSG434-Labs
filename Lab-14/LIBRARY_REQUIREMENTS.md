# Required Libraries - ESP32 MQTT Analog Sensor Station

## 📦 Installation Instructions

Install these libraries using Arduino IDE Library Manager:
**Sketch → Include Library → Manage Libraries**

---

## Core Libraries

### 1. PubSubClient

- **Author:** Nick O'Leary
- **Version:** 2.8.0 or later
- **Purpose:** MQTT client for ESP32
- **Search:** `PubSubClient`
- **GitHub:** https://github.com/knolleary/pubsubclient

### 2. ArduinoJson

- **Author:** Benoit Blanchon
- **Version:** 6.x (NOT 7.x - use version 6.21.0 or similar)
- **Purpose:** JSON encoding/decoding
- **Search:** `ArduinoJson`
- **⚠️ Important:** Install version 6.x, not 7.x (API changes)
- **GitHub:** https://github.com/bblanchon/ArduinoJson

---

## Automatically Included Dependencies

These are usually installed automatically with the main libraries:

### WiFi (Built-in)

- **Purpose:** WiFi connectivity
- **Included with:** ESP32 Arduino core

---

## ESP32 Board Support

### ESP32 Arduino Core

- **Version:** 2.0.0 or later
- **Installation:**
  1. Go to **File → Preferences**
  2. Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
  3. Go to **Tools → Board → Boards Manager**
  4. Search "esp32"
  5. Install "esp32 by Espressif Systems"

---

## Quick Installation Script

Copy and paste this into Arduino IDE Library Manager search box, one at a time:

```
1. PubSubClient
2. ArduinoJson
```

Click "Install" for each, and approve dependencies when prompted.

---

## Verification

After installation, verify all libraries are present:
**Sketch → Include Library** - You should see:

- ✅ PubSubClient
- ✅ ArduinoJson
- ✅ WiFi (built-in)

---

## platformio.ini (Alternative: PlatformIO Users)

If using PlatformIO instead of Arduino IDE:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

lib_deps =
    knolleary/PubSubClient@^2.8
    bblanchon/ArduinoJson@^6.21.0
```

---

## Manual Installation (Advanced)

If Library Manager doesn't work, manual installation:

1. Download library ZIP from GitHub
2. **Sketch → Include Library → Add .ZIP Library**
3. Select downloaded ZIP file
4. Restart Arduino IDE

---

## Troubleshooting Library Issues

### Issue: Library not found during compilation

**Solution:**

- Close and restart Arduino IDE
- Verify library in: `Documents/Arduino/libraries/`
- Reinstall library if necessary

### Issue: Multiple library versions

**Solution:**

- Delete older versions from `Documents/Arduino/libraries/`
- Keep only latest compatible version

### Issue: Compilation errors with ArduinoJson

**Solution:**

- Ensure using version 6.x (not 7.x)
- Code uses V6 API syntax

## Library Compatibility Matrix

| Library            | Min Version | Max Version | Notes              |
| ------------------ | ----------- | ----------- | ------------------ |
| PubSubClient       | 2.8.0       | Latest      | Stable             |
| ArduinoJson        | 6.19.0      | 6.x.x       | ⚠️ NOT 7.x         |
| ESP32 Arduino Core | 2.0.0       | Latest      | Use stable release |

---

## Installation Complete! ✅

Once all libraries are installed, you're ready to:

1. Open the sketch
2. Configure WiFi and MQTT settings
3. Upload to ESP32
4. Start monitoring data!
