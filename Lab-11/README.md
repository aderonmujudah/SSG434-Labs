# ESP32 Web Server Control Panel

This project creates an ESP32 access point with a web server to control LEDs and monitor DHT11 sensor data in real-time.

## Features

- ✅ Access Point mode (no router needed)
- ✅ Control 2 LEDs via web interface
- ✅ Live DHT11 temperature and humidity readings (auto-updates every 2 seconds)
- ✅ Modern, responsive web interface
- ✅ Separate HTML, CSS, and JavaScript files

## Hardware Requirements

- ESP32 board
- 2 LEDs (with appropriate resistors, typically 220Ω)
- DHT11 temperature and humidity sensor
- Jumper wires
- Breadboard (optional)

## Required Libraries

Install these libraries via Arduino IDE Library Manager:

1. **DHT sensor library** by Adafruit
2. **Adafruit Unified Sensor** by Adafruit

## Setup Instructions

### 1. Install Libraries

- Open Arduino IDE
- Go to Sketch → Include Library → Manage Libraries
- Search for "DHT sensor library" and install it
- Search for "Adafruit Unified Sensor" and install it

### 2. Configure Pin Connections

Edit the top of `ESP32_WebServer.ino`:

```cpp
#define LED1_PIN    2   // Example: GPIO 2
#define LED2_PIN    4   // Example: GPIO 4
#define DHT_PIN     15  // Example: GPIO 15
```

### 3. Configure WiFi Credentials

Edit the access point credentials:

```cpp
const char* ssid = "ESP32-Control";     // Your AP name
const char* password = "12345678";      // Min 8 chars (or empty for open network)
```

### 4. Wiring Diagram

```
ESP32          DHT11
-----          -----
GPIO 15  ----> DATA
3.3V     ----> VCC
GND      ----> GND

ESP32          LED 1
-----          -----
GPIO 2   ----> Anode (+)
GND      ----> Cathode (-) via 220Ω resistor

ESP32          LED 2
-----          -----
GPIO 4   ----> Anode (+)
GND      ----> Cathode (-) via 220Ω resistor
```

### 5. Upload to ESP32

1. Connect your ESP32 to your computer
2. Select the correct board: Tools → Board → ESP32 → ESP32 Dev Module
3. Select the correct port: Tools → Port → (your ESP32 port)
4. Click Upload

### 6. Access the Web Interface

1. Open Serial Monitor (115200 baud) to see the IP address
2. Connect your phone/computer to the ESP32 WiFi network
3. Open a web browser and navigate to the IP address shown (usually `192.168.4.1`)

## Usage

- **LED Control**: Click the buttons to toggle LEDs on/off
- **Sensor Data**: Temperature and humidity update automatically every 2 seconds
- **Status Indicator**: Green pulsing dot indicates active connection

## Troubleshooting

### "Failed to read from DHT sensor"

- Check wiring connections
- Ensure DHT11 is getting 3.3V power
- Verify the correct GPIO pin is configured

### Cannot connect to access point

- Ensure password is at least 8 characters or empty
- Check that ESP32 is powered and code is uploaded
- Look for the SSID name you configured

### Web page not loading

- Make sure you're connected to the ESP32 WiFi network
- Try navigating to `192.168.4.1` directly
- Check Serial Monitor for the correct IP address

## Files Description

- **ESP32_WebServer.ino**: Main Arduino sketch (upload this file)
- **index.html**: Web page structure (embedded in .ino)
- **style.css**: Styling (embedded in .ino)
- **script.js**: Client-side JavaScript (embedded in .ino)

## Notes

- The HTML, CSS, and JS files are provided separately for easy editing
- They are embedded as strings in the .ino file for easy deployment
- Sensor data updates every 2 seconds automatically
- The web interface is fully responsive and works on mobile devices

## License

Free to use and modify for personal and educational projects.
