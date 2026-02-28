# Lab-18: ESP32 Deep Sleep & Power Management

## 🎯 Objectives

Learn ESP32 power management techniques to maximize battery life in IoT applications:

1. ✅ Understand ESP32 power modes and consumption
2. ✅ Implement deep sleep with timer wakeup
3. ✅ Use RTC memory for data persistence
4. ✅ Configure external wakeup sources (buttons)
5. ✅ Combine multiple wakeup strategies
6. ✅ Measure power consumption with multimeter
7. ✅ Optimize for different use cases

---

## 📋 Hardware Requirements

### Components

- ESP32 Development Board (1x)
- LED (1x) - Any color
- 220Ω Resistor (1x) - For LED
- Push Buttons (2x) - Or use built-in BOOT button
- 10kΩ Resistor (1x) - Pull-up for GPIO 35
- Breadboard
- Jumper Wires
- **Multimeter** (for current measurement)

### Optional

- Battery (3.7V Li-Po/Li-Ion)
- USB current meter module
- Oscilloscope (for detailed analysis)

---

## 🔌 Pin Configuration

| Component    | GPIO Pin | Notes                                   |
| ------------ | -------- | --------------------------------------- |
| **LED**      | 12       | 220Ω resistor to GND                    |
| **Button 1** | 0        | Built-in BOOT button (internal pull-up) |
| **Button 2** | 35       | Needs **external 10kΩ pull-up** to 3.3V |

### Why These Pins?

**GPIO 12 (LED):**

- Standard GPIO with RTC support
- Can be isolated during deep sleep
- Safe for LED output

**GPIO 0 (Button 1):**

- Built-in BOOT button on most ESP32 boards
- Internal pull-up resistor
- RTC GPIO (supports EXT0/EXT1 wakeup)

**GPIO 35 (Button 2):**

- Input-only pin
- RTC GPIO (supports EXT1 wakeup)
- **Requires external pull-up** (no internal pull-up)

---

## 🛠️ Wiring Diagram

```
ESP32 Pin Layout:

         ESP32-WROOM-32
    ┌─────────────────────┐
    │                     │
    │    3.3V ●───┬───────┼──→ 10kΩ Resistor → Button 2 → GND
    │             │       │
    │    GND  ●───┼───────┼──→ LED Cathode (-)
    │             │       │
    │    GPIO 12 ●┼───────┼──→ LED Anode (+) → 220Ω → GND
    │             │       │
    │    GPIO 0  ●┼───────┼──→ BOOT Button (built-in)
    │             │       │
    │    GPIO 35 ●┼───────┼──→ Button 2 Terminal
    │             │       │
    └─────────────┴───────┘
                  │
                  └──→ Common GND

LED Connection (Detail):
    GPIO 12 ──→ 220Ω Resistor ──→ LED (+) ──→ LED (-) ──→ GND

Button 2 Connection (Detail):
    3.3V ──→ 10kΩ Resistor ──→ GPIO 35
                               ↓
                            Button 2
                               ↓
                              GND
```

### Critical: Button 2 Pull-up

GPIO 35 is **input-only** and has **no internal pull-up**. You **must** add a 10kΩ external pull-up resistor:

```
Before Pull-up (WRONG ❌):
    GPIO 35 floating → Unreliable reads → Random wakeups

After Pull-up (CORRECT ✓):
    3.3V ──[10kΩ]──→ GPIO 35 ──[Button]──→ GND
    Normally HIGH, LOW when pressed
```

---

## 💤 ESP32 Power Modes

### Power Mode Comparison

| Mode            | Current  | CPU   | RAM  | WiFi  | RTC | Wakeup Time | Use Case         |
| --------------- | -------- | ----- | ---- | ----- | --- | ----------- | ---------------- |
| **Active**      | 80-260mA | ✓     | ✓    | ✓     | ✓   | N/A         | Normal operation |
| **Modem Sleep** | 20-40mA  | ✓     | ✓    | Sleep | ✓   | <1ms        | WiFi standby     |
| **Light Sleep** | 0.8mA    | Sleep | ✓    | Sleep | ✓   | 1-3ms       | Quick wakeup     |
| **Deep Sleep**  | 10-150µA | Off   | Lost | Off   | ✓   | 50-200ms    | Long sleep       |
| **Hibernation** | 5µA      | Off   | Lost | Off   | Min | 50-200ms    | Ultra low power  |

