# ESP32 Bluetooth Classic (BR/EDR) LED & Sensor Controller

This project uses an ESP32 to control 2 LEDs and read DHT11 sensor data via Bluetooth Classic (BR/EDR). It includes a command parser and auto-send feature for continuous sensor monitoring.

## Features

- ✅ **Bluetooth Classic (BR/EDR)** - Compatible with most mobile devices
- ✅ **Dual LED Control** - Turn on/off two LEDs independently
- ✅ **DHT11 Sensor** - Read temperature and humidity data
- ✅ **Command Parser** - Process text commands from mobile app
- ✅ **Auto-Send Mode** - Automatically send sensor data every 5 seconds
- ✅ **Status Monitoring** - Get current state of all components

## Hardware Requirements

- ESP32 Development Board
- 2x LEDs (any color)
- 2x 220Ω Resistors (for LEDs)
- 1x DHT11 Temperature & Humidity Sensor
- 1x 10kΩ Resistor (pull-up for DHT11, if not built-in)
- Breadboard and jumper wires
- USB cable for programming

## Wiring Diagram

### LED Connections

```
ESP32 GPIO 2  ----> [220Ω Resistor] ----> LED1 (+) ----> GND
ESP32 GPIO 4  ----> [220Ω Resistor] ----> LED2 (+) ----> GND
```

### DHT11 Sensor Connections

```
DHT11 VCC     ----> ESP32 3.3V
DHT11 DATA    ----> ESP32 GPIO 15 (with 10kΩ pull-up to 3.3V if needed)
DHT11 GND     ----> ESP32 GND
```

### Pin Summary

| Component  | ESP32 Pin |
| ---------- | --------- |
| LED 1      | GPIO 2    |
| LED 2      | GPIO 4    |
| DHT11 Data | GPIO 15   |

## Software Setup

### Required Libraries

Install these libraries via Arduino IDE Library Manager:

1. **DHT sensor library** by Adafruit
   - Library Manager → Search "DHT sensor library" → Install
   - Also install "Adafruit Unified Sensor" (dependency)

2. **BluetoothSerial** (Built-in with ESP32 board package)

### Arduino IDE Configuration

1. Install ESP32 board support:
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" → Install

2. Select your board:
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module

3. Configure Bluetooth:
   - Tools → Partition Scheme → "Minimal SPIFFS" or "Default"
   - Ensure Bluetooth is enabled in sdkconfig

4. Upload the code to your ESP32

## Bluetooth Commands

All commands are case-insensitive and should end with a newline character.

### LED Control Commands

| Command    | Description    | Response              |
| ---------- | -------------- | --------------------- |
| `LED1:ON`  | Turn on LED 1  | `OK: LED1 turned ON`  |
| `LED1:OFF` | Turn off LED 1 | `OK: LED1 turned OFF` |
| `LED2:ON`  | Turn on LED 2  | `OK: LED2 turned ON`  |
| `LED2:OFF` | Turn off LED 2 | `OK: LED2 turned OFF` |

### Sensor Commands

| Command      | Description                     | Response Example                                  |
| ------------ | ------------------------------- | ------------------------------------------------- |
| `GET:SENSOR` | Request sensor data immediately | `SENSOR:{"temp":25.5,"humidity":60.0,"unit":"C"}` |

### Auto-Send Commands

| Command    | Description                        | Response                              |
| ---------- | ---------------------------------- | ------------------------------------- |
| `AUTO:ON`  | Enable auto-send (every 5 seconds) | `OK: Auto-send enabled (5s interval)` |
| `AUTO:OFF` | Disable auto-send                  | `OK: Auto-send disabled`              |

### Status Command

| Command  | Description                | Response                 |
| -------- | -------------------------- | ------------------------ |
| `STATUS` | Get all status information | Multi-line status report |

### Response Format

#### Sensor Data Response

```json
SENSOR:{"temp":25.5,"humidity":60.0,"unit":"C"}
```

#### Status Response Example

```
STATUS:
- LED1: ON
- LED2: OFF
- Auto-send: ENABLED
- Temperature: 25.5 C
- Humidity: 60.0 %
```

## Mobile App Setup

### Android - Serial Bluetooth Terminal (Recommended)

1. Install **Serial Bluetooth Terminal** from Google Play Store
2. Pair with ESP32:
   - Settings → Bluetooth → Scan for devices
   - Find "ESP32_BT_Controller" and pair (no PIN required)
