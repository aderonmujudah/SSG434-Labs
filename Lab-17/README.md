# Lab-17: ESP32 Capacitive Touch Sensors

## 🎯 Overview

This lab explores the ESP32's built-in capacitive touch sensing capabilities. Learn how to read touch values, calibrate baseline thresholds, detect touch duration, implement proximity detection, and provide LED feedback based on interaction patterns.

### Learning Objectives

- ✅ Understand capacitive touch sensing principles
- ✅ Read and interpret touch sensor values
- ✅ Calibrate baseline values for untouched state
- ✅ Implement touch detection with threshold values
- ✅ Create duration-based LED patterns (quick, medium, long)
- ✅ Detect proximity/hovering without physical contact
- ✅ Adjust sensitivity dynamically via code
- ✅ Handle multiple touch sensors simultaneously

---

## 🔧 Hardware Requirements

### Components

| Component               | Quantity | Notes                                    |
| ----------------------- | -------- | ---------------------------------------- |
| ESP32 Development Board | 1        | Must have touch-capable GPIO pins        |
| LEDs                    | 3        | Standard 5mm LEDs                        |
| Resistors (220Ω - 330Ω) | 3        | For LED current limiting                 |
| Jumper Wires            | Several  | Male-to-male and male-to-female          |
| Touch Pads/Wires        | 3        | Copper tape, aluminum foil, or bare wire |
| Breadboard              | 1        | For connections                          |

### Optional Enhancement Components

- Copper tape or PCB pads for professional touch surfaces
- Aluminum foil for larger touch areas
- Coin cell batteries for standalone operation

---

## 📍 Pin Configuration

### Touch Sensors

| Sensor   | GPIO Pin | Touch Channel | Purpose               |
| -------- | -------- | ------------- | --------------------- |
| Sensor 1 | GPIO 4   | T0            | Primary touch input   |
| Sensor 2 | GPIO 2   | T2            | Secondary touch input |
| Sensor 3 | GPIO 15  | T3            | Tertiary touch input  |

### LED Outputs

| LED   | GPIO Pin | Description           |
| ----- | -------- | --------------------- |
| LED 1 | GPIO 13  | Feedback for Sensor 1 |
| LED 2 | GPIO 12  | Feedback for Sensor 2 |
| LED 3 | GPIO 14  | Feedback for Sensor 3 |

**Note:** Same LED pins as Lab-16 for consistency.

---

## 🔌 Wiring Diagram

### Touch Sensor Connections

```
Touch Pad 1 ──────── GPIO 4
Touch Pad 2 ──────── GPIO 2
Touch Pad 3 ──────── GPIO 15

(Touch pads can be: copper tape, aluminum foil,
 bare wire, or any conductive material)
```

### LED Connections

```
ESP32                  LEDs
GPIO 13 ────[220Ω]────LED1────GND
GPIO 12 ────[220Ω]────LED2────GND
GPIO 14 ────[220Ω]────LED3────GND
```

### Creating Touch Pads

**Option 1 - Copper Tape:**

```
┌─────────────┐
│  Copper     │──── Jumper wire to GPIO
│   Tape      │
│  (Touch     │
│   Surface)  │
└─────────────┘
```

**Option 2 - Aluminum Foil:**

```
Aluminum foil (2cm x 2cm)
    ↓
Connect with jumper wire to GPIO
Tape down edges (leave center exposed)
```

**Option 3 - Simple Wire:**

```
Bare copper wire (5-10cm)
Strip 2cm of insulation at end
Connect to GPIO
Touch exposed copper part
```

---

## 💻 Software Setup

### Required Libraries

**All libraries are built into ESP32 core - no installation needed!**

- Arduino.h (standard)
- Touch sensor functions (built-in)

### Board Setup

1. **Install ESP32 Support:**
   - File → Preferences
   - Add URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Tools → Board → Boards Manager
   - Search "esp32" and install

2. **Select Board:**
   - Tools → Board → ESP32 Arduino → **ESP32 Dev Module**
   - Tools → Upload Speed → **115200**
   - Tools → Port → [Your ESP32 COM Port]

