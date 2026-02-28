# ESP32 BLE Sensor Monitoring System

## SSG434 Lab 13

A comprehensive Bluetooth Low Energy (BLE) system for monitoring DHT11 sensor data and controlling an LED remotely from a mobile device.

---

## 📋 Project Overview

This project demonstrates BLE communication between an ESP32 microcontroller and mobile devices. It creates a sensor monitoring system that:

- Reads temperature and humidity from a DHT11 sensor
- Sends automatic data updates via BLE notifications
- Allows remote LED control through BLE commands
- Provides device information through standard BLE services

---

## 🔌 Hardware Requirements

### Components

- **ESP32 Development Board** (ESP32-WROOM-32 or similar)
- **DHT11 Temperature & Humidity Sensor**
- **LED** (any color)
- **220Ω Resistor** (for LED)
- **10kΩ Resistor** (pull-up for DHT11, if not integrated)
- **Breadboard and Jumper Wires**
- **USB Cable** (for programming and power)

### Pin Connections

| Component  | ESP32 Pin | Notes                         |
| ---------- | --------- | ----------------------------- |
| LED (+)    | GPIO 12   | Through 220Ω resistor         |
| LED (-)    | GND       |                               |
| DHT11 VCC  | 3.3V      |                               |
| DHT11 DATA | GPIO 15   | May need 10kΩ pull-up to 3.3V |
| DHT11 GND  | GND       |                               |

### Wiring Diagram

```
ESP32                    DHT11
┌─────────┐             ┌─────┐
│         │             │     │
│  3.3V   ├─────────────┤ VCC │
│  GND    ├─────────────┤ GND │
│  GPIO15 ├─────────────┤ DATA│
│         │             └─────┘
│         │
│  GPIO12 ├───[220Ω]───LED───GND
│         │
└─────────┘
```

---

## 🚀 Software Setup

### Arduino IDE Configuration

1. **Install ESP32 Board Support:**
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" → Install

2. **Install Required Libraries:**
   - Sketch → Include Library → Manage Libraries
   - Install:
     - `DHT sensor library` by Adafruit
     - `Adafruit Unified Sensor` (dependency)
   - **Note:** BLE libraries are built-in with ESP32 core

3. **Board Configuration:**
   - Board: "ESP32 Dev Module"
   - Upload Speed: 115200
   - Flash Frequency: 80MHz
   - CPU Frequency: 240MHz
   - Port: Select your ESP32 COM port

4. **Upload the Code:**
   - Open `ESP32_BLE_Sensor_Monitor.ino`
   - Click Upload (→) button
   - Wait for "Done uploading" message

---

## 📡 BLE Architecture

### Service Structure

The system implements three BLE services:

#### 1️⃣ **Device Information Service** (UUID: `180A`)

Standard BLE service providing device metadata.

| Characteristic | UUID   | Properties | Description       |
| -------------- | ------ | ---------- | ----------------- |
| Device Name    | `2A00` | Read       | Device identifier |
| Manufacturer   | `2A29` | Read       | Manufacturer name |

#### 2️⃣ **Sensor Service** (UUID: `4fafc201-1fb5-459e-8fcc-c5c9c331914b`)

Custom service for environmental monitoring.

| Characteristic | UUID                                   | Properties   | Description       |
| -------------- | -------------------------------------- | ------------ | ----------------- |
| Temperature    | `beb5483e-36e1-4688-b7f5-ea07361b26a8` | Read, Notify | Temperature in °C |
| Humidity       | `beb5483e-36e1-4688-b7f5-ea07361b26a9` | Read, Notify | Humidity in %     |

**Notification Behavior:**

- Automatic updates every 2 seconds when connected
- Only sends updates when values change by >0.1 units
- Values formatted as strings (e.g., "23.5")

#### 3️⃣ **Control Service** (UUID: `6e400001-b5a3-f393-e0a9-e50e24dcca9e`)

Custom service for device control.

| Characteristic | UUID                                   | Properties   | Description          |
| -------------- | -------------------------------------- | ------------ | -------------------- |
| LED Control    | `6e400002-b5a3-f393-e0a9-e50e24dcca9e` | Write        | Send commands to LED |
| LED Status     | `6e400003-b5a3-f393-e0a9-e50e24dcca9e` | Read, Notify | Current LED state    |

