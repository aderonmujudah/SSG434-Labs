# Web-Based OTA Update Guide - ESP32

Complete guide to updating ESP32 firmware through a web browser interface.

---

## 📋 Table of Contents

1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Preparing Firmware Binary](#preparing-firmware-binary)
4. [Accessing Web Interface](#accessing-web-interface)
5. [Uploading Firmware](#uploading-firmware)
6. [Monitoring Progress](#monitoring-progress)
7. [Verification](#verification)
8. [Troubleshooting](#troubleshooting)
9. [Best Practices](#best-practices)
10. [Advanced Topics](#advanced-topics)

---

## 🎯 Overview

Web-based OTA allows you to update ESP32 firmware using any web browser. No special tools required - just:

- ✅ Web browser (Chrome, Firefox, Safari, Edge)
- ✅ Network connection to ESP32
- ✅ Compiled firmware binary (.bin file)

### Advantages

| Feature             | Web OTA           | USB Upload   | Arduino OTA     |
| ------------------- | ----------------- | ------------ | --------------- |
| **No USB Cable**    | ✅                | ❌           | ✅              |
| **Visual Feedback** | ✅ Progress bar   | ❌           | ⚠️ IDE only     |
| **Remote Access**   | ✅ Over network   | ❌           | ✅ Same network |
| **User Friendly**   | ✅ Anyone can use | ⚠️ Technical | ⚠️ Technical    |
| **Mobile Support**  | ✅ Phone/tablet   | ❌           | ❌              |

---

## 📦 Prerequisites

### 1. ESP32 Setup

- ✅ ESP32 with OTA firmware already running
- ✅ Connected to WiFi network
- ✅ IP address known
- ✅ Web server responding

**Verify ESP32 is ready:**

```bash
# Test connectivity
ping 192.168.1.100

# Test web server
curl http://192.168.1.100/status
```

Expected response:

```json
{
  "version": "1.0.0",
  "uptime": "10m 45s",
  "freeHeap": 234567,
  "updateStatus": "Ready",
  "updateProgress": 0
}
```

### 2. Network Requirements

- ✅ ESP32 and computer on same network
- ✅ No firewall blocking HTTP (port 80)
- ✅ Stable WiFi connection
- ✅ Good signal strength (RSSI > -70 dBm)

### 3. Browser Requirements

**Supported Browsers:**

- ✅ Google Chrome (recommended)
- ✅ Mozilla Firefox
- ✅ Microsoft Edge
- ✅ Safari (macOS/iOS)
- ✅ Opera

**Minimum versions:**

- Chrome 60+
- Firefox 55+
- Edge 79+
- Safari 12+

---

## 🔨 Preparing Firmware Binary

### Step 1: Modify Your Code

Before creating a new firmware, make changes to test:

```cpp
// Example: Change version number
#define FIRMWARE_VERSION "2.0.0"  // Was "1.0.0"

// Example: Change LED blink rate
int blinkInterval = 500;  // Was 1000
```

### Step 2: Export Compiled Binary

#### Method A: Arduino IDE Menu

1. Open your modified sketch
2. Click **Sketch** in menu bar
3. Select **Export Compiled Binary**
   - Or press: **Ctrl+Alt+S** (Windows/Linux)
   - Or press: **Cmd+Alt+S** (macOS)
4. Wait for compilation (progress shown in output window)
5. Look for "Done compiling" message

#### Method B: Arduino IDE 2.x

1. Open sketch
2. Click the **three dots (⋮)** next to Upload button
3. Select **Export Compiled Binary**
4. Wait for compilation

#### Method C: Command Line (Advanced)

```bash
# Using arduino-cli
arduino-cli compile --fqbn esp32:esp32:esp32 --export-binaries ESP32_OTA_Firmware_v2.ino

# Binary saved to: build/esp32.esp32.esp32/
```

### Step 3: Locate Binary File

Compiled binary is saved in your sketch folder:

```
Your Sketch Folder/
├── ESP32_OTA_Firmware_v2.ino
└── ESP32_OTA_Firmware_v2.ino.esp32.bin  ← This file!
```

**File naming pattern:**

```
[SketchName].ino.esp32.bin
```

**File size:**

- Typical: 800 KB - 1.5 MB
- With libraries: Can be larger
- **Important:** Must fit in available flash partition

### Step 4: Verify Binary

Check binary file:

```bash
# Windows
dir ESP32_OTA_Firmware_v2.ino.esp32.bin

# Linux/Mac
ls -lh ESP32_OTA_Firmware_v2.ino.esp32.bin

# Should show file size (e.g., 856 KB)
```

**Warning Signs:**

- ❌ File size = 0 bytes (compilation failed)
- ❌ No .bin file created (check for errors)
- ❌ File smaller than 500 KB (incomplete)

---

## 🌐 Accessing Web Interface

### Step 1: Get ESP32 IP Address

#### Option A: From Serial Monitor

```
✅ WiFi Connected!
   IP Address: 192.168.1.100  ← Use this
```

#### Option B: From Router Admin Panel

1. Open router admin (usually 192.168.1.1)
2. Find connected devices list
3. Look for device named "ESP32-OTA"

#### Option C: Using Network Scanner

```bash
# Windows
arp -a

# Linux
arp-scan --localnet

# Look for ESP32 MAC address
```

### Step 2: Open Web Interface

1. Open web browser
2. Enter URL: `http://[ESP32_IP]`
   - Example: `http://192.168.1.100`
3. Press Enter

**Alternative URL (if mDNS works):**

```
http://esp32-ota.local
```

### Step 3: Verify Page Loads

You should see:

```
╔═══════════════════════════════════════╗
║     🚀 ESP32 OTA Update Center       ║
║  Over-The-Air Firmware Update System ║
╚═══════════════════════════════════════╝

Firmware Version: v1.0.0
Build Date: Feb 28 2026
Build Time: 10:30:00

[Device Information Card]
[Upload Section]
[LED Controls]
```

**If page doesn't load:**

- Check IP address is correct
- Ping ESP32: `ping 192.168.1.100`
- Verify ESP32 is powered and connected
- Check firewall settings

---

## 📤 Uploading Firmware

### Step 1: Navigate to Upload Section

Scroll to the "📦 Upload New Firmware" section.

**What you'll see:**

- File input field
- "Upload & Update Firmware" button
- Warning about .bin files only

### Step 2: Select Firmware File

**Method A: Click to Browse**

1. Click **Choose File** button
2. Navigate to your sketch folder
3. Select the `.bin` file
4. Click **Open**

**Method B: Drag and Drop (some browsers)**

1. Open file explorer
2. Locate your `.bin` file
3. Drag file to the file input area
4. Drop when highlighted

### Step 3: Verify File Selected

After selection, you should see:

```
[File Input: ESP32_OTA_Firmware_v2.ino.esp32.bin]
```

**Validation:**

- ✅ File name ends with `.bin`
- ✅ File size shown (if supported by browser)
- ✅ Upload button becomes active

### Step 4: Initiate Upload

1. Double-check correct file selected
2. Click **Upload & Update Firmware** button
3. Button becomes disabled (grayed out)
4. Status message appears: "Uploading firmware... Please wait"

**⚠️ Important:**

- Don't close browser window
- Don't refresh page
- Don't navigate away
- Keep WiFi connected
- Don't power off ESP32

### Step 5: JavaScript Upload Process

Behind the scenes:

```javascript
1. File is read from browser
2. FormData object created
3. XMLHttpRequest initiated
4. File sent to ESP32 in chunks
5. Progress events tracked
6. Server processes upload
7. Firmware written to flash
8. Verification performed
9. Success/error response sent
```

---

## 📊 Monitoring Progress

### Visual Indicators

#### Progress Bar

```
╔════════════════════════════════════╗
║ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓░░░░░░░░░░░░  60% ║
╚════════════════════════════════════╝
```

**Progress Stages:**

| Stage         | Progress | What's Happening            |
| ------------- | -------- | --------------------------- |
| **Uploading** | 0-99%    | File being sent to ESP32    |
| **Verifying** | 99-100%  | Checking firmware integrity |
| **Flashing**  | 100%     | Writing to flash memory     |

#### Status Messages

```
📤 0-30%:   "Uploading firmware... [MB] of [MB]"
⚡ 31-70%:  "Upload in progress..."
✅ 71-99%:  "Almost done..."
🔍 100%:    "Verifying firmware..."
✅ Complete: "Update successful! Device will reboot..."
```

#### LED Feedback

Watch the ESP32 LED:

| LED Pattern             | Meaning            |
| ----------------------- | ------------------ |
| **Rapid blink (100ms)** | Upload in progress |
| **Steady ON**           | Processing         |
| **5 quick blinks**      | Success!           |
| **3 slow blinks**       | Error              |

### Serial Monitor Output

If connected to Serial Monitor (115200 baud):

```
╔══════════════════════════════════════╗
║     OTA UPDATE STARTED (WEB)         ║
╚══════════════════════════════════════╝
Filename: ESP32_OTA_Firmware_v2.ino.esp32.bin
Progress: 0% (0/856234 bytes)
Progress: 10% (85623/856234 bytes)
Progress: 20% (171246/856234 bytes)
...
Progress: 100% (856234/856234 bytes)
✅ Update Success! Size: 856234 bytes
Rebooting in 3 seconds...
```

### Browser Console (F12)

For debugging, open browser console:

```javascript
Upload started
File size: 856234 bytes
Progress: 10%
Progress: 20%
...
XHR response: 200 OK
Update successful!
```

---

## ✅ Verification

### Step 1: Wait for Reboot

After successful upload:

```
✅ Update successful! Device will reboot in 3 seconds...
```

ESP32 will:

1. Wait 3 seconds
2. Restart automatically
3. Boot with new firmware
4. Reconnect to WiFi
5. Start web server

**Timeline:**

- T+0s: Update completes
- T+3s: Reboot initiated
- T+5s: Boot sequence starts
- T+8s: WiFi connecting
- T+12s: Web server ready
- T+15s: Page auto-reloads

### Step 2: Page Auto-Reload

Browser will attempt to reload page automatically:

```
🔄 Rebooting... Page will reload automatically
```

**If auto-reload fails:**

- Wait 5 more seconds
- Manually refresh: F5 or Ctrl+R
- Or re-enter URL

### Step 3: Check Version Number

After page reloads, verify:

**v1.0.0 → v2.0.0** ✅

Look for:

- Version banner changed
- Build date updated
- New features visible

### Step 4: Test New Features

**v2.0.0 specific checks:**

1. **LED Blink Rate** - Should be faster (500ms vs 1000ms)
2. **New Button** - "Blink Pattern" button present
3. **UI Changes** - Green version banner
4. **Serial Output** - Shows v2.0.0 on boot

### Step 5: Verify via API

Make API call to confirm:

```bash
curl http://192.168.1.100/status
```

Expected response:

```json
{
  "version": "2.0.0",    ← Changed!
  "uptime": "15s",       ← Reset (freshly booted)
  "freeHeap": 245678,
  "updateStatus": "Ready",
  "updateProgress": 0
}
```

### Step 6: Functional Testing

Test all features still work:

- ✅ LED ON control
- ✅ LED OFF control
- ✅ LED Blink Pattern (new feature)
- ✅ WiFi connectivity
- ✅ Web interface responsive
- ✅ Serial output correct

---

## ⚠️ Troubleshooting

### Upload Errors

#### Error: "Upload failed: Connection timeout"

**Caused by:**

- WiFi signal weak
- ESP32 too far from router
- Network congestion

**Solutions:**

1. Move ESP32 closer to router
2. Check WiFi RSSI in Serial Monitor
3. Reduce network traffic
4. Try again during off-peak hours

---

#### Error: "Invalid firmware file"

**Caused by:**

- Wrong file type (not .bin)
- Corrupted binary
- Wrong architecture (not ESP32)

**Solutions:**

1. Verify file ends with `.bin`
2. Re-export binary from Arduino IDE
3. Check compilation was successful
4. Ensure targeting ESP32 (not ESP8266)

---

#### Error: "Not enough space"

**Caused by:**

- Firmware too large for partition
- Flash memory full

**Solutions:**

1. Check firmware size vs partition size
2. Reduce firmware size (remove unused libraries)
3. Use different partition scheme
4. Check available space in Serial Monitor

---

#### Error: "Update failed at 50%"

**Caused by:**

- Power loss during update
- WiFi disconnected
- Memory allocation failure

**Solutions:**

1. Ensure stable power supply
2. Use quality USB cable/power adapter
3. Check WiFi doesn't disconnect
4. Power cycle ESP32 and retry

---

### Recovery Procedures

#### If ESP32 Won't Boot After Update

**Issue:** Update completed but device won't start

**Recovery Steps:**

1. **Serial Monitor Check**

   ```
   Connect USB cable
   Open Serial Monitor (115200 baud)
   Press Reset button
   Look for boot errors
   ```

2. **Re-upload via USB**

   ```
   Connect USB cable
   Upload working firmware
   Use ESP32 Flash Tool if completely bricked
   ```

3. **Factory Reset** (if supported)
   ```cpp
   // Add factory reset function
   if (digitalRead(RESET_PIN) == LOW) {
       ESP.restart();
   }
   ```

#### If Web Interface Inaccessible After Update

**Steps:**

1. Verify ESP32 booted (check LED pattern)
2. Ping IP address: `ping 192.168.1.100`
3. Check Serial Monitor for WiFi status
4. Try alternative URL: `http://esp32-ota.local`
5. Power cycle ESP32

---

## 🎯 Best Practices

### Before Updating

- [ ] **Backup current firmware** - Save working .bin file
- [ ] **Test new firmware** - Upload via USB first
- [ ] **Check version number** - Ensure it incremented
- [ ] **Verify WiFi credentials** - Match current settings
- [ ] **Read changelog** - Know what's changing
- [ ] **Check memory usage** - Ensure firmware fits
- [ ] **Stable power** - Use good power supply

### During Update

- [ ] **Stay on same page** - Don't navigate away
- [ ] **Monitor progress** - Watch progress bar
- [ ] **Check Serial output** - If possible
- [ ] **Stable connection** - Don't move ESP32
- [ ] **No power interruption** - Keep plugged in
- [ ] **Wait patiently** - Update takes 1-2 minutes

### After Update

- [ ] **Verify version** - Check updated correctly
- [ ] **Test features** - Ensure everything works
- [ ] **Check logs** - Review Serial output
- [ ] **Document changes** - Note differences
- [ ] **Save binary** - Keep for future rollback
- [ ] **Update documentation** - Record version info

---

## 🚀 Advanced Topics

### Custom Progress Indicators

Add custom visual feedback:

```javascript
// Custom progress handler
xhr.upload.addEventListener("progress", function (e) {
  const percent = (e.loaded / e.total) * 100;

  // Custom logger
  console.log(`Upload: ${percent}%`);

  // Color change based on progress
  if (percent < 30) progressBar.style.background = "red";
  else if (percent < 70) progressBar.style.background = "yellow";
  else progressBar.style.background = "green";

  // Sound notifications
  if (percent === 50) playSound("halfway.mp3");
  if (percent === 100) playSound("complete.mp3");
});
```

### Batch Updates

Update multiple ESP32 devices:

```javascript
// Update multiple devices
const devices = ["192.168.1.100", "192.168.1.101", "192.168.1.102"];

async function updateAll(firmware) {
  for (const ip of devices) {
    console.log(`Updating ${ip}...`);
    await uploadFirmware(ip, firmware);
    await sleep(5000); // Wait for reboot
  }
}
```

### Scheduled Updates

Schedule update for specific time:

```javascript
// Schedule update for 2 AM
const updateTime = new Date();
updateTime.setHours(2, 0, 0, 0);

const timeUntilUpdate = updateTime - Date.now();

setTimeout(() => {
  uploadFirmware();
}, timeUntilUpdate);
```

### Version Comparison

Prevent downgrade:

```cpp
bool isNewerVersion(String current, String new) {
    // Parse and compare semantic versions
    // Return true only if new > current
    return compareVersions(current, new) < 0;
}

// In update handler
if (!isNewerVersion(FIRMWARE_VERSION, uploadedVersion)) {
    server.send(400, "text/plain", "Downgrade not allowed");
    return;
}
```

### Update Rollback

Implement automatic rollback:

```cpp
#include "esp_ota_ops.h"

void setup() {
    // Mark current partition as valid
    esp_ota_mark_app_valid_cancel_rollback();

    // Or trigger rollback on failure
    if (updateFailed) {
        esp_ota_mark_app_invalid_rollback_and_reboot();
    }
}
```

---

## 📚 API Reference

### Endpoints

#### GET `/`

- **Description:** Main web interface
- **Returns:** HTML page
- **Status:** 200 OK

#### GET `/status`

- **Description:** Device status and version
- **Returns:** JSON

```json
{
  "version": "2.0.0",
  "uptime": "10m 30s",
  "freeHeap": 234567,
  "updateStatus": "Ready",
  "updateProgress": 0
}
```

#### POST `/update`

- **Description:** Upload firmware binary
- **Content-Type:** multipart/form-data
- **Form Field:** firmware (file)
- **Returns:** Success/error message

#### GET `/led?state=[on|off|blink]`

- **Description:** Control LED
- **Parameters:** state (string)
- **Returns:** Plain text confirmation

---

## ✅ Checklist

Before declaring update successful:

- [ ] Binary file compiled without errors
- [ ] Binary file size reasonable (< 2MB typically)
- [ ] ESP32 IP address accessible
- [ ] Web interface loads correctly
- [ ] File selected in upload form
- [ ] Upload progress showed 0% → 100%
- [ ] Status message showed success
- [ ] ESP32 rebooted automatically
- [ ] New version number displayed
- [ ] New features working
- [ ] LED blink pattern changed (if applicable)
- [ ] Serial Monitor shows new version
- [ ] API returns updated version
- [ ] No error messages in console
- [ ] Device fully functional

---

## 🎓 Summary

You now know how to:

✅ Prepare firmware binaries for OTA  
✅ Access ESP32 web interface  
✅ Upload firmware through browser  
✅ Monitor update progress  
✅ Verify successful updates  
✅ Troubleshoot common issues  
✅ Follow best practices  
✅ Implement advanced features

**Web OTA = Powerful, User-Friendly, Mobile-Compatible** 🚀

---

## 📖 Further Reading

- [ESP32 OTA Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/ota.html)
- [HTTP Update Library](https://github.com/espressif/arduino-esp32/tree/master/libraries/Update)
- [MDN Web APIs](https://developer.mozilla.org/en-US/docs/Web/API)
- [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)

---

**Master of Web-Based OTA! 🎉🌐📱**