---

## 🚀 Quick Start

### 1. Upload Firmware

1. Open `ESP32_Touch_Sensor.ino` in Arduino IDE
2. Connect ESP32 via USB
3. Click **Upload**
4. Wait for "Done uploading"

### 2. Calibration

**IMPORTANT:** Do NOT touch sensors during calibration!

Open Serial Monitor (115200 baud) and observe:

```
╔════════════════════════════════════════╗
║  ESP32 Touch Sensor Demo              ║
║  Lab-17: Capacitive Touch Detection   ║
╚════════════════════════════════════════╝

Touch Sensor Configuration:
  - Sensor 1: GPIO 4 (T0) → LED GPIO 13
  - Sensor 2: GPIO 2 (T2) → LED GPIO 12
  - Sensor 3: GPIO 15 (T3) → LED GPIO 14

═══════════════════════════════════════
Starting Calibration...
═══════════════════════════════════════
⚠️  DO NOT TOUCH sensors during calibration!

Calibrating Sensor 1... .......... Done!
    Baseline: 85 (Range: 83-87)
Calibrating Sensor 2... .......... Done!
    Baseline: 92 (Range: 90-94)
Calibrating Sensor 3... .......... Done!
    Baseline: 78 (Range: 76-80)

✓ Calibration Complete!
```

### 3. Test Touch Detection

Touch each sensor and observe:

**Quick Tap (<1 second):**

```
[TOUCH START] Sensor 1 touched (value: 45)
[TOUCH END] Sensor 1 released after 523 ms
  → Quick tap detected
```

LED blinks once.

**Medium Hold (1-3 seconds):**

```
[TOUCH START] Sensor 2 touched (value: 38)
[TOUCH END] Sensor 2 released after 2145 ms
  → Medium hold detected
```

LED blinks twice.

**Long Hold (3-5 seconds):**

```
[TOUCH START] Sensor 3 touched (value: 41)
[TOUCH END] Sensor 3 released after 4230 ms
  → Long hold detected
```

LED blinks three times.

**Very Long Hold (>5 seconds):**

```
[TOUCH START] Sensor 1 touched (value: 44)
[TOUCH END] Sensor 1 released after 7890 ms
  → Very long hold detected
```

LED flashes rapidly (10 times).

### 4. Test Proximity Detection

Hover your finger 1-2cm above the sensor without touching:

```
[PROXIMITY] Sensor 1 - Hovering detected (value: 72)
```

LED dims to low brightness.

```
[PROXIMITY] Sensor 1 - Hover ended
```

LED turns off.

---

## 🎮 Serial Commands

| Command        | Function                                 |
| -------------- | ---------------------------------------- |
| **r**          | Recalibrate all sensors                  |
| **m**          | Toggle real-time value monitoring ON/OFF |
| **s**          | Show current values and states           |
| **h** or **?** | Display help menu                        |

### Example: Show Current Values

Type `s` in Serial Monitor:

```
═══════════════════════════════════════
Current Touch Values:
═══════════════════════════════════════
Sensor 1:
  Current:  83
  Baseline: 85
  Touch <:  59
  Hover <:  72
  State:    IDLE

Sensor 2:
  Current:  45
  Baseline: 92
  Touch <:  64
  Hover <:  78
  State:    TOUCHED
...
```

---

## 📊 Touch Detection System

### How It Works

```
Touch Value Scale (Example)
═══════════════════════════════════════

  0 ◄────────────── Strong touch
 20 │
 40 │  ▼ Touch detected
 50 ├──────────┐
 60 │          │ Touch Zone
 70 ├──────────┘
 75 │  ▼ Hover detected
 80 ├──────────┐
 85 │          │ Proximity Zone
 90 ├──────────┘
 95 │
100 │  ▲ Baseline (no touch)
```

**Key Points:**

- **Higher value** = No touch/far away
- **Lower value** = Touched/close
- Touch detection is **capacitive** (measures change in electrical field)

