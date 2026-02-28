# Mobile App Development Guide

This guide provides example code and instructions for creating your own Android app to control the ESP32 Bluetooth device.

## Option 1: Using Pre-Built Apps (Recommended for Beginners)

### Android Apps

#### 1. Serial Bluetooth Terminal (Recommended)

- **Download:** [Google Play Store](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal)
- **Features:** Simple, reliable, supports all text commands
- **Setup:**
  1. Install app
  2. Pair with "ESP32_BT_Controller" in Android Bluetooth settings
  3. Open app → Menu → Devices → Select ESP32
  4. Type commands and send

#### 2. BlueTerm

- **Download:** [Google Play Store](https://play.google.com/store/apps/details?id=es.pymasde.blueterm)
- **Features:** Terminal interface, command history
- **Similar setup to Serial Bluetooth Terminal**

#### 3. Arduino Bluetooth Controller

- **Download:** [Google Play Store](https://play.google.com/store/apps/details?id=com.broxcode.arduinobluetoothfree)
- **Features:** Button interface, customizable buttons
- **Setup:** Configure buttons to send your commands (LED1:ON, etc.)

### iOS Apps

⚠️ **Important:** iOS does not support Bluetooth Classic (SPP/BR/EDR).

**Options:**

1. Use an Android device
2. Modify ESP32 code to use BLE (Bluetooth Low Energy) instead
3. Use a Bluetooth-to-WiFi bridge

---

## Option 2: Build Your Own Android App

### Prerequisites

- Android Studio installed
- Basic Java/Kotlin knowledge
- Android device for testing (Bluetooth Classic not available in emulator)

### Project Setup

#### 1. Create New Android Project

```
File → New → New Project
→ Empty Activity
→ Language: Java
→ Minimum SDK: API 23 (Android 6.0)
```

#### 2. Add Permissions to AndroidManifest.xml

```xml
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.example.esp32controller">

    <!-- Bluetooth permissions -->
    <uses-permission android:name="android.permission.BLUETOOTH" />
    <uses-permission android:name="android.permission.BLUETOOTH_ADMIN" />
    <uses-permission android:name="android.permission.BLUETOOTH_CONNECT" />
    <uses-permission android:name="android.permission.BLUETOOTH_SCAN" />

    <!-- For Android 12+ -->
    <uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" />

    <application
        android:allowBackup="true"
        android:icon="@mipmap/ic_launcher"
        android:label="ESP32 Controller"
        android:roundIcon="@mipmap/ic_launcher_round"
        android:supportsRtl="true"
        android:theme="@style/Theme.ESP32Controller">
        <activity
            android:name=".MainActivity"
            android:exported="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
    </application>

</manifest>
```

#### 3. Main Activity Layout (activity_main.xml)

```xml
<?xml version="1.0" encoding="utf-8"?>
<LinearLayout xmlns:android="http://schemas.android.com/apk/res/android"
    xmlns:tools="http://schemas.android.com/tools"
    android:layout_width="match_parent"
    android:layout_height="match_parent"
    android:orientation="vertical"
    android:padding="16dp"
    tools:context=".MainActivity">

    <!-- Connection Status -->
    <TextView
        android:id="@+id/statusText"
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:text="Not Connected"
        android:textSize="18sp"
        android:textColor="#FF0000"
        android:gravity="center"
        android:padding="12dp"
        android:background="#EEEEEE"
        android:layout_marginBottom="16dp"/>

    <!-- Connect Button -->
    <Button
        android:id="@+id/connectButton"
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:text="Connect to ESP32"
        android:textSize="16sp"
        android:layout_marginBottom="24dp"/>

    <!-- LED Controls -->
    <TextView
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:text="LED Controls"
        android:textSize="20sp"
        android:textStyle="bold"
        android:layout_marginBottom="12dp"/>

    <LinearLayout
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:orientation="horizontal"
        android:layout_marginBottom="8dp">

        <Button
            android:id="@+id/led1OnButton"
            android:layout_width="0dp"
            android:layout_height="wrap_content"
            android:layout_weight="1"
            android:text="LED1 ON"
            android:layout_marginEnd="8dp"/>

        <Button
            android:id="@+id/led1OffButton"
            android:layout_width="0dp"
            android:layout_height="wrap_content"
            android:layout_weight="1"
            android:text="LED1 OFF"/>
    </LinearLayout>

    <LinearLayout
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:orientation="horizontal"
        android:layout_marginBottom="24dp">

        <Button
            android:id="@+id/led2OnButton"
            android:layout_width="0dp"
            android:layout_height="wrap_content"
            android:layout_weight="1"
            android:text="LED2 ON"
            android:layout_marginEnd="8dp"/>

        <Button
            android:id="@+id/led2OffButton"
            android:layout_width="0dp"
            android:layout_height="wrap_content"
            android:layout_weight="1"
            android:text="LED2 OFF"/>
    </LinearLayout>

    <!-- Sensor Controls -->
    <TextView
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:text="Sensor Data"
        android:textSize="20sp"
        android:textStyle="bold"
        android:layout_marginBottom="12dp"/>

    <Button
        android:id="@+id/getSensorButton"
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:text="Get Sensor Data"
        android:layout_marginBottom="8dp"/>

    <LinearLayout
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:orientation="horizontal"
        android:layout_marginBottom="16dp">

        <Button
            android:id="@+id/autoOnButton"
            android:layout_width="0dp"
            android:layout_height="wrap_content"
            android:layout_weight="1"
            android:text="Auto ON"
            android:layout_marginEnd="8dp"/>

        <Button
            android:id="@+id/autoOffButton"
            android:layout_width="0dp"
            android:layout_height="wrap_content"
            android:layout_weight="1"
            android:text="Auto OFF"/>
    </LinearLayout>

    <!-- Sensor Data Display -->
    <TextView
        android:id="@+id/sensorDataText"
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:text="Temperature: --°C\nHumidity: --%"
        android:textSize="16sp"
        android:padding="12dp"
        android:background="#E3F2FD"
        android:layout_marginBottom="16dp"/>

    <!-- Log Output -->
    <TextView
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:text="Log Output"
        android:textSize="16sp"
        android:textStyle="bold"
        android:layout_marginBottom="8dp"/>

    <ScrollView
        android:layout_width="match_parent"
        android:layout_height="0dp"
        android:layout_weight="1">

        <TextView
            android:id="@+id/logOutput"
            android:layout_width="match_parent"
            android:layout_height="wrap_content"
            android:text="Ready to connect..."
            android:textSize="12sp"
            android:fontFamily="monospace"
            android:padding="8dp"
            android:background="#F5F5F5"/>
    </ScrollView>

</LinearLayout>
```

#### 4. MainActivity.java (Main Code)

```java
package com.example.esp32controller;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    // Bluetooth
    private static final UUID BT_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private static final String ESP32_NAME = "ESP32_BT_Controller";
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothSocket bluetoothSocket;
    private OutputStream outputStream;
    private InputStream inputStream;
    private Thread receiveThread;
    private boolean isConnected = false;

    // UI Elements
    private TextView statusText;
    private TextView sensorDataText;
    private TextView logOutput;
    private Button connectButton;
    private Button led1OnButton, led1OffButton;
    private Button led2OnButton, led2OffButton;
    private Button getSensorButton;
    private Button autoOnButton, autoOffButton;

    // Handler for UI updates
    private Handler mainHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Initialize UI elements
        statusText = findViewById(R.id.statusText);
        sensorDataText = findViewById(R.id.sensorDataText);
        logOutput = findViewById(R.id.logOutput);
        connectButton = findViewById(R.id.connectButton);
        led1OnButton = findViewById(R.id.led1OnButton);
        led1OffButton = findViewById(R.id.led1OffButton);
        led2OnButton = findViewById(R.id.led2OnButton);
        led2OffButton = findViewById(R.id.led2OffButton);
        getSensorButton = findViewById(R.id.getSensorButton);
        autoOnButton = findViewById(R.id.autoOnButton);
        autoOffButton = findViewById(R.id.autoOffButton);

        // Initialize Bluetooth
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        mainHandler = new Handler(Looper.getMainLooper());

        // Check if Bluetooth is supported
        if (bluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth not supported", Toast.LENGTH_LONG).show();
            finish();
            return;
        }

        // Set button listeners
        connectButton.setOnClickListener(v -> {
            if (!isConnected) {
                connectToESP32();
            } else {
                disconnect();
            }
        });

        led1OnButton.setOnClickListener(v -> sendCommand("LED1:ON"));
        led1OffButton.setOnClickListener(v -> sendCommand("LED1:OFF"));
        led2OnButton.setOnClickListener(v -> sendCommand("LED2:ON"));
        led2OffButton.setOnClickListener(v -> sendCommand("LED2:OFF"));
        getSensorButton.setOnClickListener(v -> sendCommand("GET:SENSOR"));
        autoOnButton.setOnClickListener(v -> sendCommand("AUTO:ON"));
        autoOffButton.setOnClickListener(v -> sendCommand("AUTO:OFF"));

        // Initially disable control buttons
        setControlsEnabled(false);
    }

    private void connectToESP32() {
        // Check permissions
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.BLUETOOTH_CONNECT,
                            Manifest.permission.BLUETOOTH_SCAN}, 1);
            return;
        }

        // Check if Bluetooth is enabled
        if (!bluetoothAdapter.isEnabled()) {
            Toast.makeText(this, "Please enable Bluetooth", Toast.LENGTH_SHORT).show();
            return;
        }

        addLog("Searching for " + ESP32_NAME + "...");

        // Find paired ESP32 device
        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
        BluetoothDevice esp32Device = null;

        for (BluetoothDevice device : pairedDevices) {
            if (device.getName() != null && device.getName().equals(ESP32_NAME)) {
                esp32Device = device;
                break;
            }
        }

        if (esp32Device == null) {
            addLog("ERROR: ESP32 not found. Please pair it first.");
            Toast.makeText(this, "ESP32 not paired. Pair it in Bluetooth settings first.",
                    Toast.LENGTH_LONG).show();
            return;
        }

        addLog("Found ESP32: " + esp32Device.getAddress());

        // Connect in background thread
        BluetoothDevice finalEsp32Device = esp32Device;
        new Thread(() -> {
            try {
                bluetoothSocket = finalEsp32Device.createRfcommSocketToServiceRecord(BT_UUID);
                bluetoothSocket.connect();
                outputStream = bluetoothSocket.getOutputStream();
                inputStream = bluetoothSocket.getInputStream();

                mainHandler.post(() -> {
                    isConnected = true;
                    updateConnectionStatus(true);
                    addLog("Connected successfully!");
                    setControlsEnabled(true);
                    startReceiving();
                });

            } catch (IOException e) {
                mainHandler.post(() -> {
                    addLog("ERROR: Connection failed - " + e.getMessage());
                    Toast.makeText(MainActivity.this, "Connection failed", Toast.LENGTH_SHORT).show();
                });
            }
        }).start();
    }

    private void disconnect() {
        try {
            if (receiveThread != null) {
                receiveThread.interrupt();
            }
            if (outputStream != null) {
                outputStream.close();
            }
            if (inputStream != null) {
                inputStream.close();
            }
            if (bluetoothSocket != null) {
                bluetoothSocket.close();
            }
            isConnected = false;
            updateConnectionStatus(false);
            setControlsEnabled(false);
            addLog("Disconnected");
        } catch (IOException e) {
            addLog("ERROR during disconnect: " + e.getMessage());
        }
    }

    private void sendCommand(String command) {
        if (!isConnected) {
            Toast.makeText(this, "Not connected", Toast.LENGTH_SHORT).show();
            return;
        }

        new Thread(() -> {
            try {
                outputStream.write((command + "\n").getBytes());
                outputStream.flush();
                mainHandler.post(() -> addLog("Sent: " + command));
            } catch (IOException e) {
                mainHandler.post(() -> addLog("ERROR sending: " + e.getMessage()));
            }
        }).start();
    }

    private void startReceiving() {
        receiveThread = new Thread(() -> {
            byte[] buffer = new byte[1024];
            int bytes;
            StringBuilder messageBuffer = new StringBuilder();

            while (!Thread.currentThread().isInterrupted() && isConnected) {
                try {
                    bytes = inputStream.read(buffer);
                    String received = new String(buffer, 0, bytes);
                    messageBuffer.append(received);

                    // Process complete lines
                    int newlineIndex;
                    while ((newlineIndex = messageBuffer.indexOf("\n")) != -1) {
                        String line = messageBuffer.substring(0, newlineIndex).trim();
                        messageBuffer.delete(0, newlineIndex + 1);

                        if (!line.isEmpty()) {
                            String finalLine = line;
                            mainHandler.post(() -> processReceivedData(finalLine));
                        }
                    }
                } catch (IOException e) {
                    if (isConnected) {
                        mainHandler.post(() -> {
                            addLog("Connection lost");
                            disconnect();
                        });
                    }
                    break;
                }
            }
        });
        receiveThread.start();
    }

    private void processReceivedData(String data) {
        addLog("Received: " + data);

        // Parse sensor data
        if (data.startsWith("SENSOR:")) {
            try {
                String jsonStr = data.substring(7); // Remove "SENSOR:"
                JSONObject json = new JSONObject(jsonStr);
                double temp = json.getDouble("temp");
                double humidity = json.getDouble("humidity");
                String unit = json.getString("unit");

                sensorDataText.setText(String.format("Temperature: %.1f°%s\nHumidity: %.1f%%",
                        temp, unit, humidity));
            } catch (Exception e) {
                addLog("ERROR parsing sensor data: " + e.getMessage());
            }
        }
    }

    private void updateConnectionStatus(boolean connected) {
        if (connected) {
            statusText.setText("Connected");
            statusText.setTextColor(0xFF00FF00); // Green
            connectButton.setText("Disconnect");
        } else {
            statusText.setText("Not Connected");
            statusText.setTextColor(0xFFFF0000); // Red
            connectButton.setText("Connect to ESP32");
        }
    }

    private void setControlsEnabled(boolean enabled) {
        led1OnButton.setEnabled(enabled);
        led1OffButton.setEnabled(enabled);
        led2OnButton.setEnabled(enabled);
        led2OffButton.setEnabled(enabled);
        getSensorButton.setEnabled(enabled);
        autoOnButton.setEnabled(enabled);
        autoOffButton.setEnabled(enabled);
    }

    private void addLog(String message) {
        String currentLog = logOutput.getText().toString();
        logOutput.setText(currentLog + "\n" + message);

        // Auto-scroll to bottom
        final ScrollView scrollView = (ScrollView) logOutput.getParent();
        scrollView.post(() -> scrollView.fullScroll(ScrollView.FOCUS_DOWN));
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        disconnect();
    }
}
```

#### 5. Build and Run

1. Connect your Android device via USB
2. Enable Developer Mode and USB Debugging on your device
3. Build → Make Project
4. Run → Run 'app'

---

## Option 3: Flutter/React Native App

### Flutter Example (main.dart snippet)

```dart
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';

class ESP32Controller extends StatefulWidget {
  @override
  _ESP32ControllerState createState() => _ESP32ControllerState();
}

class _ESP32ControllerState extends State<ESP32Controller> {
  BluetoothConnection? connection;
  bool isConnected = false;
  String sensorData = "No data";

  void sendCommand(String cmd) {
    if (connection != null && connection!.isConnected) {
      connection!.output.add(utf8.encode(cmd + "\n"));
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text('ESP32 Controller')),
      body: Column(
        children: [
          ElevatedButton(
            onPressed: () => sendCommand('LED1:ON'),
            child: Text('LED1 ON'),
          ),
          ElevatedButton(
            onPressed: () => sendCommand('LED1:OFF'),
            child: Text('LED1 OFF'),
          ),
          // Add more buttons...
        ],
      ),
    );
  }
}
```

Dependencies (pubspec.yaml):

```yaml
dependencies:
  flutter_bluetooth_serial: ^0.4.0
```

---

## Testing Your App

### Pre-Deployment Checklist

- [ ] ESP32 is powered on
- [ ] ESP32 Bluetooth name appears in device Bluetooth settings
- [ ] Device is paired with ESP32
- [ ] App has necessary permissions granted
- [ ] Serial Monitor shows "Bluetooth device started"

### Test Sequence

1. **Connection Test**: Connect and verify status shows "Connected"
2. **LED Test**: Try LED1:ON, LED1:OFF, verify physical LED response
3. **LED2 Test**: Try LED2:ON, LED2:OFF
4. **Sensor Test**: Click "Get Sensor Data", verify temperature/humidity display
5. **Auto-Send Test**: Enable AUTO:ON, verify data updates every 5 seconds
6. **Disconnect Test**: Disconnect and verify clean shutdown

---

## Troubleshooting

### 🔴 "Connected but NO Data Received" (MOST COMMON ISSUE!)

**Symptoms:**

- App shows "Connected" ✓
- ESP32 Serial Monitor shows "Bluetooth device started" ✓
- You send "LED1:ON" but nothing happens ✗
- Serial Monitor doesn't show "Received command:" ✗
- LEDs don't respond ✗

**This is the #1 issue! Here's how to fix it:**

#### Quick Fix (Try This First!)

**Problem: App not sending newline character**

In **Serial Bluetooth Terminal** app:

1. Tap **3-dot menu** (top right)
2. Go to **Settings**
3. Find **Send** section
4. Set **"Append"** to **"Newline"** or **"CR+LF"**
5. **Do NOT use "None"**

Then try sending `STATUS` command again.

#### Still Not Working? Follow These Steps:

**Step 1: Verify Serial Monitor is Open**

```
Arduino IDE → Tools → Serial Monitor
Baud Rate: 115200 (bottom-right dropdown)

You MUST see:
"Bluetooth device started, you can pair it with your phone!"

If not → Re-upload the code
```

**Step 2: Check You're Using the RIGHT App Type**

```
❌ WRONG: "BLE Scanner", "nRF Connect", "Bluetooth LE"
          These are for Bluetooth Low Energy (won't work!)

✅ CORRECT: "Serial Bluetooth Terminal" (by Kai Morich)
           "BlueTerm"
           "Arduino Bluetooth Controller"

Using wrong app = Connected but no data flows!
```

**Step 3: Test with Echo Code**

Upload this simple test to verify Bluetooth is working:

```cpp
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Test");
  Serial.println("==================================");
  Serial.println("BT Ready! Waiting for data...");
  Serial.println("Send anything from your phone!");
  Serial.println("==================================");
}

void loop() {
  if (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.print(">>> Received byte: ");
    Serial.print(c);
    Serial.print(" (ASCII: ");
    Serial.print((int)c);
    Serial.println(")");

    SerialBT.print("Echo back: ");
    SerialBT.println(c);
  }
}
```

**What to expect:**

- Connect from phone to "ESP32_Test"
- Send the letter `a`
- Serial Monitor should show: `>>> Received byte: a (ASCII: 97)`
- Phone should receive: `Echo back: a`

**If this works:** Problem is in main code (unlikely)  
**If this doesn't work:** Issue with Bluetooth connection (see below)

**Step 4: Check Data is Actually Being Sent**

In Serial Bluetooth Terminal:

- Look at bottom of screen for **"Sent:"** counter
- Send a command
- Counter should increase
- If counter doesn't change → App isn't sending!

**Step 5: Force Complete Reset**

```
1. ESP32: Press RST/EN button
2. Wait 5 seconds
3. Open Serial Monitor (115200 baud)
4. Verify "Bluetooth device started" appears
5. Phone: Completely close app (swipe away)
6. Reopen app
7. Reconnect to ESP32
8. Try STATUS command
```

**Step 6: Try Different App**

Install "BlueTerm" as backup test:

1. Install from Play Store
2. Open BlueTerm
3. Menu → Connect Device
4. Select "ESP32_BT_Controller"
5. Type: `STATUS` and press Enter
6. Should get response

#### Common Causes Checklist

| Check                          | Fix                                              |
| ------------------------------ | ------------------------------------------------ |
| ⬜ Newline not appended        | Settings → Send → Append → **Newline**           |
| ⬜ Wrong app (BLE not Classic) | Use "Serial Bluetooth Terminal"                  |
| ⬜ Serial Monitor wrong baud   | Set to **115200**                                |
| ⬜ Serial Monitor not open     | Open before testing                              |
| ⬜ ESP32 crashed/frozen        | Press **RST button**                             |
| ⬜ Using iOS device            | **iOS doesn't support BT Classic** - use Android |
| ⬜ Multiple apps connected     | Only one app can connect at once                 |

#### Debug Output Enhancement

Add this to your main code after line 68 to see MORE debug info:

```cpp
void loop() {
  // ADD THIS DEBUG CODE:
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 5000) {
    lastDebug = millis();
    Serial.println("[DEBUG] Still running, waiting for BT data...");
    Serial.print("[DEBUG] BT available: ");
    Serial.println(SerialBT.available());
  }

  // ... rest of your code
```

This prints a message every 5 seconds so you know ESP32 is alive.

---

### "ESP32 not found" Error

- Pair ESP32 in Android Bluetooth settings first
- Ensure ESP32 device name matches exactly: "ESP32_BT_Controller"

### Permission Errors

- Grant all Bluetooth permissions in Android settings
- For Android 12+, also grant Location permission

### Connection Timeout

- Check ESP32 is powered and running
- Try unpairing and re-pairing
- Restart both ESP32 and phone

### Commands Not Working (After Connection Works)

- Ensure commands end with newline character (`\n`)
- Check command format matches exactly (case-insensitive)
- View ESP32 Serial Monitor for debugging

---

## Advanced Features

### Add Custom UI Sliders

For PWM control, future brightness adjustment:

```java
SeekBar brightnessSlider = findViewById(R.id.brightnessSlider);
brightnessSlider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        sendCommand("BRIGHTNESS:" + progress);
    }
    // ... other methods
});
```

### Add Graph for Sensor Data

Use MPAndroidChart library to plot temperature/humidity over time.

---

## Resources

- [Android Bluetooth Documentation](https://developer.android.com/guide/topics/connectivity/bluetooth)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [Serial Bluetooth Terminal GitHub](https://github.com/kai-morich/Serial-Bluetooth-Terminal)

---

**Happy Coding!** 🎉