**LED Control Commands:**

- `1` or `ON` or `on` → Turn LED ON
- `0` or `OFF` or `off` → Turn LED OFF
- `TOGGLE` or `toggle` → Toggle LED state

---

## 📱 Testing with Mobile Apps

### Recommended BLE Apps

#### **For Android:**

1. **nRF Connect** (Recommended ⭐)
   - Download: Google Play Store
   - Developer: Nordic Semiconductor
   - Best for: Professional testing and debugging

2. **BLE Scanner**
   - Download: Google Play Store
   - Developer: Bluepixel Technologies
   - Best for: Simple, user-friendly interface

3. **Serial Bluetooth Terminal**
   - Download: Google Play Store
   - Best for: Text-based communication

#### **For iOS:**

1. **nRF Connect** (Recommended ⭐)
   - Download: App Store
   - Developer: Nordic Semiconductor
   - Best for: Professional testing

2. **LightBlue Explorer**
   - Download: App Store
   - Developer: Punch Through
   - Best for: iOS users, clean UI

3. **BLE Scanner 4.0**
   - Download: App Store
   - Best for: Quick scanning and testing

### Testing Procedure with nRF Connect

#### Step 1: Initial Connection

1. Open nRF Connect app
2. Tap "SCAN" button
3. Look for device named **"ESP32_Sensor_Monitor"**
4. Tap "CONNECT"

#### Step 2: Explore Services

Once connected, you'll see three services:

- **Device Information** (0x180A)
- **Unknown Service** (4fafc201...) - This is Sensor Service
- **Unknown Service** (6e400001...) - This is Control Service

#### Step 3: Read Device Information

1. Expand "Device Information Service"
2. Tap read icon (↓) on "Device Name"
3. Should display: "ESP32_Sensor_Monitor"
4. Tap read icon on "Manufacturer Name String"
5. Should display: "SSG434 Labs"

#### Step 4: Monitor Sensor Data

1. Expand the Sensor Service (4fafc201...)
2. Find Temperature characteristic (beb5483e...26a8)
3. Tap the **three-arrows icon** (🔔) to enable notifications
4. Observe real-time temperature updates every 2 seconds
5. Repeat for Humidity characteristic (beb5483e...26a9)
6. Watch values update automatically

#### Step 5: Control LED

1. Expand the Control Service (6e400001...)
2. Find LED Control characteristic (6e400002...)
3. Tap **up-arrow icon** (↑) to write
4. Select "Text" format
5. Type `ON` and send → LED should turn on
6. Type `OFF` and send → LED should turn off
7. Type `TOGGLE` and send → LED should toggle state

#### Step 6: Monitor LED Status

1. In Control Service, find LED Status characteristic (6e400003...)
2. Enable notifications (🔔 icon)
3. Control the LED and watch status updates automatically
4. Status will show "ON" or "OFF"

### Expected Serial Monitor Output

When connected and running, you should see:

```
=== ESP32 BLE Sensor Monitor ===
DHT11 sensor initialized
Device Info Service started
Sensor Service started
Control Service started

✓ BLE Advertising started
Waiting for client connection...

Device Name: ESP32_Sensor_Monitor
Ready to accept connections!

Client Connected!
Temperature: 23.5°C | Humidity: 45.2%
LED Control received: ON
LED ON
Temperature: 23.6°C | Humidity: 45.3%
```

---

## 🔧 Troubleshooting

### Common Issues

| Problem                      | Possible Cause  | Solution                                    |
| ---------------------------- | --------------- | ------------------------------------------- |
| Device not appearing in scan | Not advertising | Check serial monitor, reboot ESP32          |
| Can't connect to device      | BLE stack issue | Reboot phone's Bluetooth, restart ESP32     |
| No sensor readings           | Wrong wiring    | Verify DHT11 connections, check pin 15      |
| Sensor shows NaN             | Power issue     | Check 3.3V connection, add pull-up resistor |
| LED not responding           | Wrong pin       | Verify LED on pin 12, check orientation     |
| Connection drops frequently  | Range issue     | Move closer to ESP32                        |
| No notifications             | Not enabled     | Enable notifications (🔔) in BLE app        |

### Debugging Tips

