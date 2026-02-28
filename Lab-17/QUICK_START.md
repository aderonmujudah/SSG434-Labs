# Lab-17 Quick Start Guide

## ⚡ 10-Minute Touch Sensor Setup

Get your ESP32 touch sensors running in minutes!

---

## Step 1: Hardware Setup (5 minutes)

### Minimal Wiring

```
ESP32          Touch Pads & LEDs
═══════════════════════════════════════════
GPIO 4  ────── Touch Pad 1 (wire/foil)
GPIO 2  ────── Touch Pad 2 (wire/foil)
GPIO 15 ────── Touch Pad 3 (wire/foil)

GPIO 13 ───[220Ω]───LED1(+)───GND
GPIO 12 ───[220Ω]───LED2(+)───GND
GPIO 14 ───[220Ω]───LED3(+)───GND
```

### Quick Touch Pad Creation

**Option 1 - Aluminum Foil (Fastest):**

```
1. Cut 2cm x 2cm square of aluminum foil
2. Tape to table/breadboard
3. Strip 1cm of jumper wire
4. Place wire on foil, tape down
5. Connect other end to GPIO pin
```

**Option 2 - Bare Wire (Simplest):**

```
1. Take a jumper wire
2. Strip 5cm of one end
3. Leave exposed copper
4. Connect other end to GPIO
5. Touch the exposed part!
```

**Option 3 - Copper Tape (Best):**

```
1. Apply copper tape to surface
2. Solder or tape wire to it
3. Connect to GPIO
4. Professional appearance!
```

**✓ Quick Check:**

- 3 touch pads connected to GPIO 4, 2, 15
- 3 LEDs with resistors connected to GPIO 13, 12, 14
- All grounds connected together

---

## Step 2: Upload Firmware (3 minutes)

### Upload Steps

1. **Open Arduino IDE**

   ```
   File → Open → ESP32_Touch_Sensor.ino
   ```

2. **Select Board**

   ```
   Tools → Board → ESP32 Dev Module
   Tools → Upload Speed → 115200
   Tools → Port → [Your ESP32 Port]
   ```

3. **Upload**
   - Click **Upload** button (→)
   - Wait for "Done uploading"

---

## Step 3: Calibration (2 minutes)

### Critical Step: Don't Touch During Calibration!

1. Open Serial Monitor: **Tools → Serial Monitor**
2. Set baud rate to **115200**
3. **Keep hands away from touch pads**
4. Wait for calibration to complete

You'll see:

```
═══════════════════════════════════════
Starting Calibration...
═══════════════════════════════════════
⚠️  DO NOT TOUCH sensors during calibration!

Calibrating Sensor 1... .......... Done!
    Baseline: 85
Calibrating Sensor 2... .......... Done!
    Baseline: 92
Calibrating Sensor 3... .......... Done!
    Baseline: 78

✓ Calibration Complete!
```

**Calibration Results:**

```
┌──────────┬──────────┬────────────┬────────────┐
│ Sensor   │ Baseline │ Touch (<)  │ Hover (<)  │
├──────────┼──────────┼────────────┼────────────┤
│ Sensor 1 │       85 │         59 │         72 │
│ Sensor 2 │       92 │         64 │         78 │
│ Sensor 3 │       78 │         54 │         66 │
└──────────┴──────────┴────────────┴────────────┘
```

---

## 🧪 Quick Tests

### Test 1: Basic Touch Detection ✓

**Touch Sensor 1 briefly**

**You Should See:**

```
[TOUCH START] Sensor 1 touched (value: 45)
[TOUCH END] Sensor 1 released after 523 ms
  → Quick tap detected
```

- ✅ LED 1 turns ON immediately
- ✅ LED 1 blinks once after release
- ✅ Serial shows touch messages

### Test 2: Touch Duration Patterns ✓

**Quick Tap (<1 sec):**

- Touch and release quickly
- **Result:** LED blinks once (single flash)

**Medium Hold (2 seconds):**

- Touch and hold for ~2 seconds
- **Result:** LED blinks twice (double flash)

**Long Hold (4 seconds):**

- Touch and hold for ~4 seconds
- **Result:** LED blinks three times (triple flash)

**Very Long (7 seconds):**

- Touch and hold for ~7 seconds
- **Result:** LED flashes rapidly 10 times

### Test 3: Proximity Detection ✓

