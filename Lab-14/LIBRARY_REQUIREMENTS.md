# Required Libraries - ESP32 MQTT Weather Station

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

### 2. Adafruit BME280 Library

- **Author:** Adafruit
- **Version:** 2.2.0 or later
- **Purpose:** BME280 sensor communication
- **Search:** `Adafruit BME280`
- **Dependencies:** Will auto-install Adafruit Unified Sensor
- **GitHub:** https://github.com/adafruit/Adafruit_BME280_Library

### 3. DHT sensor library

- **Author:** Adafruit
- **Version:** 1.4.0 or later
- **Purpose:** DHT11/DHT22 sensor reading
- **Search:** `DHT sensor library`
- **Dependencies:** Adafruit Unified Sensor
- **GitHub:** https://github.com/adafruit/DHT-sensor-library

### 4. ArduinoJson

- **Author:** Benoit Blanchon
- **Version:** 6.x (NOT 7.x - use version 6.21.0 or similar)
- **Purpose:** JSON encoding/decoding
- **Search:** `ArduinoJson`
- **⚠️ Important:** Install version 6.x, not 7.x (API changes)
- **GitHub:** https://github.com/bblanchon/ArduinoJson

---

## Automatically Included Dependencies

These are usually installed automatically with the main libraries:

### Adafruit Unified Sensor

- **Version:** 1.1.0 or later
- **Installed with:** BME280 and DHT libraries
- **Purpose:** Common sensor interface

### Wire (Built-in)

- **Purpose:** I2C communication
- **Included with:** ESP32 Arduino core

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
2. Adafruit BME280
3. DHT sensor library
4. ArduinoJson
```

Click "Install" for each, and approve dependencies when prompted.

---

## Verification

After installation, verify all libraries are present:
**Sketch → Include Library** - You should see:

- ✅ Adafruit BME280 Library
- ✅ Adafruit Unified Sensor
- ✅ DHT sensor library
- ✅ ArduinoJson
- ✅ PubSubClient

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
    adafruit/Adafruit BME280 Library@^2.2.0
    adafruit/DHT sensor library@^1.4.0
    bblanchon/ArduinoJson@^6.21.0
    adafruit/Adafruit Unified Sensor@^1.1.0
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

### Issue: BME280 example doesn't work

**Solution:**

- Ensure Adafruit Unified Sensor is installed
- Check in Library Manager dependencies

---

## Library Compatibility Matrix

| Library            | Min Version | Max Version | Notes                   |
| ------------------ | ----------- | ----------- | ----------------------- |
| PubSubClient       | 2.8.0       | Latest      | Stable                  |
| Adafruit BME280    | 2.2.0       | Latest      | Requires Unified Sensor |
| DHT sensor library | 1.4.0       | Latest      | Requires Unified Sensor |
| ArduinoJson        | 6.19.0      | 6.x.x       | ⚠️ NOT 7.x              |
| ESP32 Arduino Core | 2.0.0       | Latest      | Use stable release      |

---

## Installation Complete! ✅

Once all libraries are installed, you're ready to:

1. Open the sketch
2. Configure WiFi and MQTT settings
3. Upload to ESP32
4. Start monitoring data!