### Deep Sleep Details

**What's ON:**

- ✅ RTC (Real-Time Clock)
- ✅ RTC memory (8KB slow memory)
- ✅ RTC GPIO (for wakeup)
- ✅ ULP co-processor (optional)

**What's OFF:**

- ❌ Main CPU (Xtensa cores)
- ❌ Main RAM (all variables lost)
- ❌ WiFi/Bluetooth
- ❌ Peripherals (UART, SPI, I2C)
- ❌ Most GPIOs

**Current Consumption Factors:**

```
Base Deep Sleep:         ~10µA
+ Timer enabled:         +140µA  → ~150µA
+ EXT0 wakeup:          +0µA    → ~10µA  (lowest!)
+ EXT1 wakeup:          +0µA    → ~10µA  (very low)
+ ULP running:          +100µA  → ~110µA
+ RTC peripherals:      +varies
```

---

## 🔋 Wakeup Sources

### 1. Timer Wakeup (ESP_SLEEP_WAKEUP_TIMER)

Wake after specified time:

```cpp
esp_sleep_enable_timer_wakeup(10 * 1000000); // 10 seconds
esp_deep_sleep_start();
```

**Pros:**

- ✅ Predictable timing
- ✅ Good for periodic tasks (sensor readings, data uploads)

**Cons:**

- ⚠️ Higher power (~150µA vs ~10µA)
- ⚠️ Still drains battery even when not needed

**Use Cases:**

- Weather station (read every 10 minutes)
- Data logger (log every hour)
- Periodic health checks

---

### 2. External Wakeup - EXT0 (Single Pin)

Wake via level change on **one** RTC GPIO:

```cpp
esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0); // Wake when GPIO 0 is LOW
esp_deep_sleep_start();
```

**Pros:**

- ✅ **Lowest power** (~10µA)
- ✅ Simple logic (HIGH or LOW)
- ✅ Fast wakeup

**Cons:**

- ⚠️ Only supports **one pin**
- ⚠️ Limited to RTC GPIOs

**Use Cases:**

- Doorbell (single button)
- Power switch
- ON/OFF control

---

### 3. External Wakeup - EXT1 (Multiple Pins)

Wake via logic on **multiple** RTC GPIOs:

```cpp
// Wake when ANY button is pressed (goes HIGH)
esp_sleep_enable_ext1_wakeup(
  (1ULL << GPIO_NUM_0) | (1ULL << GPIO_NUM_35),
  ESP_EXT1_WAKEUP_ANY_HIGH
);

// OR wake when BOTH buttons are pressed (go LOW)
esp_sleep_enable_ext1_wakeup(
  (1ULL << GPIO_NUM_0) | (1ULL << GPIO_NUM_35),
  ESP_EXT1_WAKEUP_ALL_LOW
);

esp_deep_sleep_start();
```

**Logic Modes:**

- `ESP_EXT1_WAKEUP_ANY_HIGH` - Wake if **any** pin goes HIGH (OR logic)
- `ESP_EXT1_WAKEUP_ALL_LOW` - Wake if **all** pins are LOW (AND logic)

**Pros:**

- ✅ Multiple inputs
- ✅ AND/OR logic
- ✅ Very low power (~10µA)

**Cons:**

- ⚠️ No individual pin identification (must read after wakeup)
- ⚠️ More complex than EXT0

**Use Cases:**

- Multiple buttons (menu, select, back)
- Security systems (door + window sensors)
- Multi-input triggers

---

### 4. Combined Wakeup Sources

Enable **multiple** wakeup sources simultaneously:

```cpp
// Wake on timer OR button press
esp_sleep_enable_timer_wakeup(10 * 1000000);
esp_sleep_enable_ext1_wakeup(
  (1ULL << GPIO_NUM_0) | (1ULL << GPIO_NUM_35),
  ESP_EXT1_WAKEUP_ANY_HIGH
);
esp_deep_sleep_start();
```

