# Project Overview

## ESP32 Bluetooth Classic LED & Sensor Controller

A complete IoT project using ESP32 with Bluetooth Classic to control LEDs and monitor environmental data from a DHT11 sensor via mobile app.

---

## Project Files

### Main Code Files

#### `esp32_bluetooth_control.ino`

**Purpose:** Main ESP32 Arduino sketch  
**Description:** Complete implementation with:

- Bluetooth Classic (BR/EDR) communication
- Command parser for text-based commands
- 2 LED control (on/off)
- DHT11 temperature & humidity sensor reading
- Auto-send feature for continuous monitoring
- Status reporting

**Upload this file to your ESP32 to run the project.**

---

#### `component_tests.ino`

**Purpose:** Component testing sketches  
**Description:** Contains 5 individual test programs:

1. LED Blink Test
2. DHT11 Sensor Test
3. Bluetooth Echo Test
4. Bluetooth LED Control Test
5. Full Component Test

**Use these to verify each component works before running the main project.**

---

### Documentation Files

#### `README.md`

**Purpose:** Complete project documentation  
**Contents:**

- Features overview
- Hardware requirements & wiring
- Software setup instructions
- Command reference
- Usage examples
- Customization guide
- Troubleshooting basics

**Start here for comprehensive project information.**

---

#### `QUICK_START.md`

**Purpose:** Fast setup guide  
**Contents:**

- 5-minute setup instructions
- Quick wiring reference
- Upload steps
- Basic testing commands
- Pin reference card

**Perfect for getting started quickly.**

---

#### `WIRING_GUIDE.md`

**Purpose:** Detailed circuit diagrams  
**Contents:**

- ASCII circuit diagrams
- Breadboard layout
- Component pinouts (DHT11, LEDs)
- Resistor color codes
- Step-by-step wiring instructions
- Component testing procedures
- Common wiring mistakes
- Safety notes

**Use this when building your circuit.**

---

#### `MOBILE_APP_GUIDE.md`

**Purpose:** Mobile app development  
**Contents:**

- Pre-built app recommendations
- Complete Android app source code (Java)
- Flutter example code
- AndroidManifest.xml configuration
- UI layout XML
- Bluetooth connection handling
- JSON parsing for sensor data
- Testing procedures

**For creating your own custom mobile app.**

---

#### `TROUBLESHOOTING.md`

**Purpose:** Problem-solving guide  
**Contents:**

- Upload & compilation issues
- Bluetooth connection problems
- LED issues
- Sensor problems
- Command issues
- General debugging tips
- Error message explanations
- Complete diagnostic checklist

**Consult this when things aren't working.**

---

### Configuration Files

#### `platformio.ini`

**Purpose:** PlatformIO project configuration  
**Description:** Alternative to Arduino IDE

- Platform: ESP32
- Framework: Arduino
- Libraries: DHT sensor, Adafruit Unified Sensor
- Build flags for Bluetooth
- Upload configuration
- OTA update support

**Use this if you prefer PlatformIO over Arduino IDE.**

---

#### `.gitignore`

**Purpose:** Git version control  
**Description:** Excludes from repository:

- Build files (.pio, .vscode)
- Compiled objects
- IDE files
- Android/iOS build artifacts
- Temporary files

**Automatically configured if you use Git.**

---

## Project Structure

```
SSG434-12/
│
├── esp32_bluetooth_control.ino    ← Main code (upload to ESP32)
├── component_tests.ino             ← Testing utilities
│
├── README.md                       ← Complete documentation
├── QUICK_START.md                  ← Fast setup guide
├── WIRING_GUIDE.md                 ← Circuit diagrams
├── MOBILE_APP_GUIDE.md             ← App development
├── TROUBLESHOOTING.md              ← Problem solving
├── PROJECT_OVERVIEW.md             ← This file
│
├── platformio.ini                  ← PlatformIO config
└── .gitignore                      ← Git ignore rules
```

---

## Quick Navigation

### I want to...

**...get started quickly**
→ Read [QUICK_START.md](QUICK_START.md)

**...understand the full project**
→ Read [README.md](README.md)

**...wire my circuit**
→ Follow [WIRING_GUIDE.md](WIRING_GUIDE.md)

**...test individual components**
→ Use [component_tests.ino](component_tests.ino)

**...create a mobile app**
→ Follow [MOBILE_APP_GUIDE.md](MOBILE_APP_GUIDE.md)

**...fix a problem**
→ Check [TROUBLESHOOTING.md](TROUBLESHOOTING.md)

**...use PlatformIO**
→ Use [platformio.ini](platformio.ini)

---

## Hardware Requirements

### Required Components

- 1× ESP32 Development Board
- 2× LEDs (any color)
- 2× 220Ω Resistors (for LEDs)
- 1× DHT11 Temperature & Humidity Sensor
- 1× 10kΩ Resistor (pull-up for DHT11)
- 1× Breadboard
- Jumper wires (M-M)
- USB cable (for programming)

### Optional Components

- 470µF capacitor (power stabilization)
- 100nF capacitor (sensor stabilization)
- Power adapter (5V 2A)

---

## Software Requirements

### Required Software

- Arduino IDE 1.8+ or PlatformIO
- ESP32 Board Support Package
- Libraries:
  - DHT sensor library (Adafruit)
  - Adafruit Unified Sensor

### Mobile App Options

- Serial Bluetooth Terminal (Android - Recommended)
- Custom Android app (Java/Kotlin)
- Custom Flutter app (cross-platform)

---

## Features Summary

### Communication

✅ Bluetooth Classic (BR/EDR)  
✅ Text-based command protocol  
✅ JSON data format for sensors  
✅ Two-way communication