### Threshold System

```cpp
// Default Configuration
#define TOUCH_THRESHOLD_PERCENT  70    // Touch at 70% of baseline
#define HOVER_THRESHOLD_PERCENT  85    // Hover at 85% of baseline

Example with baseline = 100:
- Touch threshold = 70 (touch when value < 70)
- Hover threshold = 85 (hover when 70 ≤ value < 85)
- Idle when value ≥ 85
```

---

## ⏱️ Duration-Based Patterns

### Touch Duration Categories

| Duration | Category    | LED Pattern      | Description        |
| -------- | ----------- | ---------------- | ------------------ |
| < 1s     | Quick Tap   | 1 blink          | Single 300ms blink |
| 1-3s     | Medium Hold | 2 blinks         | Two 200ms blinks   |
| 3-5s     | Long Hold   | 3 blinks         | Three 200ms blinks |
| > 5s     | Very Long   | 10 rapid flashes | Ten 50ms flashes   |

### Pattern Visualization

```
Quick Tap:
LED: ████▌         ▐████

Medium Hold:
LED: ██▌ ▐██   ██▌ ▐██

Long Hold:
LED: ██▌ ▐██   ██▌ ▐██   ██▌ ▐██

Very Long:
LED: █▌█▌█▌█▌█▌█▌█▌█▌█▌█▌
```

---

## 🔍 Proximity Detection

### How Hover Detection Works