**When to Use:**

- Timer: Regular periodic tasks
- Button: Manual override for immediate wakeup

**Power Impact:**

- Combined power = Timer current (~150µA)
- Timer is the dominant power consumer

---

## 💾 RTC Memory

### What is RTC Memory?

**RTC Memory** = Special RAM that stays powered during deep sleep

**Specifications:**

- **Size:** 8KB (RTC_SLOW_MEM)
- **Retention:** Persists through deep sleep
- **Lost on:** Power loss, hard reset, brownout
- **Access:** Use `RTC_DATA_ATTR` attribute

### Usage Example

```cpp
// Declare variables with RTC_DATA_ATTR
RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR float lastTemperature = 0.0;
RTC_DATA_ATTR char deviceName[32] = "";

void setup() {
  bootCount++;  // Increments across deep sleeps!

  Serial.print("Boot #");
  Serial.println(bootCount);

  // Last value is remembered
  Serial.print("Previous temperature: ");
  Serial.println(lastTemperature);
}
```

**What Persists:**

- ✅ Simple variables (int, float, char, bool)
- ✅ Arrays
- ✅ Structs

**What Doesn't Persist:**

- ❌ Regular variables (lost after sleep)
- ❌ Heap memory (malloc/new)
- ❌ Global objects with constructors
- ❌ File handles, WiFi connections

### Use Cases

1. **Boot Counter** - Track wakeup events
2. **State Persistence** - Remember device state
3. **Calibration Data** - Store sensor baselines
4. **Configuration** - Keep settings during sleep
5. **Accumulation** - Sum values over time

---

## 🎛️ Wakeup Strategies

This lab implements **5 strategies** with different power profiles:

### Strategy 1: Timer Only

```cpp
esp_sleep_enable_timer_wakeup(10 * 1000000); // 10s
```

**Power:** ~150µA  
**Use Case:** Periodic sensor readings (weather station)  
**Battery Life (2000mAh):** ~555 days

---

### Strategy 2: Buttons Only

```cpp
esp_sleep_enable_ext1_wakeup(
  (1ULL << GPIO_NUM_0) | (1ULL << GPIO_NUM_35),
  ESP_EXT1_WAKEUP_ANY_HIGH
);
```

**Power:** ~10µA  
**Use Case:** Manual trigger devices (doorbell, remote control)  
**Battery Life (2000mAh):** ~23 years! ⚡

---

### Strategy 3: Combined (Timer + Buttons)

```cpp
esp_sleep_enable_timer_wakeup(10 * 1000000);
esp_sleep_enable_ext1_wakeup(...);
```

**Power:** ~150µA (timer dominates)  
**Use Case:** Periodic tasks with manual override  
**Battery Life (2000mAh):** ~555 days

---

### Strategy 4: Both Buttons (AND Logic)

```cpp
esp_sleep_enable_ext1_wakeup(
  (1ULL << GPIO_NUM_0) | (1ULL << GPIO_NUM_35),
  ESP_EXT1_WAKEUP_ALL_LOW  // Both pressed
);
```

**Power:** ~10µA  
**Use Case:** Security (require two actions)  
**Battery Life (2000mAh):** ~23 years

---

### Strategy 5: Any Button (OR Logic)

```cpp
esp_sleep_enable_ext1_wakeup(
  (1ULL << GPIO_NUM_0) | (1ULL << GPIO_NUM_35),
  ESP_EXT1_WAKEUP_ANY_HIGH  // Either pressed
);
```

**Power:** ~10µA  
**Use Case:** Multiple input triggers  
**Battery Life (2000mAh):** ~23 years

---

## 🔬 Measuring Power Consumption

### Method 1: Multimeter (Most Accurate)

**Setup:**

1. Remove USB cable (USB adds power)
2. Connect battery/power supply
3. Insert multimeter in **series** with VIN or 3.3V

