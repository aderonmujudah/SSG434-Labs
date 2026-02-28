# Lab-18 Quick Start Guide

## ⚡ 10-Minute Setup

Get ESP32 deep sleep running in 10 minutes!

---

## 🎯 What You'll Learn

- Deep sleep basics
- Timer and button wakeup
- Power consumption measurement

---

## 📦 What You Need

### Essential

- ESP32 board
- LED + 220Ω resistor
- Breadboard & wires
- Multimeter (for current measurement)

### Optional

- External button (GPIO 0 is built-in)
- 10kΩ resistor for GPIO 35

---

## 🔧 Step 1: Wiring (2 minutes)

### Minimal Setup (LED Only)

```
ESP32           LED Circuit
GPIO 12 ───→ 220Ω ───→ LED (+) ───→ LED (-) ───→ GND
```

### Full Setup (LED + Button 2)

```
3.3V ───→ 10kΩ ───┬───→ GPIO 35
                  │
                Button 2
                  │
                 GND

GPIO 12 ───→ 220Ω ───→ LED ───→ GND

GPIO 0 = Built-in BOOT button (no wiring needed)
```

**Important:** GPIO 35 needs external 10kΩ pull-up to 3.3V!

---

## 📤 Step 2: Upload Code (3 minutes)

1. **Open Arduino IDE**
2. **Load firmware:** `ESP32_Deep_Sleep.ino`
3. **Configure:**
   - Board: ESP32 Dev Module
   - Upload Speed: 115200
   - Port: Select your ESP32
4. **Upload** ✓

---

## 🖥️ Step 3: Test Wakeup (3 minutes)

### Open Serial Monitor

1. Set baud rate: **115200**
2. Watch boot messages

### Expected Output

```
╔════════════════════════════════════════════╗
║  ESP32 Deep Sleep & Power Management Lab  ║
╚════════════════════════════════════════════╝

Boot #1
Wakeup cause: 🔄 Reset (not a wakeup): 0
...
You have 30 seconds to enter commands...

[Wait 30 seconds]

⏳ Entering deep sleep...

[10 seconds later - ESP32 wakes]

Boot #2
Wakeup cause: ⏰ Timer wakeup
```

### Quick Tests

**Test 1: Timer Wakeup**

- Wait 30 seconds for sleep
- ESP32 wakes after 10 seconds
- LED blinks 3x on wakeup ✓

**Test 2: Button Wakeup**

- Wait for deep sleep
- Press BOOT button (GPIO 0)
- ESP32 wakes immediately ✓

**Test 3: Statistics**

- Press 'S' key before sleep
- View wakeup counts
- Boot count increments ✓

---

## 📊 Step 4: Measure Power (2 minutes)

### Quick Measurement

1. **Disconnect USB cable** (important!)
2. **Connect battery** (3.7V or 3.3V supply)
3. **Insert multimeter:**
   ```
   Battery (+) ───→ [Multimeter] ───→ ESP32 VIN
   Battery (-) ───────────────────→ ESP32 GND
   ```
4. **Set multimeter:**
   - Start: mA range (200mA)
   - After sleep: µA range (200µA or 2000µA)
5. **Wait for deep sleep** (30 seconds)
6. **Read current:**
   - Timer mode: ~150µA
   - Button mode: ~10µA

### No Multimeter?

Watch LED behavior:

- **Blinking (1 Hz):** Active mode (~80mA)
- **Off:** Deep sleep (~10-150µA)

---

## 🎮 Quick Command Reference

| Key   | Action                       |
| ----- | ---------------------------- |
| **1** | Timer only (10s wakeup)      |
| **2** | Buttons only (lowest power!) |
| **3** | Combined (default)           |
| **S** | Show statistics              |
| **R** | Reset statistics             |
| **P** | Power consumption info       |
| **H** | Help menu                    |

**Note:** 30-second window after wakeup to enter commands!

---

## 💡 Quick Tips

### Lowest Power Strategy

Want 22+ years battery life?

```
1. Press '2' (Buttons Only)
2. Let ESP32 sleep
3. Current drops to ~10µA
4. Wake with button press
```

### Periodic Sensor Readings

Want automatic wakeup every 10s?

```
1. Press '1' (Timer Only)
2. ESP32 wakes every 10s
3. Current: ~150µA
4. Add sensor code in setup()
```

### Manual Override

Want timer + manual wakeup?

```
1. Press '3' (Combined) - Default
2. Wakes every 10s OR on button press
3. Best of both worlds
```

---

## 🐛 Quick Troubleshooting

### ESP32 Won't Wake

**Problem:** Sleeps forever

**Fixes:**

- ✓ Check timer value (should be 10s)
- ✓ Press EN button to reset
- ✓ Re-upload code

---

### Button Not Working (GPIO 35)