**Hover finger 1-2cm above sensor (don't touch)**

**You Should See:**

```
[PROXIMITY] Sensor 1 - Hovering detected (value: 72)
```

- ✅ LED dims (low brightness)
- ✅ Serial shows "Hovering detected"
- ✅ LED turns off when you move away

**If hover doesn't work:**

- Use larger touch pad (3cm x 3cm)
- Ensure hands are dry
- Adjust sensitivity (see Troubleshooting)

---

## 🎮 Serial Monitor Commands

| Key   | Action                             |
| ----- | ---------------------------------- |
| **r** | Recalibrate (if values drift)      |
| **m** | Toggle real-time monitoring ON/OFF |
| **s** | Show current sensor values         |
| **h** | Show help menu                     |

### Example: Real-time Monitoring

Type `m` to enable monitoring:

```
Touch Values: S1: 83 [     ] | S2: 91 [     ] | S3: 77 [     ]
```

Touch a sensor:

```
Touch Values: S1: 42 [TOUCH] | S2: 91 [     ] | S3: 77 [     ]
```

Hover over sensor:

```
Touch Values: S1: 72 [HOVER] | S2: 91 [     ] | S3: 77 [     ]
```

---

## 🎯 Understanding the Values

### What the Numbers Mean

```
Value Scale (Example for Sensor 1):
═══════════════════════════════════════
100 ◄── Baseline (no touch)
 85 │
 72 ├─── Hover Threshold (proximity)
 65 │
 59 ├─── Touch Threshold (contact)
 45 │
 20 ◄── Strong touch
  0
```

**Key Points:**

- **Higher value** = Farther away / Not touching
- **Lower value** = Closer / Touching
- **Capacitive sensing** measures electrical field changes

### Touch States

| Value Range           | State        | LED Response    |
| --------------------- | ------------ | --------------- |
| ≥ Baseline            | **IDLE**     | Off             |
| Between Hover & Touch | **HOVERING** | Dim (PWM)       |
| < Touch Threshold     | **TOUCHED**  | Full brightness |

---

## 🐛 Quick Troubleshooting

### Touch Not Detected ❌

**Check:**

- [ ] Touch pad is conductive material (metal, not plastic)
- [ ] Wire is connected firmly
- [ ] Using correct GPIO pin (4, 2, or 15)
- [ ] Completed calibration without touching

**Fix:** Type `r` to recalibrate

---

### False Triggers (Triggers Without Touch) ❌

**Fix 1 - Reduce Sensitivity:**

```cpp
// In ESP32_Touch_Sensor.ino, change:
#define TOUCH_THRESHOLD_PERCENT  60  // Was 70 (less sensitive)
```

**Fix 2 - Recalibrate:**

- Type `r` in Serial Monitor
- Keep hands completely away

**Fix 3 - Move Wires:**

- Keep touch wires away from power lines
- Don't bundle with other cables

---

### Hover Not Working ❌

**Fixes:**

1. **Increase touch pad size** - use 3cm x 3cm foil
2. **Dry your hands** - moisture affects detection
3. **Lower hover threshold:**
   ```cpp
   #define HOVER_THRESHOLD_PERCENT  90  // Was 85
   ```
4. **Check value range** - Type `s` to see if values change on approach

---

### Inconsistent/Jumpy Readings ❌

**Fixes:**

1. **Improve connection** - solder wire if possible
2. **Use shorter wires** - 10-15cm maximum
3. **Increase debounce:**
   ```cpp
   #define DEBOUNCE_TIME  100  // Was 50ms
   ```

---

### Serial Monitor Shows Nothing ❌

**Check:**

- [ ] Baud rate set to **115200**
- [ ] Correct COM port selected
- [ ] Cable supports data (not charge-only)

**Fix:** Press ESP32 reset button

---

## 🎨 LED Pattern Reference

Quick visual guide to patterns:

```
Quick Tap (<1s):
LED: ████████░░░░░░░░
     (single 300ms blink)

Medium Hold (1-3s):
LED: ████░░░░████░░░░
     (two 200ms blinks)

Long Hold (3-5s):
LED: ███░░███░░███░░
     (three 200ms blinks)

Very Long (>5s):
LED: █░█░█░█░█░█░█░█░█░█░
     (ten 50ms flashes)

Hover (proximity):
LED: ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
     (dimmed, constant)
```

---

## ⚙️ Quick Sensitivity Adjustment

### If Touch is Too Sensitive

```cpp
// Make LESS sensitive (in .ino file):
#define TOUCH_THRESHOLD_PERCENT  60  // Lower = harder to trigger
```

### If Touch is Not Sensitive Enough

```cpp
// Make MORE sensitive (in .ino file):
#define TOUCH_THRESHOLD_PERCENT  80  // Higher = easier to trigger
```

**Remember:** After changing, re-upload firmware!

---

## ✅ Success Checklist

- [ ] All 3 LEDs working
- [ ] Touch detection working on all 3 sensors
- [ ] Quick tap → single blink
- [ ] Medium hold (2s) → double blink
- [ ] Long hold (4s) → triple blink
- [ ] Very long (7s) → rapid flash
- [ ] Hover detection working (LED dims)
- [ ] Serial Monitor shows touch values
- [ ] Commands (r, m, s, h) working

**All checked? Congratulations! Your touch sensors are working perfectly! 🎉**

---

## 🚀 Next Steps

Once basic operation is verified, try:

### Experiment 1: Adjust Thresholds

```cpp
// Try different sensitivity levels
#define TOUCH_THRESHOLD_PERCENT  75
#define HOVER_THRESHOLD_PERCENT  88
```

### Experiment 2: Custom Patterns

```cpp
// Add your own pattern in playPattern() function
case PATTERN_CUSTOM:
  // Your creative LED pattern here!
  break;
```

### Experiment 3: Larger Touch Pads

- Use 5cm x 5cm aluminum foil
- Compare sensitivity with small pad
- Test hover distance improvement

### Experiment 4: Touch Slider

- Arrange 3 pads in a line
- Slide finger across them
- Detect swipe gestures!

---

## 📊 Expected Performance

**Typical Values (varies by hardware):**

- **Baseline:** 70-100 (untouched)
- **Touch:** 20-60 (touched)
- **Hover:** 65-85 (proximity)
- **Response Time:** < 20ms
- **Hover Range:** 0.5-3cm

---

## 📚 Learn More

For detailed information:

- **[README.md](README.md)** - Complete documentation
- **[TOUCH_CONCEPTS.md](TOUCH_CONCEPTS.md)** - How capacitive sensing works
- **[CALIBRATION_GUIDE.md](CALIBRATION_GUIDE.md)** - Advanced calibration techniques

---

**Need Help?** Check the full [README.md](README.md) for detailed troubleshooting!

**Ready to experiment?** See [TOUCH_CONCEPTS.md](TOUCH_CONCEPTS.md) for advanced techniques!