```
Power Supply (+) ───→ [Multimeter] ───→ ESP32 VIN
                                          ↓
Power Supply (-) ←───────────────────── ESP32 GND
```

**Multimeter Settings:**

- **Active mode:** Set to mA range (200mA or higher)
- **Deep sleep:** Set to µA range (200µA or 2000µA)
- **Auto-ranging:** Enable if available

**Expected Readings:**

- Active: 80-260mA (varies with WiFi)
- Deep Sleep (Timer): 140-160µA
- Deep Sleep (EXT0/EXT1): 8-15µA

---

### Method 2: USB Current Meter

**Pros:**

- ✅ Easy to use
- ✅ Real-time display
- ✅ No circuit modification

**Cons:**

- ⚠️ Less accurate for µA measurements
- ⚠️ USB circuit adds overhead (~5mA)

**Devices:**

- USB power meter modules (~$5-10)
- Inline USB current meters
- Lab power supplies with current display

---

### Method 3: INA219 Current Sensor Module

**Pros:**

- ✅ Programmable measurement
- ✅ Data logging capability
- ✅ I2C interface

**Cons:**

- ⚠️ Requires additional hardware
- ⚠️ More complex setup

---

### Measurement Tips

**For Deep Sleep Mode:**

1. **Let ESP32 stabilize:**
   - Upload code
   - Wait for deep sleep to activate (30s timeout)
   - Readings stabilize after 1-2 seconds in sleep

2. **Remove USB cable:**
   - USB circuit draws extra current
   - Use battery or external supply

3. **Use correct range:**
   - Start with mA range (to avoid blowing fuse)
   - Switch to µA range once in deep sleep

4. **Account for LED:**
   - LED off in deep sleep: ~10-150µA
   - LED on would add: ~10-20mA

5. **Temperature matters:**
   - Colder = less current
   - ESP32 leakage increases with temperature

**Expected Values:**

| Configuration | Typical | Range     |
| ------------- | ------- | --------- |
| Timer wakeup  | 150µA   | 140-170µA |
| EXT0 wakeup   | 10µA    | 8-15µA    |
| EXT1 wakeup   | 10µA    | 8-15µA    |
| Hibernation   | 5µA     | 3-7µA     |

**Troubleshooting High Current:**

```
Measured 1mA in deep sleep? Check:
□ USB cable disconnected?
□ No LEDs powered?
□ All peripherals powered down?
□ GPIO floating (use pull-ups/downs)?
□ External components unpowered?
```

---

## 🚀 Testing Procedures

### Test 1: Basic Wakeup Verification

**Goal:** Verify ESP32 wakes up correctly

**Steps:**

1. Upload code
2. Open Serial Monitor (115200 baud)
3. Wait 30 seconds for first sleep
4. Observe boot message after 10 seconds
5. Press Button 1 (GPIO 0) during sleep
6. Press Button 2 (GPIO 35) during sleep

**Expected Results:**

```
Boot #1
Wakeup cause: Reset (not a wakeup): 0
...
💤 Deep sleep configured. Will wake in 10s or on button press.

[10 seconds later]
Boot #2
Wakeup cause: ⏰ Timer wakeup
...

[Press Button 1]
Boot #3
Wakeup cause: 🔘 External signal (EXT1) - Multiple pins
   → Button 1 (GPIO 0) pressed
```

---

### Test 2: RTC Memory Persistence

**Goal:** Verify data survives deep sleep

**Steps:**

1. Reset ESP32 (power cycle or EN button)
2. Note boot count starts at 1
3. Wait for multiple wakeups (timer or button)
4. Observe boot count incrementing
5. Power cycle ESP32
6. Boot count resets to 1 ✓

**Expected Results:**

```
Boot #1 → Boot #2 → Boot #3 → [Power off] → Boot #1
```

---

### Test 3: Strategy Comparison

**Goal:** Test different wakeup strategies

**Steps:**

1. Upload code and open Serial Monitor
2. Press '1' - Timer Only
   - Wait 10 seconds → Should wake
   - Press buttons → Should NOT wake
3. Press '2' - Buttons Only
   - Wait 10+ seconds → Should NOT wake
   - Press button → Should wake
