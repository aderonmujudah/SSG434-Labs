# ESP32 Touch Sensor Calibration Guide

## 🎯 Overview

This guide provides detailed calibration procedures for ESP32 capacitive touch sensors, ensuring reliable detection across different environments and usage scenarios.

---

## 📑 Table of Contents

1. [Why Calibration Matters](#why-calibration-matters)
2. [Calibration Methods](#calibration-methods)
3. [Initial Calibration Procedure](#initial-calibration-procedure)
4. [Advanced Calibration Techniques](#advanced-calibration-techniques)
5. [Troubleshooting Calibration Issues](#troubleshooting-calibration-issues)
6. [Adaptive Calibration](#adaptive-calibration)
7. [Multi-Environment Calibration](#multi-environment-calibration)
8. [Validation and Testing](#validation-and-testing)

---

## 🔍 Why Calibration Matters

### The Problem

Touch sensor baseline values vary significantly due to:

| Factor                  | Impact                    | Variation    |
| ----------------------- | ------------------------- | ------------ |
| **Wire Length**         | Changes capacitance       | ±10-20 units |
| **Pad Size**            | Affects baseline          | ±20-30 units |
| **Temperature**         | Dielectric changes        | ±5-10 units  |
| **Humidity**            | Moisture affects readings | ±10-15 units |
| **PCB Layout**          | Parasitic capacitance     | ±15-25 units |
| **Component Tolerance** | ESP32 variations          | ±5-10 units  |

**Without calibration:**

```
Sensor 1: Baseline = 85, Touch = 45
Sensor 2: Baseline = 120, Touch = 60 ❌ Fixed threshold won't work!
Sensor 3: Baseline = 70, Touch = 35

Fixed threshold of 60 would:
  - Miss touches on Sensor 1 (45 < 60 ✓ works)
  - Never trigger on Sensor 2 (60 = 60 ✗ boundary)
  - Always trigger on Sensor 3 (35 < 60 ✓ works)
```

**With calibration:**

```
Sensor 1: Baseline = 85  → Threshold = 59 (70%)
Sensor 2: Baseline = 120 → Threshold = 84 (70%)
Sensor 3: Baseline = 70  → Threshold = 49 (70%)

All sensors work reliably! ✓
```

---

## 🛠️ Calibration Methods

### Method 1: Startup Calibration (Default)

**When:** On power-up or reset  
**Duration:** 1-2 seconds  
**Accuracy:** High

```cpp
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("⚠️  DO NOT TOUCH sensors!");
  delay(2000);  // Give user time to read

  calibrateSensors();  // Capture baseline

  Serial.println("✓ Calibration complete");
}
```

**Advantages:**

- ✅ Simple to implement
- ✅ Known clean state
- ✅ User-controlled timing

**Disadvantages:**

- ⚠️ Requires user attention
- ⚠️ No adaptation to changes
- ⚠️ Must recalibrate on environment change

---

### Method 2: Command-Based Recalibration

**When:** On user command (serial input)  
**Duration:** 1-2 seconds  
**Accuracy:** High

```cpp
void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'r' || cmd == 'R') {
      Serial.println("Recalibrating...");
      calibrateSensors();
    }
  }
}
```

**Use Cases:**

- Environment changes (indoor → outdoor)
- Temperature drift detected
- False triggers occurring
- After long operation time

---

### Method 3: Adaptive Calibration

**When:** Continuously during operation  
**Duration:** Ongoing  
**Accuracy:** Medium-High

```cpp
void updateBaseline(TouchSensor* sensor) {
  if (!sensor->isTouched) {
    // Only update when not touched
    float alpha = 0.001;  // Very slow adaptation
    sensor->baseline = alpha * sensor->currentValue +
                       (1 - alpha) * sensor->baseline;

    // Recalculate thresholds
    sensor->touchThreshold = sensor->baseline * 0.70;
    sensor->hoverThreshold = sensor->baseline * 0.85;
  }
}
```

**Advantages:**

- ✅ Adapts to slow changes
- ✅ No user intervention
- ✅ Compensates for drift

**Disadvantages:**

- ⚠️ Can drift if touched for extended time
- ⚠️ Slower to respond to sudden changes
- ⚠️ More complex logic

---

## 📋 Initial Calibration Procedure

### Step-by-Step Process

**1. Prepare Environment**

```
✓ Remove all objects from sensors
✓ Ensure hands are away (>10cm)
✓ Stabilize power supply
✓ Wait for ESP32 to reach operating temperature (~30 seconds)
✓ Minimize nearby movement during calibration
```

**2. Collect Baseline Samples**

```cpp
void calibrateSensor(TouchSensor* sensor) {
  const int SAMPLES = 50;
  const int DELAY_MS = 20;

  uint32_t sum = 0;
  uint16_t minVal = 65535;
  uint16_t maxVal = 0;

  for (int i = 0; i < SAMPLES; i++) {
    uint16_t value = touchRead(sensor->pin);

    sum += value;
    if (value < minVal) minVal = value;
    if (value > maxVal) maxVal = value;

    delay(DELAY_MS);
  }

  // Calculate statistics
  sensor->baseline = sum / SAMPLES;
  uint16_t range = maxVal - minVal;

  // Validate quality
  if (range > sensor->baseline * 0.15) {
    Serial.println("⚠️ WARNING: Noisy calibration!");
    Serial.print("   Range: ");
    Serial.println(range);
  }
}
```

**3. Calculate Thresholds**

```cpp
// Touch threshold (70% of baseline)
sensor->touchThreshold = (sensor->baseline * TOUCH_THRESHOLD_PERCENT) / 100;

// Hover threshold (85% of baseline)
sensor->hoverThreshold = (sensor->baseline * HOVER_THRESHOLD_PERCENT) / 100;
```

**4. Validate Calibration**

```cpp
void validateCalibration(TouchSensor* sensor) {
  // Re-sample to check stability
  uint16_t recheck = touchRead(sensor->pin);
  int16_t delta = abs((int16_t)recheck - (int16_t)sensor->baseline);

  if (delta > 5) {
    Serial.println("⚠️ Baseline unstable, recalibrating...");
    calibrateSensor(sensor);
  } else {
    Serial.println("✓ Calibration validated");
  }
}
```

---

## 🎓 Advanced Calibration Techniques

### Technique 1: Outlier Rejection

Remove anomalous readings:

```cpp
void calibrateWithOutlierRejection(TouchSensor* sensor) {
  const int SAMPLES = 60;
  uint16_t values[SAMPLES];

  // Collect samples
  for (int i = 0; i < SAMPLES; i++) {
    values[i] = touchRead(sensor->pin);
    delay(20);
  }

  // Sort array
  qsort(values, SAMPLES, sizeof(uint16_t), compare);

  // Discard top and bottom 10% (outliers)
  int start = SAMPLES / 10;
  int end = SAMPLES - (SAMPLES / 10);

  // Average middle 80%
  uint32_t sum = 0;
  for (int i = start; i < end; i++) {
    sum += values[i];
  }

  sensor->baseline = sum / (end - start);
}

int compare(const void* a, const void* b) {
  return (*(uint16_t*)a - *(uint16_t*)b);
}
```

**Benefit:** More robust against transient noise

---

### Technique 2: Multi-Point Calibration

Calibrate for multiple thresholds:

```cpp
struct Thresholds {
  uint16_t light_touch;
  uint16_t medium_touch;
  uint16_t firm_touch;
  uint16_t hover_far;
  uint16_t hover_near;
};

void calibrateMultiPoint(TouchSensor* sensor, Thresholds* thresh) {
  uint16_t baseline = sensor->baseline;

  thresh->hover_far = baseline * 0.90;   // 90% - distant hover
  thresh->hover_near = baseline * 0.85;  // 85% - close hover
  thresh->light_touch = baseline * 0.70; // 70% - light touch
  thresh->medium_touch = baseline * 0.55; // 55% - medium
  thresh->firm_touch = baseline * 0.40;  // 40% - firm
}
```

---

### Technique 3: Temperature Compensation

Adjust for temperature changes:

```cpp
#include <esp_system.h>

float getTemperatureCompensation() {
  // ESP32 internal temperature sensor (approximate)
  // Note: Not highly accurate, use external sensor for precision

  float temp = temperatureRead();  // Celsius

  // Compensation factor: ~0.1% per degree from 25°C
  float compensation = 1.0 + ((temp - 25.0) * 0.001);

  return compensation;
}

void temperatureCompensatedCalibration(TouchSensor* sensor) {
  calibrateSensor(sensor);

  float tempFactor = getTemperatureCompensation();
  sensor->touchThreshold *= tempFactor;
  sensor->hoverThreshold *= tempFactor;
}
```

---

### Technique 4: Differential Calibration

Calibrate relative change instead of absolute values:

```cpp
struct DifferentialSensor {
  uint16_t baseline;
  uint16_t touchDelta;    // Change required for touch
  uint16_t hoverDelta;    // Change required for hover
};

void calibrateDifferential(DifferentialSensor* sensor, uint8_t pin) {
  // Capture baseline
  sensor->baseline = getAverageReading(pin, 50);

  // Set deltas as absolute differences
  sensor->touchDelta = sensor->baseline * 0.30;  // 30% change
  sensor->hoverDelta = sensor->baseline * 0.15;  // 15% change

  Serial.print("Touch when value < ");
  Serial.println(sensor->baseline - sensor->touchDelta);
}

bool isDifferentialTouch(DifferentialSensor* sensor, uint16_t current) {
  int16_t delta = (int16_t)sensor->baseline - (int16_t)current;
  return (delta > sensor->touchDelta);
}
```

**Advantage:** More robust to baseline drift

---

## 🐛 Troubleshooting Calibration Issues

### Issue 1: Wide Range During Calibration

**Symptoms:**

```
Calibrating Sensor 1... Done!
    Baseline: 85 (Range: 75-95)  ← Range of 20 is too high!
```

**Causes:**

- Noise from power supply
- EMI from nearby devices
- Person moving near sensors
- Unstable power

**Solutions:**

1. **Increase sample count:**

```cpp
#define CALIBRATION_SAMPLES  100  // Was 50
```

2. **Add settling time:**

```cpp
void calibrateSensor(TouchSensor* sensor) {
  // Discard first few readings
  for (int i = 0; i < 10; i++) {
    touchRead(sensor->pin);
    delay(10);
  }

  // Now calibrate
  // ... rest of calibration
}
```

3. **Use median instead of mean:**

```cpp
sensor->baseline = getMedian(samples, CALIBRATION_SAMPLES);
```

---

### Issue 2: Baseline Changes After Calibration

**Symptoms:**

```
Calibration: Baseline = 85
After 1 minute: Current = 95  ← Drifted +10!
```

**Causes:**

- Temperature stabilization
- Humidity changes
- Component warm-up

**Solutions:**

1. **Delayed calibration:**

```cpp
void setup() {
  Serial.begin(115200);

  Serial.println("Warming up ESP32...");
  delay(30000);  // Wait 30 seconds for stabilization

  calibrateSensors();
}
```

2. **Adaptive tracking:**

```cpp
void trackBaseline(TouchSensor* sensor) {
  if (!sensor->isTouched) {
    // Check for significant drift
    int16_t drift = (int16_t)sensor->currentValue -
                    (int16_t)sensor->baseline;

    if (abs(drift) > 10) {
      Serial.println("Drift detected, recalibrating...");
      calibrateSensor(sensor);
    }
  }
}
```

---

### Issue 3: Calibration Fails (Baseline = 0 or Very Low)

**Symptoms:**

```
Calibrating Sensor 1... Done!
    Baseline: 0    ← ERROR!
```

**Causes:**

- Sensor disconnected
- Wrong GPIO pin
- Hardware failure

**Solutions:**

1. **Validation check:**

```cpp
void calibrateSensor(TouchSensor* sensor) {
  // ... perform calibration ...

  if (sensor->baseline < 10 || sensor->baseline > 200) {
    Serial.println("❌ ERROR: Calibration failed!");
    Serial.print("   Baseline: ");
    Serial.println(sensor->baseline);
    Serial.println("   Check wiring!");

    // Use safe defaults
    sensor->baseline = 100;
    sensor->touchThreshold = 70;
    sensor->hoverThreshold = 85;
  }
}
```

2. **Test read before calibration:**

```cpp
void verifyConnection(TouchSensor* sensor) {
  uint16_t test = touchRead(sensor->pin);

  if (test < 10) {
    Serial.print("❌ ");
    Serial.print(sensor->name);
    Serial.println(" not connected!");
    return false;
  }
  return true;
}
```

---

## 🔄 Adaptive Calibration

### Exponential Moving Average (EMA)

Continuously update baseline slowly:

```cpp
void setupAdaptive(TouchSensor* sensor) {
  sensor->emaBaseline = (float)sensor->baseline;
}

void updateAdaptiveBaseline(TouchSensor* sensor) {
  const float ALPHA = 0.001;  // Very slow (0.1% per sample)

  if (!sensor->isTouched && !sensor->wasHovering) {
    // Only update when truly untouched
    sensor->emaBaseline = ALPHA * sensor->currentValue +
                          (1 - ALPHA) * sensor->emaBaseline;

    // Update integer baseline every N samples
    static int counter = 0;
    if (++counter >= 100) {
      sensor->baseline = (uint16_t)sensor->emaBaseline;

      // Recalculate thresholds
      sensor->touchThreshold = sensor->baseline * 0.70;
      sensor->hoverThreshold = sensor->baseline * 0.85;

      counter = 0;
    }
  }
}
```

### Time-Based Recalibration

Automatically recalibrate periodically:

```cpp
unsigned long lastCalibrationTime = 0;
const unsigned long RECAL_INTERVAL = 3600000;  // 1 hour

void loop() {
  if (millis() - lastCalibrationTime > RECAL_INTERVAL) {
    Serial.println("Periodic recalibration...");

    // Check if sensors are idle
    bool allIdle = true;
    for (int i = 0; i < 3; i++) {
      if (sensors[i].isTouched) {
        allIdle = false;
        break;
      }
    }

    if (allIdle) {
      calibrateSensors();
      lastCalibrationTime = millis();
    }
  }
}
```

---

## 🌍 Multi-Environment Calibration

### Scenario: Indoor/Outdoor Operation

```cpp
enum Environment {
  ENV_INDOOR,
  ENV_OUTDOOR,
  ENV_HUMID,
  ENV_DRY
};

struct EnvironmentProfile {
  uint16_t baseline;
  uint8_t touchPercent;
  uint8_t hoverPercent;
};

EnvironmentProfile profiles[3][4];  // 3 sensors × 4 environments

void calibrateForEnvironment(Environment env) {
  Serial.print("Calibrating for environment: ");
  Serial.println(env);

  for (int i = 0; i < 3; i++) {
    calibrateSensor(&sensors[i]);

    // Save profile
    profiles[i][env].baseline = sensors[i].baseline;
    profiles[i][env].touchPercent = TOUCH_THRESHOLD_PERCENT;
    profiles[i][env].hoverPercent = HOVER_THRESHOLD_PERCENT;
  }

  saveProfilesToEEPROM();
}

void loadEnvironment(Environment env) {
  for (int i = 0; i < 3; i++) {
    sensors[i].baseline = profiles[i][env].baseline;
    sensors[i].touchThreshold =
      (sensors[i].baseline * profiles[i][env].touchPercent) / 100;
    sensors[i].hoverThreshold =
      (sensors[i].baseline * profiles[i][env].hoverPercent) / 100;
  }
}
```

---

## ✅ Validation and Testing

### Calibration Quality Metrics

```cpp
struct CalibrationQuality {
  uint16_t range;          // max - min
  float stdDeviation;      // Standard deviation
  float coefficient;       // Coefficient of variation (%)
  bool isGood;             // Overall quality flag
};

CalibrationQuality assessCalibration(uint16_t* samples, int count) {
  CalibrationQuality quality;

  // Calculate range
  uint16_t minVal = 65535, maxVal = 0;
  for (int i = 0; i < count; i++) {
    if (samples[i] < minVal) minVal = samples[i];
    if (samples[i] > maxVal) maxVal = samples[i];
  }
  quality.range = maxVal - minVal;

  // Calculate mean
  uint32_t sum = 0;
  for (int i = 0; i < count; i++) {
    sum += samples[i];
  }
  float mean = (float)sum / count;

  // Calculate standard deviation
  float variance = 0;
  for (int i = 0; i < count; i++) {
    float diff = samples[i] - mean;
    variance += diff * diff;
  }
  quality.stdDeviation = sqrt(variance / count);

  // Coefficient of variation
  quality.coefficient = (quality.stdDeviation / mean) * 100.0;

  // Quality assessment
  quality.isGood = (quality.coefficient < 5.0);  // <5% variation

  return quality;
}
```

### Usage:

```cpp
void calibrateSensor(TouchSensor* sensor) {
  uint16_t samples[50];

  for (int i = 0; i < 50; i++) {
    samples[i] = touchRead(sensor->pin);
    delay(20);
  }

  CalibrationQuality quality = assessCalibration(samples, 50);

  Serial.print("Range: ");
  Serial.println(quality.range);
  Serial.print("Std Dev: ");
  Serial.println(quality.stdDeviation);
  Serial.print("CV: ");
  Serial.print(quality.coefficient);
  Serial.println("%");

  if (quality.isGood) {
    Serial.println("✓ Good quality");
  } else {
    Serial.println("⚠️ Poor quality - check environment");
  }

  sensor->baseline = samples[25];  // Use median
}
```

---

## 📊 Calibration Best Practices Summary

### Do's ✅

1. **Calibrate in target environment**
   - Same temperature
   - Same humidity
   - Same electrical noise

2. **Use sufficient samples**
   - Minimum: 30 samples
   - Recommended: 50-100 samples
   - Critical applications: 200+ samples

3. **Filter outliers**
   - Discard top/bottom 10%
   - Use median for noisy environments

4. **Validate results**
   - Check range is reasonable
   - Verify stability over time
   - Test with actual touches

5. **Document baselines**
   - Log calibration values
   - Track changes over time
   - Note environmental conditions

### Don'ts ❌

1. **Don't skip calibration**
   - Fixed thresholds will fail
   - Different hardware = different values

2. **Don't calibrate while touched**
   - Results will be invalid
   - User must be away from sensors

3. **Don't ignore drift**
   - Temperature affects readings
   - Recalibrate when needed

4. **Don't use single sample**
   - Noise will skew results
   - Always average multiple readings

5. **Don't forget validation**
   - Always test after calibration
   - Confirm touch detection works

---

## 🎯 Quick Reference

### Calibration Checklist

```
Before Calibration:
□ Power supply stable
□ ESP32 warmed up (30+ seconds)
□ Sensors connected properly
□ No objects near sensors
□ Hands away from sensors
□ Minimal environmental movement

During Calibration:
□ Collecting 50+ samples
□ 20ms delay between samples
□ Visual progress indication
□ Checking for outliers
□ Calculating mean/median

After Calibration:
□ Baseline within expected range (50-150)
□ Range < 15% of baseline
□ Thresholds calculated
□ Validation test performed
□ Results displayed to user

Ongoing Monitoring:
□ Check for drift (>10 units)
□ Recalibrate on command
□ Track false positives/negatives
□ Log baseline over time
```

---

## 📚 Summary

**Key Takeaways:**

1. **Calibration is essential** - Touch sensor baselines vary significantly
2. **Sample size matters** - Use 50+ samples for reliable baseline
3. **Validate quality** - Check range and stability
4. **Adapt to changes** - Temperature and humidity affect readings
5. **Test thoroughly** - Confirm touch detection after calibration
6. **Provide feedback** - Show user what's happening during calibration

**Calibration Formula:**

```
Baseline = Average of N samples (untouched state)
Touch Threshold = Baseline × 70%
Hover Threshold = Baseline × 85%

Where:
  N ≥ 50 samples
  Sample interval = 20ms
  Total time ≈ 1 second
```

---

**Need help?** See [README.md](README.md) for complete documentation or [TOUCH_CONCEPTS.md](TOUCH_CONCEPTS.md) for theory!