When your finger approaches (but doesn't touch) the sensor:

1. **Capacitance changes slightly**
2. **Touch value decreases** (but not below touch threshold)
3. **Hover state activates**
4. **LED dims** to indicate proximity

**Effective Range:** Typically 0.5-3cm depending on:

- Sensor size
- Finger size
- Environmental conditions (humidity)
- Touch pad material

### Hover Sensitivity

```cpp
// In code, adjust hover threshold:
#define HOVER_THRESHOLD_PERCENT  85

// Higher % = Less sensitive (must be closer)
// Lower % = More sensitive (detects from farther)

85% = Default
90% = Requires closer proximity
80% = Detects from farther away
```

---

## 🎛️ Sensitivity Adjustment

### Manual Adjustment in Code

**Method 1: Change Threshold Percentages**

```cpp
// In ESP32_Touch_Sensor.ino

// Less sensitive (harder to trigger):
#define TOUCH_THRESHOLD_PERCENT  60    // Was 70
#define HOVER_THRESHOLD_PERCENT  80    // Was 85

// More sensitive (easier to trigger):
#define TOUCH_THRESHOLD_PERCENT  80    // Was 70
#define HOVER_THRESHOLD_PERCENT  90    // Was 85
```

**Method 2: Dynamic Adjustment**

```cpp
// Call this function to adjust sensitivity
adjustSensitivity(-5);  // Make 5% more sensitive
adjustSensitivity(+5);  // Make 5% less sensitive
```

### When to Adjust Sensitivity

**Make MORE sensitive (lower %) if:**

- Touch not reliably detected
- Must press hard to trigger
- Hover detection not working

**Make LESS sensitive (higher %) if:**

- False triggers when not touching
- Triggers from distance unintentionally
- Environmental interference

---

## 🧪 Testing Procedures

### Test 1: Baseline Stability

**Purpose:** Verify calibration is stable

**Steps:**

1. Upload firmware and complete calibration
2. Type `s` to show values
3. Wait 30 seconds
4. Type `s` again
5. Compare baseline values

**Expected:** Baseline should be within ±5 of original

---

### Test 2: Touch Detection Accuracy

**Purpose:** Verify all sensors detect touches

**Steps:**

1. Touch Sensor 1 briefly
2. Observe LED 1 blinks once
3. Repeat for Sensors 2 and 3

**Expected:**

- Immediate touch detection message
- LED turns ON during touch
- Correct pattern after release

---

### Test 3: Duration Detection

**Purpose:** Test different hold durations

**Steps:**

1. Quick tap Sensor 1 (< 1 second)
   - Should see: Single blink
2. Hold Sensor 2 for ~2 seconds
   - Should see: Double blink
3. Hold Sensor 3 for ~4 seconds
   - Should see: Triple blink
4. Hold Sensor 1 for ~7 seconds
   - Should see: Rapid flash (10x)

**Expected:** Correct pattern for each duration

---

### Test 4: Proximity Detection

**Purpose:** Test hover sensing

**Steps:**

1. Hold finger 1-2cm above Sensor 1
2. Don't touch the surface
3. Move closer/farther slowly

**Expected:**

- "Hovering detected" message
- LED dims (low brightness)
- Message when hover ends

**Troubleshooting if not working:**

- Increase sensor pad size (more sensitive)
- Lower `HOVER_THRESHOLD_PERCENT`
- Ensure hands are dry

---

### Test 5: Multi-Sensor Simultaneous

**Purpose:** Test handling multiple touches

**Steps:**

1. Touch Sensor 1 and hold
2. While holding 1, touch Sensor 2
3. Release both

**Expected:**

- Both detect independently
- Both LEDs respond
- Both show correct patterns

---

## ⚙️ Advanced Configuration

### Custom Touch Pad Sizing

**Larger Pads:**

- More sensitive
- Easier to trigger
- Better for hover detection

**Smaller Pads:**

- Less sensitive
- Requires direct touch
- Less false triggers

**Recommended Sizes:**

- Minimum: 1cm x 1cm
- Optimal: 2cm x 2cm
- Maximum: 5cm x 5cm

### Grounding Considerations

For best performance:

```
ESP32 GND ──┬── LED Common Ground
             │
             └── Touch Pad Ground Ring (optional)
```

**Optional:** Create ground ring around touch pad with copper tape for better sensitivity.

---

## 🐛 Troubleshooting

### Issue: Touch Not Detected

**Symptoms:**

- Touching sensor does nothing
- Value doesn't change

**Solutions:**

1. **Check wiring:**

   ```
   Touch pad → Jumper wire → GPIO pin
   ```

2. **Verify GPIO pin:**

   ```cpp
   // Touch-capable pins:
   GPIO 4, 0, 2, 15, 13, 12, 14, 27, 33, 32
   ```

3. **Recalibrate:**
   - Type `r` in Serial Monitor
   - Don't touch during calibration

4. **Check touch pad material:**
   - Must be conductive (metal)
   - Aluminum foil, copper tape, bare wire work well
   - Paper, plastic won't work

---

### Issue: False Triggers

**Symptoms:**

- Triggers without touching
- Constant "touched" state

**Solutions:**

1. **Reduce sensitivity:**

   ```cpp
   #define TOUCH_THRESHOLD_PERCENT  60  // Was 70
   ```

2. **Move wires away from interference:**
   - Keep touch wires away from power lines
   - Don't bundle with other wires

3. **Recalibrate in actual environment:**
   - Calibrate where you'll use it
   - Environmental factors affect readings

4. **Add debounce delay:**
   ```cpp
   #define DEBOUNCE_TIME  100  // Increase from 50
   ```

---

### Issue: Hover Not Working

**Symptoms:**

- No proximity detection
- Must physically touch

**Solutions:**

1. **Increase sensor size:**
   - Use larger touch pad (3cm x 3cm)

2. **Adjust hover threshold:**

   ```cpp
   #define HOVER_THRESHOLD_PERCENT  90  // Was 85
   ```

3. **Ensure dry hands:**
   - Moisture affects capacitance
   - Dry hands work better for hover

4. **Check value range:**
   - Type `s` to see current values
   - Hover value should be between touch and baseline

---

### Issue: Inconsistent Readings

**Symptoms:**

- Values jump around
- Unreliable detection

**Solutions:**

1. **Improve touch pad connection:**
   - Solder wire to pad (if possible)
   - Use shorter wires
   - Ensure good contact

2. **Add capacitor:**

   ```
   GPIO ──┬── Touch Pad
          │
        [100pF]  (optional filtering)
          │
         GND
   ```

3. **Shield wires:**
   - Wrap touch wire in grounded foil
   - Helps reduce interference

---

### Issue: Calibration Fails

**Symptoms:**

- Large range during calibration
- "Baseline: 0" or very low values

**Solutions:**

1. **Check physical connection:**
   - Wire must be connected
   - Check breadboard connections

2. **Don't touch during calibration:**
   - Keep hands away
   - Wait for process to complete

3. **Reset and retry:**
   - Press ESP32 reset button
   - Upload firmware again

---

## 📈 Performance Optimization

### 1. Sampling Rate

```cpp
// In loop():
delay(10);  // Default: 10ms (100 samples/sec)

// For faster response:
delay(5);   // 5ms (200 samples/sec)

// For lower power:
delay(50);  // 50ms (20 samples/sec)
```

### 2. Calibration Sample Count

```cpp
#define CALIBRATION_SAMPLES  50    // Default

// More samples = More accurate, slower:
#define CALIBRATION_SAMPLES  100

// Fewer samples = Faster, less accurate:
#define CALIBRATION_SAMPLES  20
```

### 3. Debounce Tuning

```cpp
#define DEBOUNCE_TIME  50    // Default: 50ms

// Increase if false triggers:
#define DEBOUNCE_TIME  100

// Decrease for faster response:
#define DEBOUNCE_TIME  20
```

---

## 🌟 Advanced Experiments

### Experiment 1: Touch Slider

Use multiple sensors in sequence to create a slider:

```cpp
// Detect which sensor is touched
// Interpolate position based on touch strength
// Create sliding LED animation
```

### Experiment 2: Gesture Detection

Implement swipe gestures:

```cpp
// Swipe: Touch sensor 1 → 2 → 3 in sequence
// Detect time and order
// Trigger special pattern
```

### Experiment 3: Touch-Based Menu

Create a menu system:

```cpp
// Quick tap: Next menu item
// Medium hold: Select
// Long hold: Back/Exit
```

### Experiment 4: Proximity Distance Measurement

```cpp
// Map touch value to approximate distance
float getDistance(TouchSensor* sensor) {
  int delta = sensor->baseline - sensor->currentValue;
  // Convert to approximate cm
  return map(delta, 0, 50, 50, 0) / 10.0;
}
```

### Experiment 5: Multi-Touch Combinations

```cpp
// Detect two sensors touched simultaneously
// Create chord inputs (like keyboard shortcuts)
// Example: Touch 1+2 = Special function
```

---

## 📚 Additional Resources

### Documentation

- [ESP32 Touch Sensor API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/touch_pad.html)
- [Capacitive Sensing Basics](https://learn.sparkfun.com/tutorials/capacitive-touch-sensor-hookup-guide)

### Tutorials

- ESP32 Capacitive Touch with Arduino
- Building Touch Interfaces
- Advanced Touch Sensor Projects

---

## 📋 Summary

### What You Learned

✅ Read capacitive touch sensor values  
✅ Calibrated baseline for accurate detection  
✅ Implemented touch detection with thresholds  
✅ Created duration-based LED patterns  
✅ Detected proximity without physical touch  
✅ Adjusted sensitivity dynamically  
✅ Handled multiple sensors simultaneously  
✅ Implemented debouncing and filtering

### Key Takeaways

1. **Touch sensing is capacitive** - measures electrical field changes
2. **Calibration is critical** - always calibrate in target environment
3. **Lower values = closer/touched** - inverse relationship
4. **Thresholds prevent false triggers** - tune for your use case
5. **Sensor size matters** - larger = more sensitive
6. **Debouncing prevents jitter** - essential for stable detection
7. **Environmental factors** - humidity, temperature affect readings

---

## 🎯 Next Steps

- **Lab-18:** Combine touch sensors with WiFi for remote control
- **Lab-19:** Build touch-controlled IoT devices
- **Advanced:** Create gesture-based interfaces

---

**Happy Touch Sensing! 👆**