4. Press '3' - Combined
   - Wait 10 seconds → Should wake
   - Press button before timer → Should wake early

**Expected Results:**

- Each strategy behaves as configured
- Statistics track wakeup sources correctly

---

### Test 4: Power Consumption Measurement

**Goal:** Measure actual current draw

**Requirements:** Multimeter with µA range

**Steps:**

1. Disconnect USB cable
2. Connect battery (3.7V) or lab supply (3.3V)
3. Insert multimeter in series
4. Set multimeter to mA range
5. Reset ESP32
6. Wait 30 seconds for deep sleep
7. Switch multimeter to µA range
8. Record reading

**Expected Results:**

```
Strategy 1 (Timer):        ~150µA
Strategy 2 (Buttons):      ~10µA
Strategy 3 (Combined):     ~150µA
Strategy 4 (Both buttons): ~10µA
Strategy 5 (Any button):   ~10µA
```

**Troubleshooting:**

- Reading >1mA: USB still connected or LED powered
- Reading 0µA: Multimeter in wrong mode or blown fuse
- Reading unstable: Poor connections or interference

---

### Test 5: Long-Term Stability

**Goal:** Verify stable operation over time

**Steps:**

1. Set Strategy 1 (Timer Only)
2. Let run for 1 hour (= 360 wakeups)
3. Check boot count matches expected
4. Verify no missed wakeups
5. Check RTC memory data integrity

**Expected Results:**

- Boot count ≈ 360 (± a few due to processing time)
- No anomalous wakeup causes
- Statistics remain accurate

---

## 📊 Statistics Tracking

The firmware tracks comprehensive statistics in RTC memory:

```cpp
RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int timerWakeupCount = 0;
RTC_DATA_ATTR int button1WakeupCount = 0;
RTC_DATA_ATTR int button2WakeupCount = 0;
RTC_DATA_ATTR unsigned long totalActiveTime = 0;
```

### View Statistics

Press 'S' in Serial Monitor:

```
┌─────────────────────────────┐
│   Wakeup Statistics          │
├─────────────────────────────┤
│ Total Boots:             47 │
│ Timer Wakeups:           32 │
│ Button 1 Wakeups:        10 │
│ Button 2 Wakeups:         5 │
├─────────────────────────────┤
│ Total Active Time:     1410s│
│ Avg Sleep Time:          10s│
└─────────────────────────────┘
```

### Reset Statistics

Press 'R' to clear all counters (useful for new tests)

---

## ⚙️ Serial Commands

| Command | Function                     |
| ------- | ---------------------------- |
| **1**   | Timer Only strategy          |
| **2**   | Buttons Only strategy        |
| **3**   | Combined strategy (default)  |
| **4**   | Both Buttons Required (AND)  |
| **5**   | Any Button (OR)              |
| **S**   | Show statistics              |
| **R**   | Reset statistics             |
| **T**   | Test wakeup sources          |
| **P**   | Print power consumption info |
| **H**   | Print help menu              |

**Note:** You have **30 seconds** after wakeup to enter commands before automatic sleep.

---

## 🐛 Troubleshooting

### Issue 1: ESP32 Won't Wake from Deep Sleep

**Symptoms:**

- ESP32 enters sleep but never wakes
- Serial output stops forever

**Causes & Solutions:**

1. **No wakeup source enabled**

   ```cpp
   // WRONG - No wakeup source!
   esp_deep_sleep_start();

   // CORRECT - Enable at least one source
   esp_sleep_enable_timer_wakeup(10 * 1000000);
   esp_deep_sleep_start();
   ```

2. **Wrong button state**
   - EXT0: Check wake level (HIGH vs LOW)
   - Button wiring matches expected state

3. **Timer value too large**

   ```cpp
   // WRONG - Overflow
   esp_sleep_enable_timer_wakeup(10 * 1000);  // Only 10ms!

   // CORRECT - 10 seconds
   esp_sleep_enable_timer_wakeup(10 * 1000000ULL);
   ```

**Fix:** Press EN button to reset, upload corrected code

---

