# ESP32 Capacitive Touch Sensing - Core Concepts

## 📖 Table of Contents

1. [Introduction to Capacitive Sensing](#introduction-to-capacitive-sensing)
2. [ESP32 Touch Sensor Hardware](#esp32-touch-sensor-hardware)
3. [How Touch Detection Works](#how-touch-detection-works)
4. [Calibration Theory](#calibration-theory)
5. [Threshold Selection](#threshold-selection)
6. [Proximity Detection](#proximity-detection)
7. [Environmental Factors](#environmental-factors)
8. [Touch Pad Design](#touch-pad-design)
9. [Signal Processing](#signal-processing)
10. [Best Practices](#best-practices)

---

## 🔬 Introduction to Capacitive Sensing

### What is Capacitance?

**Capacitance** is the ability to store electrical charge. A capacitor has two conductive plates separated by an insulator (dielectric):

```
  ┌────────┐  +Q
  │ Plate  │
  └────────┘
    ║║║║║║
    Dielectric (air, glass, etc.)
    ║║║║║║
  ┌────────┐  -Q
  │ Plate  │
  └────────┘

Capacitance (C) = ε × A / d

Where:
  ε = Dielectric constant
  A = Plate area
  d = Distance between plates
```

**Key Insight:** Capacitance increases when:

- Plates are **larger** (↑ A)
- Plates are **closer** (↓ d)
- Dielectric constant is **higher** (↑ ε)

---

### Capacitive Touch Principle

Human body acts as a conductor and ground reference:

```
Before Touch:
┌────────────┐
│ Touch Pad  │───────┐
└────────────┘       │
     ║║║              │ Small capacitance
   (Air gap)         │ to ground
     ║║║              │
   Ground ───────────┘

After Touch:
┌────────────┐
│ Touch Pad  │────┐
└────────────┘    │
  Touching        │ Increased capacitance
┌────────────┐    │ (finger is conductive)
│   Finger   │────┤
│  (Human)   │    │
└────────────┘    │
      Connected to ground via body
```

**When you touch:**

1. Your finger acts as a capacitor plate
2. Capacitance between pad and ground increases
3. ESP32 measures this change
4. Touch is detected!

---

## 🔌 ESP32 Touch Sensor Hardware

### Touch-Capable GPIO Pins

ESP32 has **10 capacitive touch sensors** built-in:

| Touch Channel | GPIO Pin | Notes                      |
| ------------- | -------- | -------------------------- |
| **T0**        | GPIO 4   | Available                  |
| **T1**        | GPIO 0   | Boot button (avoid)        |
| **T2**        | GPIO 2   | Available (used in Lab-17) |
| **T3**        | GPIO 15  | Available (used in Lab-17) |
| **T4**        | GPIO 13  | Available                  |
| **T5**        | GPIO 12  | Available                  |
| **T6**        | GPIO 14  | Available                  |
| **T7**        | GPIO 27  | Available                  |
| **T8**        | GPIO 33  | Available                  |
| **T9**        | GPIO 32  | Available                  |

**Pin Selection Tips:**

- ✅ Use T0, T2, T3 (GPIO 4, 2, 15) - Good, stable
- ⚠️ Avoid T1 (GPIO 0) - Boot button pin
- ✅ Use T4-T9 for additional sensors

---

### Touch Sensor Circuit

Inside ESP32:

```
┌─────────────────────────────────────────────┐
│              ESP32 Chip                     │
│                                             │
│  GPIO Pin ──┬── Ultra-low Power ───┐      │
│             │   Oscillator         │      │
│             │                       │      │
│             └── Charge/Discharge ───┤      │
│                 Circuit             │      │
│                                     │      │
│                 Counter ────────────┤      │
│                                     │      │
│                 ADC ────────────────┘      │
│                  │                          │
│                  ▼                          │
│            Touch Value                      │
│           (0-65535)                         │
└─────────────────────────────────────────────┘
```

**How It Works:**

1. **Oscillator generates** charge/discharge cycles
2. **Frequency changes** with capacitance
3. **Counter measures** charging time
4. **Higher capacitance** = Slower charging = Lower count
5. **Touch increases capacitance** = Lower value

---

### Reading Touch Values

```cpp
uint16_t value = touchRead(GPIO_PIN);

// Returns: 0-65535
// Higher value = No touch
// Lower value = Touched
```

**Example Values:**

```
No Touch:     value = 85
Approaching:  value = 75 (proximity)
Light Touch:  value = 50
Firm Touch:   value = 30
Very Firm:    value = 15
```

---

## ⚙️ How Touch Detection Works

### Measurement Cycle

```
┌─────────────────────────────────────────────┐
│         Touch Sensor Operation              │
└─────────────────────────────────────────────┘

1. Charge Phase:
   ══════════════════════════════════════════
   ESP32 → GPIO → Touch Pad → Capacitor charges

   Time to charge depends on:
   - Capacitance (C)
   - Resistance (R)
   - τ = R × C (time constant)

2. Measure Phase:
   ══════════════════════════════════════════
   Count cycles until voltage threshold

   More capacitance = More time = Lower count

3. Discharge Phase:
   ══════════════════════════════════════════
   Discharge capacitor to ground
   Prepare for next measurement

4. Return Value:
   ══════════════════════════════════════════
   Count value proportional to 1/C
   (Inverse relationship)
```

### Why Values Decrease on Touch

```
Mathematical Relationship:

Frequency (f) ∝ 1 / √(C)
Count value ∝ Frequency
Therefore: Count ∝ 1 / √(C)

When capacitance increases (touch):
  C ↑ → f ↓ → count ↓ → touchRead() value ↓

Example:
  No touch:  C = 10pF → value = 100
  Touched:   C = 30pF → value = 58
```

---

## 📊 Calibration Theory

### Why Calibration is Necessary

**Problem:** Baseline values vary due to:

- Hardware differences (PCB layout, wire length)
- Environmental conditions (temperature, humidity)
- Touch pad size and material
- Nearby conductive objects

**Solution:** Measure baseline in untouched state, calculate thresholds

---

### Calibration Process

```
Step 1: Collect Samples
═══════════════════════════════════════
┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
│ 85│ 87│ 84│ 86│ 85│ 86│ 85│ 84│ 87│ 85│
└───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘
(50 samples, 20ms apart = 1 second)

Step 2: Calculate Statistics
═══════════════════════════════════════
Mean (Baseline) = Σ samples / n = 85.4 ≈ 85
Min = 84
Max = 87
Range = 3
Standard Deviation = 1.2

Step 3: Set Thresholds
═══════════════════════════════════════
Touch Threshold = Baseline × 70% = 59.5 ≈ 59
Hover Threshold = Baseline × 85% = 72.3 ≈ 72

Step 4: Validate
═══════════════════════════════════════
Check that:
- Range is small (< 10% of baseline)
- No outliers
- Stable over time
```

---

### Adaptive Calibration

Some implementations continuously adjust baseline:

```cpp
// Exponential moving average
float alpha = 0.01;  // Smoothing factor

void updateBaseline(TouchSensor* sensor) {
  if (!sensor->isTouched) {
    // Only update when not touched
    sensor->baseline = alpha * sensor->currentValue +
                       (1 - alpha) * sensor->baseline;
  }
}
```

**Advantages:**

- Adapts to temperature changes
- Compensates for slow drift

**Disadvantages:**

- Can drift if touched for long time
- More complex

---

## 🎯 Threshold Selection

### Types of Thresholds

```
Value Scale:
═══════════════════════════════════════
100 ◄─── Baseline (untouched)
 95 │
 90 │
 85 ├─── Hover Threshold (proximity)
 80 │    ▲
 75 │    │ Proximity Zone
 70 ├─── Touch Threshold (contact)
 65 │    ▲
 60 │    │ Touch Zone
 55 │    │
 50 │    │
  0 ◄─── Maximum touch
```

---

### Touch Threshold Calculation

**Formula:**

```
Touch Threshold = Baseline × (Percentage / 100)

Default: 70%
  Baseline = 100 → Threshold = 70
  Baseline = 85  → Threshold = 59
```

**Choosing Percentage:**

| Percentage | Sensitivity    | Use Case                   |
| ---------- | -------------- | -------------------------- |
| 60%        | Very Sensitive | Large pads, hover emphasis |
| 70%        | Normal         | **Default, good balance**  |
| 80%        | Less Sensitive | Noisy environments         |
| 90%        | Very Low       | Strong touch only          |

---

### Hysteresis

Prevent oscillation near threshold:

```cpp
// Without Hysteresis:
if (value < threshold) {
  // Touched!
}
// Problem: Value = 69, Threshold = 70
// Tiny movement causes rapid on/off

// With Hysteresis:
#define HYSTERESIS 5

if (!isTouched && value < (threshold - HYSTERESIS)) {
  isTouched = true;  // Touch detected
}
if (isTouched && value > (threshold + HYSTERESIS)) {
  isTouched = false;  // Release detected
}

// Now stable: Must cross 65 to touch, 75 to release
```

---

## 🌫️ Proximity Detection

### How Hover Detection Works

Your finger affects electrical field even without contact:

```
Distance vs. Capacitance:
═══════════════════════════════════════

Capacitance
    ▲
  High│     ╱── Touch (contact)
      │    ╱
      │   ╱
  Med │  ╱────── Hover (1-2cm)
      │ ╱
      │╱
  Low └──────────── Far (>5cm)
        Distance →

Formula: C ∝ 1/d (inverse relationship)
```

### Effective Hover Range

**Factors Affecting Range:**

1. **Pad Size:**

   ```
   Small (1cm²):    0.5-1cm range
   Medium (4cm²):   1-2cm range
   Large (10cm²):   2-3cm range
   ```

2. **Finger Size:**
   - Larger fingers = More capacitance = Longer range

3. **Dielectric:**
   - Air: Normal range
   - Glass/plastic over pad: Reduces range
   - Moisture: Increases range (but less stable)

---

### Multi-Level Detection

```cpp
// Three detection levels
if (value < TOUCH_THRESHOLD) {
  // Definite touch
  state = TOUCHED;
} else if (value < NEAR_THRESHOLD) {
  // Very close proximity
  state = NEAR;
} else if (value < HOVER_THRESHOLD) {
  // Distant proximity
  state = HOVER;
} else {
  // Not detected
  state = IDLE;
}
```

**Example Implementation:**

```cpp
#define TOUCH_PERCENT  70  // Touch
#define NEAR_PERCENT   80  // Very close
#define HOVER_PERCENT  85  // Hovering

// Creates three zones for nuanced interaction
```

---

## 🌍 Environmental Factors

### Temperature Effects

**Impact:**

- Dielectric constant changes with temperature
- Conductivity changes
- Baseline drifts

```
Temperature vs. Reading:
═══════════════════════════════════════
Value
  95│     ╱╲
    │    ╱  ╲
  90│   ╱    ╲
    │  ╱      ╲
  85│ ╱        ╲        Cold → Warm → Hot
    └──────────────→
       10°C  25°C  40°C

Typical drift: ±5% over 30°C range
```

**Mitigation:**

- Regular recalibration
- Adaptive baseline
- Temperature compensation algorithms

---

### Humidity Effects

**Impact:**

- Water vapor increases capacitance
- Surface moisture creates conductive paths
- False triggers in high humidity

```
Humidity vs. Sensitivity:
═══════════════════════════════════════
  High │ ████████ More sensitive
       │ ██████   (water increases C)
  Low  │ ███      Less sensitive
       └─────────────────→
         0%   50%   100% RH

Typical change: 10-20% from dry to humid
```

**Solutions:**

- Conformal coating on pads
- Sealed enclosures
- Hydrophobic materials
- Adjust thresholds for environment

---

### Interference Sources

**Electromagnetic Interference (EMI):**

| Source             | Frequency | Impact         |
| ------------------ | --------- | -------------- |
| Power supplies     | 50/60 Hz  | Baseline noise |
| WiFi/Bluetooth     | 2.4 GHz   | Minimal        |
| Motors             | Variable  | Spikes         |
| Fluorescent lights | 20-50 kHz | Moderate noise |

**Mitigation:**

1. **Shielding:** Ground plane under touch pads
2. **Filtering:** Capacitor to ground (10-100pF)
3. **Software:** Averaging, median filtering
4. **Layout:** Keep touch traces away from noisy signals

---

## 🎨 Touch Pad Design

### Size Considerations

```
┌─────────────────────────────────────┐
│        Touch Pad Sizing             │
└─────────────────────────────────────┘

Small (5-10mm):
  ✓ Precise touch
  ✓ High density
  ✗ Less sensitive
  ✗ Limited hover

Medium (15-25mm):
  ✓ Good balance
  ✓ Reliable detection
  ✓ Moderate hover
  ✓ Easy to touch

Large (30-50mm):
  ✓ Very sensitive
  ✓ Extended hover
  ✓ Easy target
  ✗ Takes more space
  ✗ Accidental touches
```

**Recommendation:** 20mm × 20mm for most applications

---

### Shape Options

```
┌─────────┐  Circular:     Best for single-point touch
│    ●    │  ✓ Even field distribution
└─────────┘  ✓ No corners to snag

┌─────────┐  Square:       Space-efficient
│         │  ✓ Easy to array
└─────────┘  ✓ Simple to fabricate

┌─────────┐  Rectangle:    Directional sensing
│         │  ✓ Slider elements
│         │  ✓ Swipe gestures
└─────────┘

    ╱╲      Triangle:      Artistic designs
   ╱  ╲     ✓ Custom shapes
  ╱────╲    ⚠ Complex field
```

---

### Layer Stack-Up

**Direct Touch:**

```
   Finger
     │
     ▼
┌─────────────┐
│ Copper Pad  │◄── Conductive layer (trace/foil)
├─────────────┤
│   PCB/FR4   │◄── Substrate
└─────────────┘
```

**Protected Touch (Through Overlay):**

```
   Finger
     │
     ▼
┌─────────────┐
│  Overlay    │◄── Plastic/glass (0.5-3mm)
├─────────────┤
│ Copper Pad  │◄── Conductive layer
├─────────────┤
│   PCB/FR4   │◄── Substrate
├─────────────┤
│ Ground Plane│◄── Shield (optional)
└─────────────┘

Max overlay thickness: 3-5mm (depends on pad size)
```

---

### Guard Ring

Improves noise immunity:

```
┌─────────────────────────────────┐
│   Ground Guard Ring             │
│  ┌───────────────────────────┐  │
│  │                           │  │
│  │    Touch Pad (Signal)     │  │◄─ To GPIO
│  │                           │  │
│  └───────────────────────────┘  │
│                                 │
└─────────────────────────────────┘
           │
          GND

Benefits:
✓ Shields from EMI
✓ Defines sensing area
✓ Reduces crosstalk
✓ Improves stability
```

---

## 📈 Signal Processing

### Raw Signal Characteristics

```
Typical Raw Touch Data:
═══════════════════════════════════════
Value
 100│     Baseline
    │  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
  80│  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
    │  ▒▒▒▒        ▒▒▒▒  Hover
  60│  ▒▒▒▒        ▒▒▒▒
    │  ░░░░░░░░░░░░░░░░  Touch
  40│  ░░░░░░░░░░░░░░░░
    └────────────────────→ Time

Problems:
  ⚠ Noise (±2-5 counts)
  ⚠ Drift over time
  ⚠ EMI spikes
```

---

### Filtering Techniques

**1. Moving Average:**

```cpp
#define FILTER_SIZE 4
uint16_t history[FILTER_SIZE] = {0};
uint8_t index = 0;

uint16_t filteredRead(uint8_t pin) {
  history[index] = touchRead(pin);
  index = (index + 1) % FILTER_SIZE;

  uint32_t sum = 0;
  for (int i = 0; i < FILTER_SIZE; i++) {
    sum += history[i];
  }
  return sum / FILTER_SIZE;
}

// Effect: Smooths noise, slight delay
```

**2. Exponential Moving Average (EMA):**

```cpp
float ema = 0;
const float alpha = 0.1;  // 0-1, lower = more smoothing

uint16_t filteredRead(uint8_t pin) {
  float current = touchRead(pin);
  ema = alpha * current + (1 - alpha) * ema;
  return (uint16_t)ema;
}

// Effect: Smooth with less memory, adapts to changes
```

**3. Median Filter:**

```cpp
#define SAMPLES 5
uint16_t buffer[SAMPLES];

uint16_t medianRead(uint8_t pin) {
  for (int i = 0; i < SAMPLES; i++) {
    buffer[i] = touchRead(pin);
    delay(1);
  }

  // Sort and return middle value
  sort(buffer, SAMPLES);
  return buffer[SAMPLES / 2];
}

// Effect: Removes outliers and spikes
```

---

### Debouncing

Prevent false transitions:

```cpp
// State machine debouncing
unsigned long lastChangeTime = 0;
const unsigned long DEBOUNCE_MS = 50;

bool currentRawState = (value < threshold);

if (currentRawState != lastRawState) {
  lastChangeTime = millis();
}

if ((millis() - lastChangeTime) > DEBOUNCE_MS) {
  // State has been stable for DEBOUNCE_MS
  confirmedState = currentRawState;
}

lastRawState = currentRawState;
```

**Benefit:** Eliminates jitter at threshold crossings

---

## ✅ Best Practices

### Hardware Design

1. **Pad Sizing:**
   - Minimum: 10mm × 10mm
   - Recommended: 20mm × 20mm
   - Maximum practical: 50mm × 50mm

2. **Trace Routing:**
   - Keep short (< 15cm)
   - Avoid parallel runs with power/signals
   - Use ground pour around traces

3. **Wire Selection:**
   - Shielded wire for long runs
   - Connect shield to ground at ESP32 end only

4. **Grounding:**
   - Solid ground reference
   - Ground plane under sensors (if space allows)

---

### Software Configuration

1. **Calibration:**
   - Always calibrate on startup
   - Recalibrate on command
   - Consider periodic auto-calibration

2. **Thresholds:**
   - Start with 70% for touch
   - Start with 85% for hover
   - Tune based on testing

3. **Filtering:**
   - Use moving average (4-8 samples)
   - Add debouncing (50ms typical)
   - Consider median filter for noisy environments

4. **Monitoring:**
   - Log baseline values
   - Track drift over time
   - Alert on abnormal readings

---

### Testing Protocol

```
1. Initial Calibration Test:
   ═══════════════════════════════════
   - Calibrate 3 times
   - Record baselines
   - Verify consistent (±5%)

2. Touch Repeatability:
   ═══════════════════════════════════
   - Touch 10 times
   - Confirm detection each time
   - Check for false negatives

3. Non-Touch Stability:
   ═══════════════════════════════════
   - Don't touch for 1 minute
   - Verify no false positives
   - Check baseline stability

4. Hover Testing:
   ═══════════════════════════════════
   - Approach slowly
   - Note detection distance
   - Verify smooth transition

5. Environmental:
   ═══════════════════════════════════
   - Test in different locations
   - Check near fluorescent lights
   - Verify near WiFi router

6. Duration Test:
   ═══════════════════════════════════
   - Run for 1 hour
   - Monitor for drift
   - Check memory usage
```

---

## 🔬 Advanced Topics

### Multi-Touch Support

```cpp
// Detect simultaneous touches
bool chord[3] = {false};

for (int i = 0; i < 3; i++) {
  chord[i] = sensors[i].isTouched;
}

if (chord[0] && chord[1]) {
  // Sensors 1+2 touched together
  handleChord12();
}
```

---

### Gesture Recognition

```cpp
// Swipe detection
uint8_t touchSequence[3] = {0};
unsigned long touchTimes[3] = {0};

// When touch detected:
touchSequence[sequenceIndex] = sensorNumber;
touchTimes[sequenceIndex] = millis();

// Check for swipe:
if (touchSequence[0] == 0 &&
    touchSequence[1] == 1 &&
    touchSequence[2] == 2) {
  unsigned long duration = touchTimes[2] - touchTimes[0];
  if (duration < 500) {
    // Swipe detected!
  }
}
```

---

### Waterproofing

For wet environments:

1. **Conformal Coating:**
   - Apply to PCB and pads
   - Maintains capacitive coupling
   - Prevents corrosion

2. **Algorithm Adjustments:**

   ```cpp
   // Detect persistent touch (water)
   if (touchDuration > 60000) {  // 1 minute
     // Probably water, not finger
     recalibrate();
   }
   ```

3. **Material Selection:**
   - Hydrophobic coatings
   - Sealed enclosures
   - Drainage holes

---

## 📚 Summary

### Key Principles

1. **Capacitive sensing** measures changes in electrical field
2. **Lower values** indicate closer proximity/touch
3. **Calibration** is essential for reliable detection
4. **Thresholds** must be tuned for each application
5. **Environmental factors** significantly affect performance
6. **Filtering** and **debouncing** improve stability
7. **Pad design** directly impacts sensitivity and range

### Decision Guide

| Requirement            | Solution                            |
| ---------------------- | ----------------------------------- |
| High sensitivity       | Large pad, 80% threshold            |
| Dust/moisture immunity | Small pad, 60% threshold, sealed    |
| Long hover range       | 30mm+ pad, guard ring               |
| Low false triggers     | Strong filtering, 65% threshold     |
| Fast response          | Minimal filtering, high sample rate |
| Multiple sensors       | Independent calibration each        |

---

**Ready to implement?** Return to [README.md](README.md) for practical examples and [CALIBRATION_GUIDE.md](CALIBRATION_GUIDE.md) for advanced calibration techniques!