1. **Always check Serial Monitor first** (115200 baud)
2. **Verify pin connections** match code (LED=12, DHT=15)
3. **Test DHT sensor separately** before BLE testing
4. **Ensure phone Bluetooth is ON** and location enabled (Android)
5. **Try different BLE app** if one isn't working
6. **Reboot ESP32** if behavior becomes erratic

---

## 📲 Creating Your Own Mobile App

If you want to create a custom mobile app instead of using generic BLE apps, here are the requirements:

### Flutter App Example (Cross-Platform)

#### Required Package

```yaml
dependencies:
  flutter_blue_plus: ^1.14.0
```

#### Basic Implementation Structure

```dart
import 'package:flutter_blue_plus/flutter_blue_plus.dart';

// UUIDs from ESP32
final sensorServiceUuid = Guid("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
final temperatureCharUuid = Guid("beb5483e-36e1-4688-b7f5-ea07361b26a8");
final humidityCharUuid = Guid("beb5483e-36e1-4688-b7f5-ea07361b26a9");
final controlServiceUuid = Guid("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
final ledControlCharUuid = Guid("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
final ledStatusCharUuid = Guid("6e400003-b5a3-f393-e0a9-e50e24dcca9e");

// Scan for devices
Future<void> scanDevices() async {
  FlutterBluePlus.startScan(timeout: Duration(seconds: 4));

  FlutterBluePlus.scanResults.listen((results) {
    for (ScanResult r in results) {
      if (r.device.name == "ESP32_Sensor_Monitor") {
        connectToDevice(r.device);
      }
    }
  });
}

// Connect to ESP32
Future<void> connectToDevice(BluetoothDevice device) async {
  await device.connect();
  List<BluetoothService> services = await device.discoverServices();

  for (BluetoothService service in services) {
    if (service.uuid == sensorServiceUuid) {
      for (BluetoothCharacteristic c in service.characteristics) {
        if (c.uuid == temperatureCharUuid) {
          // Enable notifications
          await c.setNotifyValue(true);
          c.value.listen((value) {
            String temp = String.fromCharCodes(value);
            print("Temperature: $temp°C");
            // Update UI with temperature
          });
        }
        if (c.uuid == humidityCharUuid) {
          await c.setNotifyValue(true);
          c.value.listen((value) {
            String humidity = String.fromCharCodes(value);
            print("Humidity: $humidity%");
            // Update UI with humidity
          });
        }
      }
    }

    if (service.uuid == controlServiceUuid) {
      for (BluetoothCharacteristic c in service.characteristics) {
        if (c.uuid == ledControlCharUuid) {
          // Control LED
          await c.write("ON".codeUnits);  // Turn ON
          await c.write("OFF".codeUnits); // Turn OFF
        }
      }
    }
  }
}
```

### React Native App Example

#### Required Package

```bash
npm install react-native-ble-plx
```

#### Basic Implementation

```javascript
import { BleManager } from "react-native-ble-plx";

const manager = new BleManager();

// UUIDs
const SENSOR_SERVICE = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const TEMP_CHAR = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
const HUMIDITY_CHAR = "beb5483e-36e1-4688-b7f5-ea07361b26a9";
const CONTROL_SERVICE = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
const LED_CONTROL = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";

// Scan and connect
manager.startDeviceScan(null, null, (error, device) => {
  if (device?.name === "ESP32_Sensor_Monitor") {
    manager.stopDeviceScan();
    device
      .connect()
      .then((device) => device.discoverAllServicesAndCharacteristics())
      .then((device) => {
        // Monitor temperature
        device.monitorCharacteristicForService(
          SENSOR_SERVICE,
          TEMP_CHAR,
          (error, characteristic) => {
            const temp = atob(characteristic.value);
            console.log("Temperature:", temp);
          },
        );

        // Control LED
        device.writeCharacteristicWithResponseForService(
          CONTROL_SERVICE,
          LED_CONTROL,
          btoa("ON"),
        );
      });
  }
});
```

### Native Android App (Java/Kotlin)

#### Key Classes to Use

- `BluetoothAdapter` - BLE initialization
- `BluetoothLeScanner` - Device scanning
- `BluetoothGatt` - Connection management
- `BluetoothGattCharacteristic` - Read/Write operations

#### Basic Flow

1. Request Bluetooth permissions
2. Scan for devices using `startScan()`
3. Connect using `device.connectGatt()`
4. Discover services with `discoverServices()`
5. Read/Write characteristics
6. Enable notifications via descriptors