### Issue 2: Button Wakeup Not Working (GPIO 35)

**Symptoms:**

- Button 1 (GPIO 0) works fine
- Button 2 (GPIO 35) doesn't wake ESP32

**Cause:** Missing pull-up resistor

**Solution:**

```
Add 10kΩ resistor from GPIO 35 to 3.3V:

    3.3V ──[10kΩ]──┬──→ GPIO 35
                   │
                 Button
                   │
                  GND
```

GPIO 35 is **input-only** and has **no internal pull-up**.

---

### Issue 3: Current Measurement Shows mA, Not µA

**Symptoms:**

- Multimeter reads 5-50mA instead of µA
- Expected deep sleep (~10-150µA)

**Causes & Solutions:**

1. **USB cable still connected**
   - USB circuit adds ~5-10mA
   - **Solution:** Disconnect USB, use battery

2. **LED or peripheral powered**
   - External components drawing current
   - **Solution:** Disconnect all external devices (except buttons)

3. **GPIO floating**
   - Unconnected GPIOs can draw current
   - **Solution:** Not usually significant, but can add a few µA

4. **ESP32 not in deep sleep yet**
   - Still in 30-second active window
   - **Solution:** Wait until LED stops blinking

5. **Multimeter in wrong range**
   - mA range has higher burden voltage
   - **Solution:** Switch to µA range (200µA or 2000µA)

---

### Issue 4: RTC Memory Data Lost

**Symptoms:**

- Boot count resets to 0 unexpectedly
- Statistics don't persist

**Causes:**

1. **Power loss** - Battery died or disconnected
2. **Hard reset** - EN button or power cycle
3. **Brownout** - Voltage dropped below threshold
4. **Not using RTC_DATA_ATTR** - Variables in normal RAM

**Solution:**

```cpp
// WRONG - Normal RAM (lost in deep sleep)
int bootCount = 0;

// CORRECT - RTC RAM (persists)
RTC_DATA_ATTR int bootCount = 0;
```

---

### Issue 5: High Current in Deep Sleep

**Measured:** >500µA  
**Expected:** 10-150µA

**Debugging Checklist:**

```cpp
□ USB disconnected? (adds ~5mA)
□ All LEDs off? (adds ~10-20mA each)
□ WiFi disabled? (code doesn't use WiFi, so should be off)
□ No floating GPIOs?
□ Correct ESP32 module? (some clones have higher leakage)
□ Room temperature? (higher temp = higher current)
□ Genuine ESP32 chip? (clones vary)
```

**Typical causes:**

- **USB cable:** Most common, adds 5-10mA
- **LED powered:** Each LED adds 10-20mA
- **Regulator efficiency:** Some boards have inefficient regulators

---

### Issue 6: Buttons Don't Work Reliably

**Symptoms:**

- Sporadic wakeups
- Button presses sometimes ignored

**Solutions:**

1. **Add debouncing capacitor (0.1µF)**

   ```
   Button ──||── GND
          0.1µF
   ```

2. **Check pull-up resistor (GPIO 35)**
   - Use 10kΩ, not higher (too weak)
   - Connect to 3.3V, not 5V

3. **Verify button type**
   - Normally open (NO) buttons required
   - Momentary contact, not latching

---

## 🎓 Advanced Topics

### 1. Hibernation Mode (Ultra Low Power)

For absolute minimum power (~5µA):

```cpp
// Disable all wakeup sources except EXT0
esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);

// Only EXT0 wakeup available in hibernation
esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);
esp_deep_sleep_start();
```

**Trade-offs:**

- ✅ Lowest power (~5µA)
- ❌ RTC memory lost
- ❌ Longer wakeup time
- ❌ Only EXT0 wakeup supported

---

### 2. ULP Co-Processor

Run code during deep sleep:

```cpp
// ULP can read sensors, monitor GPIOs during sleep
// Main CPU wakes only when ULP requests
// Adds ~100µA to deep sleep current
```

**Use cases:**

- Threshold monitoring (wake only when sensor > limit)
- Pattern detection (wake on specific GPIO sequence)
- Low-frequency sampling (read ADC every second, log to RTC RAM)