3. Open Serial Bluetooth Terminal:
   - Menu → Devices → Select "ESP32_BT_Controller"
   - Connect
4. Send commands from the terminal

### iOS - BLE Scanner or Similar

**Note:** iOS has limited support for Bluetooth Classic. For iOS compatibility, you would need to modify the code to use BLE (Bluetooth Low Energy) instead.

### Custom App Development

See [mobile_app_example.md](mobile_app_example.md) for example code to create your own Android app.

## Usage Examples

### Example 1: Control LEDs

```
> LED1:ON
< OK: LED1 turned ON

> LED2:ON
< OK: LED2 turned ON

> LED1:OFF
< OK: LED1 turned OFF
```

### Example 2: Get Sensor Data

```
> GET:SENSOR
< SENSOR:{"temp":24.8,"humidity":58.5,"unit":"C"}
```

### Example 3: Enable Auto-Send

```
> AUTO:ON
< OK: Auto-send enabled (5s interval)
< SENSOR:{"temp":24.8,"humidity":58.5,"unit":"C"}
< SENSOR:{"temp":24.9,"humidity":58.3,"unit":"C"}
< SENSOR:{"temp":24.8,"humidity":58.5,"unit":"C"}
... (continues every 5 seconds)

> AUTO:OFF
< OK: Auto-send disabled
```

### Example 4: Check Status

```
> STATUS
< STATUS:
< - LED1: ON
< - LED2: OFF
< - Auto-send: DISABLED
< - Temperature: 24.8 C
< - Humidity: 58.5 %
```

## Customization

### Change Bluetooth Device Name

Edit line 57 in `esp32_bluetooth_control.ino`:

```cpp
SerialBT.begin("ESP32_BT_Controller"); // Change name here
```

### Change LED Pins

Edit lines 31-32:

```cpp
#define LED1_PIN 2  // Change to your pin
#define LED2_PIN 4  // Change to your pin
```

### Change DHT11 Pin

Edit line 33:

```cpp
#define DHT_PIN 15  // Change to your pin
```

### Change Auto-Send Interval

Edit line 38 (value in milliseconds):

```cpp
#define AUTO_SEND_INTERVAL 5000  // 5000ms = 5 seconds
```

### Use DHT22 Instead of DHT11

Edit line 36:

```cpp
#define DHT_TYPE DHT22  // Change from DHT11 to DHT22
```

## Troubleshooting

### Bluetooth Connection Issues

- **Can't find ESP32:** Make sure Bluetooth is enabled on your phone and ESP32 is powered on
- **Won't pair:** Try unpairing and re-pairing the device
- **Disconnects frequently:** Check power supply (use USB power, not computer USB port if possible)

### Sensor Reading Errors

- **"ERROR: Failed to read from DHT sensor!"**
  - Check wiring connections
  - Ensure 10kΩ pull-up resistor is connected to DATA pin
  - Wait 2 seconds after power-on before first reading
  - Try replacing the DHT11 sensor (they can be unreliable)

### LEDs Not Working

- Check LED polarity (long leg = positive/anode)
- Verify resistor values (220Ω recommended)
- Test pins with simple blink sketch first
- Check GPIO pin numbers in code match your wiring

### Upload Issues

- Press and hold BOOT button while uploading
- Select correct board: "ESP32 Dev Module"
- Check correct COM port is selected
- Try reducing upload speed (Tools → Upload Speed → 115200)

## Serial Monitor Debugging

Open Serial Monitor at 115200 baud to see debug messages:

- Commands received
- Sensor readings
- LED state changes
- Bluetooth status

## Advanced Features

### Adding More Commands

To add custom commands, edit the `processCommand()` function around line 84:

```cpp
else if (command == "YOUR_COMMAND") {
  // Your code here
  SerialBT.println("OK: Your response");
}
```

### Adding More Sensors

1. Define sensor pins
2. Initialize sensor in `setup()`
3. Create read function
4. Add to sensor data output in `sendSensorData()`

## License

This project is open source and available for educational purposes.

## Credits

- Uses Adafruit DHT sensor library
- ESP32 BluetoothSerial library by Espressif

## Support

For issues or questions:

1. Check wiring against diagram
2. Verify all libraries are installed
3. Check Serial Monitor for error messages
4. Test components individually

---

**Enjoy your ESP32 Bluetooth project!** 🚀
