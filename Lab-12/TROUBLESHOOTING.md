# Troubleshooting Guide

Comprehensive solutions for common issues with the ESP32 Bluetooth Controller project.

## Table of Contents

- [Upload & Compilation Issues](#upload--compilation-issues)
- [Bluetooth Connection Problems](#bluetooth-connection-problems)
- [LED Issues](#led-issues)
- [Sensor Problems](#sensor-problems)
- [Command Issues](#command-issues)
- [General Tips](#general-tips)

---

## Upload & Compilation Issues

### ❌ Error: "Compilation error: 'BluetoothSerial' does not name a type"

**Cause:** ESP32 board package not installed or Bluetooth not enabled

**Solutions:**

1. Install ESP32 board support:
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" → Install latest version

2. Select correct board:
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module

3. Check Partition Scheme:
   - Tools → Partition Scheme → Select "Default" or "Minimal SPIFFS"

---

### ❌ Error: "fatal error: DHT.h: No such file or directory"

**Cause:** DHT library not installed

**Solution:**

1. Tools → Manage Libraries
2. Search "DHT sensor library"
3. Install "DHT sensor library" by Adafruit
4. Also install "Adafruit Unified Sensor" (dependency)

---

### ❌ Upload Keeps Failing / Times Out

**Symptoms:**

```
Connecting........_____....._____
A fatal error occurred: Failed to connect to ESP32
```

**Solutions:**

**Solution 1:** Hold BOOT button

- Press and HOLD the BOOT button on ESP32
- Click Upload in Arduino IDE
- Keep holding BOOT until "Connecting..." changes to "Writing..."
- Release BOOT button

**Solution 2:** Check COM Port

- Tools → Port → Select the correct port (usually appears as "COM# (ESP32)")
- If no port appears, check USB cable (must be data cable, not charge-only)

**Solution 3:** Reduce Upload Speed

- Tools → Upload Speed → 115200 (slower but more reliable)

**Solution 4:** Install USB Drivers

- For most ESP32 boards: Install CP210x or CH340 drivers
- Search "[Your ESP32 Board] USB Driver" online

**Solution 5:** Try Different USB Port/Cable

- Use USB ports directly on computer (not USB hub)
- Try a different USB cable
- Some cables are charge-only and don't support data

---

### ❌ Error: "Sketch too large"

**Cause:** Not enough space in flash memory partition

**Solution:**

- Tools → Partition Scheme → Select "Huge APP (3MB No OTA/1MB SPIFFS)"

---

## Bluetooth Connection Problems

### ❌ Can't Find "ESP32_BT_Controller" in Bluetooth Settings

**Check List:**

1. Is ESP32 powered on? (LED should be on)
2. Is code uploaded successfully?
3. Open Serial Monitor (115200 baud) - should show "Bluetooth device started"
4. Is Bluetooth enabled on your phone?
5. Try scanning again after 30 seconds

**Solutions:**

**Solution 1:** Verify Bluetooth Started

```
Open Serial Monitor (115200 baud)
Should see:
  Bluetooth device started, you can pair it with your phone!
  Device Name: ESP32_BT_Controller
```

**Solution 2:** Restart ESP32

- Press EN (Enable/Reset) button on ESP32
- Or unplug and replug USB

**Solution 3:** Check Bluetooth Name

- Verify line 57 in code: `SerialBT.begin("ESP32_BT_Controller");`
- Name must be unique if multiple ESP32s nearby

**Solution 4:** Android Location Services

- Android 12+: Settings → Apps → Bluetooth App → Permissions → Enable Location
- Reason: Android uses location for Bluetooth scanning

---

### ❌ "ESP32_BT_Controller" Appears but Won't Pair

**Solutions:**

**Solution 1:** Forget and Re-Pair

1. Android Bluetooth Settings → "ESP32_BT_Controller" → Forget
2. Restart ESP32
3. Scan and pair again

**Solution 2:** Clear Bluetooth Cache (Android)

1. Settings → Apps → Show System Apps → Bluetooth
2. Storage → Clear Cache
3. Restart phone
4. Try pairing again

**Solution 3:** No PIN Required

- ESP32 Classic Bluetooth typically doesn't require PIN
- If asked, try: 0000 or 1234 (though usually not needed)

---

### ❌ Paired but App Won't Connect

**Solutions:**

**Solution 1:** Use Correct App Type

- Need **Bluetooth Classic** (SPP) app, not BLE app
- Recommended: "Serial Bluetooth Terminal" from Play Store

**Solution 2:** Check Permissions

- Android Settings → Apps → Bluetooth Terminal → Permissions
- Enable all Bluetooth and Location permissions

**Solution 3:** Close Other Apps

- Close other apps that might use ESP32 Bluetooth
- Only one app can connect at a time

**Solution 4:** Restart Both Devices

1. Close Bluetooth app
2. Restart ESP32
3. Reopen app and connect

---

### ❌ Connection Drops Frequently

**Causes & Solutions:**

**Cause 1:** Weak Power Supply

- **Solution:** Use powered USB hub or wall adapter (5V 1A+)
- Computer USB ports may not provide enough current

**Cause 2:** Distance/Obstacles

- **Solution:** Keep phone within 10 meters, reduce obstacles
- Bluetooth Classic range: ~10m indoor

**Cause 3:** Interference

- **Solution:** Move away from WiFi routers, microwaves, other Bluetooth devices

**Cause 4:** Low Battery

- **Solution:** Charge phone to >20%

---

### ❌ iOS Device Can't Connect

**Issue:** iOS doesn't support Bluetooth Classic (SPP/BR/EDR)

**Solutions:**

1. **Use Android device** (Recommended)
2. **Modify project to use BLE** instead of Classic Bluetooth
   - Requires rewriting ESP32 code for BLE
   - Different communication protocol
3. **Use Bluetooth-WiFi Bridge**
   - Connect ESP32 to WiFi module
   - Access via HTTP/WebSocket from iOS

---

## LED Issues

### ❌ LEDs Don't Light Up at All

**Check List:**

1. LED polarity correct? (Long leg = +, short leg = -)
2. Resistors connected? (220Ω)
3. Is GPIO pin number correct in code?

**Diagnostic Steps:**

**Step 1:** Test LED manually

- Disconnect LED from ESP32
- Connect LED: 3.3V → Resistor → LED+ → LED- → GND
- If doesn't light: LED may be dead or backwards

**Step 2:** Test GPIO Pin

```cpp
void setup() {
  pinMode(2, OUTPUT);
}
void loop() {
  digitalWrite(2, HIGH);
  delay(1000);
  digitalWrite(2, LOW);
  delay(1000);
}
```

Upload and check if LED blinks. If not, pin may be damaged.

**Step 3:** Check Resistor Value

- Use multimeter to verify 220Ω (Red-Red-Brown)
- Too high resistance = dim LED
- Too low = may damage LED/ESP32

**Step 4:** Verify Wiring

```
Correct: GPIO → Resistor → LED+ (long leg) → LED- (short leg) → GND
Wrong:   GND → LED → Resistor → GPIO  (won't work)
```

---

### ❌ LED Always On (Won't Turn Off)

**Possible Causes:**

**Cause 1:** Wrong GPIO Pin

- **Solution:** Some pins have special functions
- Avoid: GPIO 0, 2, 12, 15 (boot pins)
- Recommended: GPIO 13, 14, 16-33

**Cause 2:** Code Logic Error

- **Solution:** Check `processCommand()` function
- Verify `digitalWrite(PIN, LOW)` is called for OFF commands

**Cause 3:** Short Circuit

- **Solution:** Check for accidental connections
- LED cathode (-) **must** go to GND, not VCC

---

### ❌ LED Very Dim

**Causes & Solutions:**

**Cause 1:** Resistor Too High

- **Solution:** Use 220Ω (Red-Red-Brown), not 2.2kΩ (Red-Red-Red)

**Cause 2:** Wrong Voltage

- **Solution:** Ensure LED connected to GPIO (3.3V), not GND

**Cause 3:** Weak Power Supply

- **Solution:** Use better USB power source

**Cause 4:** LED Dying

- **Solution:** Replace LED

---

## Sensor Problems

### ❌ "ERROR: Failed to read from DHT sensor!"

**Most Common Cause:** No pull-up resistor on DATA pin

**Solutions:**

**Solution 1:** Add Pull-Up Resistor

- Connect 10kΩ resistor between DHT11 DATA pin and 3.3V
- This is REQUIRED for DHT11 to work reliably

```
3.3V ──┬── DHT11 VCC
       │
       └──[10kΩ]── DHT11 DATA ── GPIO 15
```

**Solution 2:** Check Wiring

```
DHT11 Pin 1 (VCC) → ESP32 3.3V
DHT11 Pin 2 (DATA) → ESP32 GPIO 15 (+ 10kΩ to 3.3V)
DHT11 Pin 4 (GND) → ESP32 GND
```

**Solution 3:** Wait for Stabilization

- DHT11 needs 1-2 seconds after power-on
- Add `delay(2000);` in `setup()` before first read

**Solution 4:** Check Power

- Don't use 5V for DHT11 (can damage sensor)
- Use 3.3V from ESP32

**Solution 5:** Replace Sensor

- DHT11 sensors can be unreliable or defective
- Try a different DHT11 module

---

### ❌ Sensor Readings Are Unrealistic

**Examples:** Temperature = 0°C or 85°C, Humidity = 0% or 100%

**Causes & Solutions:**

**Cause 1:** Reading Too Fast

- **Solution:** Wait at least 2 seconds between readings
- `delay(2000);` in loop or use millis() timer

**Cause 2:** No Pull-Up Resistor

- **Solution:** Add 10kΩ resistor from DATA to 3.3V

**Cause 3:** Wrong DHT Type

- **Solution:** Verify you have DHT11, not DHT22
- If DHT22: Change `#define DHT_TYPE DHT22` in code

**Cause 4:** Wiring Too Long

- **Solution:** Keep wires under 20cm
- Long wires cause signal degradation

**Cause 5:** Power Supply Noise

- **Solution:** Add 100nF capacitor between VCC and GND

---

### ❌ Sensor Readings Fluctuate Wildly

**Solutions:**

1. **Add Averaging:**

```cpp
float readTemperature() {
  float sum = 0;
  for(int i=0; i<5; i++) {
    sum += dht.readTemperature();
    delay(500);
  }
  return sum / 5;
}
```

2. **Add Capacitor:** 100nF between DHT11 VCC and GND

3. **Check Pull-Up:** Ensure 10kΩ resistor is connected

4. **Use DHT22:** More accurate than DHT11

---

## Command Issues

### ❌ Commands Don't Work / No Response

**Diagnostic Steps:**

**Step 1:** Check Connection

- Status should show "Connected" in app
- Try sending `STATUS` command

**Step 2:** Verify Command Format

```
Correct:   LED1:ON
Wrong:     led1:on (works, case insensitive)
Wrong:     LED1 ON (needs colon)
Wrong:     LED1:ON  (extra space at end is OK)
```

**Step 3:** Check Newline Character

- Most apps add `\n` automatically when you press Send
- If not, enable "Append newline" in app settings

**Step 4:** Monitor Serial Output

```
Open Serial Monitor (115200 baud)
Watch for: "Received command: LED1:ON"
If not appearing: Bluetooth connection issue
```

---

### ❌ Some Commands Work, Others Don't

**Solution:** Check Spelling

- Commands are case-insensitive but spelling matters
- `LED1:ON` ✓
- `LED1:0N` ✗ (zero instead of O)

**Solution:** Check Code

- Review `processCommand()` function for typos
- Ensure all commands are implemented

---

### ❌ LED Commands Work but Sensor Commands Don't

**Possible Issues:**

1. **Sensor Not Working**
   - Run sensor test from `component_tests.ino`
   - Check Serial Monitor for "Failed to read" errors

2. **JSON Parsing Issue (in custom app)**
   - Test with Serial Bluetooth Terminal first
   - Verify JSON format: `SENSOR:{"temp":25.5,"humidity":60.0,"unit":"C"}`

---

## General Tips

### 🔍 Debugging Techniques

**1. Use Serial Monitor**

```cpp
Serial.print("Debug: variable = ");
Serial.println(variable);
```

**2. Add More Log Messages**

```cpp
Serial.println("Reached this point");
```

**3. Test Components Individually**

- Use `component_tests.ino` to isolate problems

**4. Check Power LED**

- ESP32 should have power indicator LED lit

---

### 🛠️ Hardware Testing Tools

**Multimeter Checks:**

1. **Continuity:** Test wire connections
2. **Voltage:** Verify 3.3V and 5V rails
3. **Resistance:** Check resistor values
   - 220Ω = Red-Red-Brown
   - 10kΩ = Brown-Black-Orange

**Visual Inspection:**

1. No loose wires
2. No short circuits (wires touching)
3. LED polarity correct
4. Components firmly inserted

---

### 📱 App-Specific Tips

**Serial Bluetooth Terminal:**

- Settings → Send: Choose "Newline" or "CR+LF"
- Settings → Receive: "Text (UTF-8)"
- Create macros for common commands

**Arduino Bluetooth Controller:**

- Configure buttons: Long-press button → Enter command
- Set "Terminator" to "Newline"

---

### ⚡ Power Supply Issues

**Symptoms:**

- Random resets
- Brownouts
- Bluetooth disconnects
- Unstable sensor readings

**Solutions:**

1. Use powered USB hub (2A output)
2. Use wall adapter (5V 2A)
3. Add 470µF capacitor near ESP32 VIN
4. Avoid computer USB ports (often underpowered)

---

### 🔄 When All Else Fails

**Complete Reset Procedure:**

1. **Hardware Reset:**
   - Disconnect everything
   - Start fresh with one component at a time
   - Test each addition

2. **Software Reset:**
   - Upload blank sketch:
     ```cpp
     void setup() {}
     void loop() {}
     ```
   - Then upload main project again

3. **Factory Reset ESP32:**
   - Tools → Erase Flash → "All Flash Contents"
   - Upload code again

4. **Phone Reset:**
   - Unpair ESP32
   - Clear Bluetooth cache
   - Restart phone
   - Pair again

---

## Still Having Issues?

### Diagnostic Checklist

Print this checklist and mark each item:

**Hardware:**

- [ ] All wires connected firmly
- [ ] LED polarity correct (long leg = +)
- [ ] Resistors correct values (220Ω, 10kΩ)
- [ ] DHT11 has pull-up resistor
- [ ] Power supply adequate (>500mA)
- [ ] No short circuits

**Software:**

- [ ] ESP32 board support installed
- [ ] Correct board selected (ESP32 Dev Module)
- [ ] DHT library installed
- [ ] Code uploaded successfully
- [ ] Serial Monitor shows "Bluetooth started"
- [ ] Correct COM port selected

**Bluetooth:**

- [ ] ESP32 paired in Android settings
- [ ] Bluetooth app has permissions
- [ ] Using Bluetooth Classic app (not BLE)
- [ ] Only one app connected at a time

**Testing:**

- [ ] Tested with `component_tests.ino`
- [ ] Each component works individually
- [ ] Serial Monitor shows received commands
- [ ] Status command returns data

---

## Common Error Messages Explained

| Error                            | Meaning                   | Solution                             |
| -------------------------------- | ------------------------- | ------------------------------------ |
| `Bluetooth is not enabled!`      | Bluetooth not compiled in | Select correct partition scheme      |
| `Failed to read from DHT sensor` | DHT11 wiring/sensor issue | Check wiring, add pull-up resistor   |
| `ERROR: Unknown command`         | Command spelling wrong    | Check command format                 |
| `Connection lost`                | Bluetooth disconnected    | Check power supply, reduce distance  |
| `Sketch uses too much memory`    | Not enough RAM            | Reduce string usage or use F() macro |

---

## Success Stories / Validation

**Your project is working correctly if:**

- ✅ Serial Monitor shows "Bluetooth device started"
- ✅ Phone can pair with "ESP32_BT_Controller"
- ✅ Bluetooth app connects successfully
- ✅ `LED1:ON` command lights up LED 1
- ✅ `GET:SENSOR` returns temperature and humidity
- ✅ `AUTO:ON` sends data every 5 seconds
- ✅ `STATUS` command returns all information

**Congratulations! Your project is working!** 🎉

---

**Need More Help?**

- Review [README.md](README.md) for full documentation
- Check [WIRING_GUIDE.md](WIRING_GUIDE.md) for circuit details
- Test components with [component_tests.ino](component_tests.ino)