### Native iOS App (Swift)

#### Key Frameworks

```swift
import CoreBluetooth

class BLEManager: NSObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    var centralManager: CBCentralManager!
    var peripheral: CBPeripheral!

    // Service UUIDs
    let sensorServiceUUID = CBUUID(string: "4fafc201-1fb5-459e-8fcc-c5c9c331914b")
    let tempCharUUID = CBUUID(string: "beb5483e-36e1-4688-b7f5-ea07361b26a8")

    // Scan, connect, read/write implementation
}
```

### App Features to Implement

1. **Device Scanner Screen**
   - List nearby BLE devices
   - Show signal strength (RSSI)
   - Connect button for ESP32_Sensor_Monitor

2. **Dashboard Screen**
   - Real-time temperature display (with icon/gauge)
   - Real-time humidity display (with icon/gauge)
   - LED control toggle button
   - LED status indicator
   - Connection status

3. **Settings Screen**
   - Device info (name, manufacturer)
   - Disconnect button
   - Reconnect option

4. **Nice-to-Have Features**
   - Data logging/history graphs
   - Temperature/humidity alerts
   - Auto-reconnect on disconnect
   - Multiple device support

---

## 🎓 Learning Outcomes

By completing this lab, you will understand:

✅ **BLE Fundamentals**

- GATT (Generic Attribute Profile) architecture
- Services and Characteristics structure
- UUIDs and their purposes (standard vs custom)

✅ **BLE Operations**

- Read operations for static data
- Write operations for control commands
- Notify operations for real-time data streaming

✅ **ESP32 BLE Programming**

- Creating BLE services and characteristics
- Implementing callbacks for events
- Managing connections and advertising

✅ **Sensor Integration**

- Reading DHT11 sensor data
- Handling sensor errors
- Implementing update intervals

✅ **Mobile-ESP32 Communication**

- BLE scanning and connection from mobile
- Bidirectional data exchange
- Real-time notifications

---

## 📚 Additional Resources

### Documentation

- [ESP32 BLE Arduino Documentation](https://github.com/nkolban/ESP32_BLE_Arduino)
- [BLE GATT Services](https://www.bluetooth.com/specifications/gatt/)
- [DHT Sensor Library](https://github.com/adafruit/DHT-sensor-library)

### Video Tutorials

- Search YouTube: "ESP32 BLE Tutorial"
- Recommended: "ESP32 BLE with nRF Connect"

### BLE Learning

- [Introduction to BLE](https://learn.adafruit.com/introduction-to-bluetooth-low-energy)
- [BLE GATT Profile](https://www.bluetooth.com/bluetooth-resources/intro-to-bluetooth-gap-gatt/)

---

## 🔐 Security Notes

**Important:** This is a lab project for educational purposes. For production use:

- Implement BLE pairing and bonding
- Add encryption for sensitive data
- Use secure OTA (Over-The-Air) updates
- Implement input validation
- Add authentication mechanisms

---

## 📝 Assignment Questions

1. What is the difference between BLE Read and Notify operations?
2. Why use UUIDs for services and characteristics?
3. How does the notification system reduce power consumption?
4. What happens if multiple devices try to connect simultaneously?
5. Explain the callback mechanism in BLE communication.

---

## 🤝 Support

If you encounter issues:

1. Check the Troubleshooting section
2. Verify all connections match the wiring diagram
3. Review Serial Monitor output at 115200 baud
4. Test with nRF Connect app first before custom apps
5. Contact instructor/TA with specific error messages

---

## ✅ Lab Completion Checklist

- [ ] ESP32 successfully programmed
- [ ] LED connected to GPIO 12 and working
- [ ] DHT11 connected to GPIO 15 and reading data
- [ ] Device appears in BLE scan as "ESP32_Sensor_Monitor"
- [ ] Can connect from mobile device
- [ ] Temperature notifications working
- [ ] Humidity notifications working
- [ ] LED control commands working (ON/OFF/TOGGLE)
- [ ] LED status updates correctly
- [ ] All three services accessible
- [ ] Serial Monitor showing correct output

---

**Lab 13 Complete!** 🎉

You've successfully built a BLE-enabled IoT sensor monitoring system with real-time data transmission and remote control capabilities!