**Problem:** Button 2 doesn't wake ESP32

**Fix:** Add 10kΩ pull-up resistor

```
3.3V ───→ 10kΩ ───┬───→ GPIO 35
                  │
                Button
                  │
                 GND
```

GPIO 35 has **no internal pull-up**!

---

### High Current (>1mA)

**Problem:** Current should be µA, not mA

**Fixes:**

- ❌ **Remove USB cable** (adds ~5-10mA)
- ❌ Turn off external LEDs
- ❌ Check multimeter range (use µA, not mA)
- ✓ Wait for deep sleep (LED stops blinking)

---

### Statistics Reset

**Problem:** Boot count always shows 1

**Causes:**

- Power was cycled (expected behavior)
- Hard reset (EN button)
- Brownout (low voltage)

**RTC memory clears on power loss!**

---

## 📈 Quick Results

### What to Expect

| Test                 | Result                        |
| -------------------- | ----------------------------- |
| **First boot**       | LED blinks 3x, shows menu     |
| **After 30s**        | "Entering deep sleep..."      |
| **After 10s more**   | Wakes, LED blinks 3x, Boot #2 |
| **Button press**     | Immediate wake, Boot #3       |
| **Current (Timer)**  | ~150µA                        |
| **Current (Button)** | ~10µA                         |

### Success Checklist

```
□ LED connected and blinking
□ Serial output shows boot messages
□ Timer wakeup after 10 seconds
□ Button wakeup works
□ Boot count increments
□ Current <200µA in deep sleep (without USB)
□ RTC memory persists (boot count survives sleep)
```

---

## 🎯 Next Steps

### Basic Experiments

1. **Change timer duration:**

   ```cpp
   #define TIMER_WAKEUP_TIME  60  // 60 seconds
   ```

2. **Add sensor reading:**

   ```cpp
   void setup() {
     ...
     float temp = readTemperature();
     Serial.print("Temperature: ");
     Serial.println(temp);
     ...
   }
   ```

3. **Log data in RTC memory:**

   ```cpp
   RTC_DATA_ATTR float temperatures[10];
   RTC_DATA_ATTR int tempIndex = 0;

   void setup() {
     temperatures[tempIndex++] = readTemp();
     if (tempIndex >= 10) tempIndex = 0;
   }
   ```

### Advanced Projects

- **Weather Station:** Read DHT11 every 10 minutes, sleep between
- **Door Monitor:** Button-only wakeup, ~23 years battery!
- **Data Logger:** Store readings in RTC RAM, upload on 10th wakeup
- **Motion Detector:** PIR sensor triggers via EXT0 wakeup

---

## 🔋 Power Consumption Cheat Sheet

```
Mode                  Current    Battery Life (2000mAh)
─────────────────────────────────────────────────────
Active (WiFi on)      200mA      ~10 hours
Active (WiFi off)     60mA       ~33 hours
Light Sleep           0.8mA      ~104 days
Deep Sleep (Timer)    150µA      ~555 days (1.5 years)
Deep Sleep (Button)   10µA       ~8,333 days (23 years!) ⚡
Hibernation           5µA        ~16,666 days (45 years!)
```

**Key Insight:** Button-only wakeup gives **2000x** longer battery life than light sleep!

---

## 💡 Pro Tips

1. **Always disconnect USB for accurate measurements**
   - USB circuit adds 5-10mA

2. **Use buttons-only mode for longest battery life**
   - Press '2' command
   - ~10µA current
   - 23 years on 2000mAh battery

3. **RTC memory survives sleep, not power loss**
   - Boot count increments across sleeps
   - Resets on power cycle

4. **GPIO 35 needs external pull-up**
   - No internal pull-up available
   - Use 10kΩ to 3.3V

5. **Switch multimeter ranges**
   - mA range while active (prevents blown fuse)
   - µA range during sleep (accurate reading)

---

## 📚 Learn More

For deeper understanding:

- [README.md](README.md) - Complete documentation
- [POWER_CONCEPTS.md](POWER_CONCEPTS.md) - Theory and architecture
- [MEASUREMENT_GUIDE.md](MEASUREMENT_GUIDE.md) - Advanced measurement techniques

---

## ✅ Quick Verification

Run these checks in first 5 minutes:

```
□ Upload successful
□ Serial monitor shows boot messages
□ LED blinks 3 times on wakeup
□ ESP32 enters sleep after 30 seconds
□ Timer wakeup after 10 seconds
□ BOOT button wakes ESP32
□ Boot counter increments
□ 'S' command shows statistics
```

**All checked?** You're ready to explore power management! 🎉

---

**Total Time:** ~10 minutes  
**Difficulty:** ⭐⭐☆☆☆ (Beginner-Intermediate)  
**Battery Life Achievement:** 🔋 Up to 23 years with button-only mode!