### Hardware Control

✅ 2× Independent LED control  
✅ GPIO-based switching  
✅ Digital on/off control

### Sensor Reading

✅ DHT11 temperature sensor  
✅ DHT11 humidity sensor  
✅ Real-time data acquisition  
✅ Error handling

### Automation

✅ Auto-send mode (5-second intervals)  
✅ On-demand data requests  
✅ Status reporting

### Software

✅ Robust command parser  
✅ Case-insensitive commands  
✅ Error handling & reporting  
✅ Serial debugging support

---

## Command Reference

| Command      | Function            |
| ------------ | ------------------- |
| `LED1:ON`    | Turn LED 1 ON       |
| `LED1:OFF`   | Turn LED 1 OFF      |
| `LED2:ON`    | Turn LED 2 ON       |
| `LED2:OFF`   | Turn LED 2 OFF      |
| `GET:SENSOR` | Request sensor data |
| `AUTO:ON`    | Enable auto-send    |
| `AUTO:OFF`   | Disable auto-send   |
| `STATUS`     | Get all status info |

---

## Pin Configuration

| Component  | ESP32 Pin |
| ---------- | --------- |
| LED 1      | GPIO 2    |
| LED 2      | GPIO 4    |
| DHT11 Data | GPIO 15   |
| DHT11 VCC  | 3.3V      |
| DHT11 GND  | GND       |

---

## Response Formats

### Success Response

```
OK: LED1 turned ON
```

### Sensor Data Response

```json
SENSOR:{"temp":25.5,"humidity":60.0,"unit":"C"}
```

### Status Response

```
STATUS:
- LED1: ON
- LED2: OFF
- Auto-send: ENABLED
- Temperature: 25.5 C
- Humidity: 60.0 %
```

### Error Response

```
ERROR: Unknown command
ERROR: Failed to read from DHT sensor!
```

---

## Development Workflow

### Initial Setup

1. Read [QUICK_START.md](QUICK_START.md)
2. Wire circuit following [WIRING_GUIDE.md](WIRING_GUIDE.md)
3. Install required libraries
4. Upload [esp32_bluetooth_control.ino](esp32_bluetooth_control.ino)

### Testing

1. Test components with [component_tests.ino](component_tests.ino)
2. Connect via Serial Bluetooth Terminal app
3. Test all commands
4. Verify sensor readings

### Customization

1. Modify pin assignments
2. Add new commands
3. Integrate additional sensors
4. Create custom mobile app

### Troubleshooting

1. Check [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
2. Use Serial Monitor for debugging
3. Test components individually
4. Verify wiring

---

## Customization Examples

### Change Bluetooth Device Name

```cpp
SerialBT.begin("YOUR_CUSTOM_NAME");
```

### Change LED Pins

```cpp
#define LED1_PIN 13  // Change from 2 to 13
#define LED2_PIN 14  // Change from 4 to 14
```

### Change Auto-Send Interval

```cpp
#define AUTO_SEND_INTERVAL 10000  // 10 seconds instead of 5
```

### Use DHT22 Instead of DHT11

```cpp
#define DHT_TYPE DHT22
```

### Add New Command

```cpp
else if (command == "ALL:ON") {
  digitalWrite(LED1_PIN, HIGH);
  digitalWrite(LED2_PIN, HIGH);
  SerialBT.println("OK: All LEDs ON");
}
```

---

## Educational Value

This project teaches:

- **Embedded Systems:** ESP32 microcontroller programming
- **IoT Communication:** Bluetooth Classic protocol
- **Sensor Integration:** Reading environmental sensors
- **Mobile Development:** Android app with Bluetooth
- **Command Parsing:** Text-based protocol design
- **Circuit Design:** LED control, sensor wiring
- **Debugging:** Hardware and software troubleshooting

---

## Possible Enhancements

### Hardware

- Add more LEDs
- Add RGB LED for color control
- Add servo motor
- Add relay module
- Add LCD display
- Add button inputs

### Software

- PWM for LED brightness control
- Data logging to SD card
- WiFi connectivity
- Web server interface
- MQTT integration
- Multi-device support

### Features

- Password protection
- Data visualization
- Historical data storage
- Alarm/threshold alerts
- Scheduled automation
- Voice control integration

---

## Project Statistics

- **Total Lines of Code:** ~350 (main sketch)
- **Number of Commands:** 8
- **GPIO Pins Used:** 3 (2 LEDs + 1 Sensor)
- **Power Consumption:** ~300mA max
- **Bluetooth Range:** ~10 meters
- **Setup Time:** ~15 minutes
- **Skill Level:** Beginner to Intermediate

---

## License & Credits

- **License:** Open source, free to use and modify
- **DHT Library:** Adafruit Industries
- **Bluetooth Library:** Espressif Systems
- **Suitable for:** Education, hobbyist projects, prototyping

---

## Support & Resources

### Documentation

- Start: [QUICK_START.md](QUICK_START.md)
- Reference: [README.md](README.md)
- Wiring: [WIRING_GUIDE.md](WIRING_GUIDE.md)
- Help: [TROUBLESHOOTING.md](TROUBLESHOOTING.md)

### External Resources

- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)
- [DHT Library](https://github.com/adafruit/DHT-sensor-library)

---

## Version History

### Version 1.0

- Initial release
- Basic LED control
- DHT11 sensor reading
- Bluetooth Classic communication
- Command parser
- Auto-send feature
- Complete documentation

---

**Last Updated:** February 28, 2026

**Ready to start?** → [QUICK_START.md](QUICK_START.md)

**Need help?** → [TROUBLESHOOTING.md](TROUBLESHOOTING.md)

**🎉 Have fun building!**