---

### 3. WiFi Battery Optimization

Combine deep sleep with WiFi:

```cpp
void loop() {
  // 1. Wake up
  // 2. Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(100);

  // 3. Quick data upload
  sendDataToServer();

  // 4. Disconnect immediately
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  // 5. Deep sleep
  esp_deep_sleep_start();
}
```

**Battery life calculation:**

- Active (WiFi): 200mA for 5 seconds = 0.278mAh
- Deep sleep: 0.15mA for 3600 seconds = 0.15mAh
- Total per hour: 0.428mAh
- 2000mAh battery: ~4670 hours (~195 days)

---

### 4. GPIO State During Deep Sleep

Control GPIO behavior during sleep:

```cpp
// Hold GPIO state during sleep
rtc_gpio_hold_en(GPIO_NUM_12);

// Or isolate GPIO (high impedance)
rtc_gpio_isolate(GPIO_NUM_12);

// Wake up and release hold
rtc_gpio_hold_dis(GPIO_NUM_12);
```

---

## 📈 Battery Life Calculator

### Formula

```
Battery Life (hours) = Battery Capacity (mAh) / Average Current (mA)
```

### Examples with 2000mAh Battery

| Strategy           | Current | Life (days) | Life (years) |
| ------------------ | ------- | ----------- | ------------ |
| Active (WiFi)      | 150mA   | 0.56        | -            |
| Active (no WiFi)   | 60mA    | 1.39        | -            |
| Light Sleep        | 0.8mA   | 104         | 0.29         |
| Timer wakeup (10s) | 0.15mA  | 555         | 1.52         |
| Buttons only       | 0.01mA  | 8,333       | **22.8** 🎉  |

### Real-World Adjustments

**Duty cycle matters:**

```
Weather Station Example:
- Wake every 10 minutes
- Active for 10 seconds (WiFi upload)
- Sleep for 590 seconds

Active current:   200mA × 10s  = 0.556mAh
Sleep current:    0.15mA × 590s = 0.025mAh
Total per cycle:  0.581mAh per 10 minutes
Per day:          83.7mAh
Battery life:     2000mAh / 83.7mAh = ~24 days
```

---

## 🎯 Key Takeaways

1. **Deep sleep is essential** for battery-powered IoT devices
   - 10,000x power reduction (vs active mode)
   - Enables multi-year battery life

2. **RTC memory preserves state** during deep sleep
   - 8KB available for critical data
   - Lost on power cycle

3. **Multiple wakeup sources** offer flexibility
   - Timer: periodic tasks
   - EXT0: single button, lowest power
   - EXT1: multiple buttons with logic

4. **Power measurement is critical**
   - Use multimeter for accurate readings
   - Disconnect USB for true measurements
   - Expect 10-150µA depending on strategy

5. **Choose strategy based on use case:**
   - Periodic tasks → Timer wakeup
   - Event-driven → Button wakeup (lowest power!)
   - Hybrid → Combined mode

---

## 📚 Additional Resources

- [ESP32 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf) - Chapter 4: Reset and Clock
- [ESP-IDF Sleep Modes Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html)
- [Low Power Mode Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/low-power-mode.html)

---

## 📝 Summary

**This Lab Teaches:**

- ✅ ESP32 deep sleep implementation
- ✅ Timer and external wakeup configuration
- ✅ RTC memory usage for data persistence
- ✅ Power measurement techniques
- ✅ Strategy selection for different use cases
- ✅ Battery life optimization

**Next Steps:**

- Combine with WiFi for IoT applications
- Add sensors for data logging
- Implement ULP co-processor for advanced monitoring
- Create battery-powered weather station

**Need Help?**

- See [QUICK_START.md](QUICK_START.md) for rapid setup
- See [POWER_CONCEPTS.md](POWER_CONCEPTS.md) for deep dive into power management theory
- See [MEASUREMENT_GUIDE.md](MEASUREMENT_GUIDE.md) for detailed measurement procedures

---

**🎉 Achievement Unlocked: Power Management Master!** 🔋⚡
