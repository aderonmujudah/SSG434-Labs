# Lab-16 Quick Start Guide

## ⚡ 10-Minute Setup

Get your ESP32 dual-core system running in minutes!

---

## Step 1: Hardware Setup (5 minutes)

### Minimal Wiring

```
ESP32          Components
═══════════════════════════════════════
GPIO 2  ───[220Ω]───LED1(+)───GND
GPIO 4  ───[220Ω]───LED2(+)───GND
GPIO 5  ───[220Ω]───LED3(+)───GND
GPIO 18 ───BUTTON1───GND
GPIO 19 ───BUTTON2───GND
```

**✓ Quick Check:**

- 3 LEDs with resistors in series
- 2 buttons connected to GND (internal pull-ups used)
- All grounds connected together

---

## Step 2: Software Setup (3 minutes)

### Upload Firmware

1. **Open Arduino IDE**

   ```
   File → Open → ESP32_Dual_Core_Demo.ino
   ```

2. **Select Board**

   ```
   Tools → Board → ESP32 Dev Module
   Tools → Upload Speed → 115200
   Tools → Port → [Your ESP32 Port]
   ```

3. **Upload**
   - Click **Upload** button (→)
   - Wait for "Done uploading" message

---

## Step 3: Verify Operation (2 minutes)

### Serial Monitor

1. Open Serial Monitor: **Tools → Serial Monitor**
2. Set baud rate to **115200**
3. You should see:

```
╔════════════════════════════════════════╗
║  ESP32 Dual-Core Programming Demo     ║
║  Lab-16: FreeRTOS Multi-Core Tasks    ║
╚════════════════════════════════════════╝

✓ Mutexes created
✓ Semaphore created
✓ Queue created

CPU Configuration:
  - CPU Frequency: 240 MHz
  - Number of Cores: 2

✓ All tasks created and started!
```

### Visual Check

You should see:

- ✅ All LEDs OFF at startup
- ✅ Press Button 1: Shared LED blinks 3 times, LED 1 (GPIO 2) starts blinking
- ✅ Press Button 2: Shared LED blinks 3 times, LED 1 stops and LED 2 (GPIO 4) starts blinking

---

## 🧪 Quick Tests

### Test 1: Button Event → Flash Pattern

**Press Button 1 (GPIO 18)**

**Result:**

- Shared LED flashes 3 times rapidly
- LED 1 (GPIO 2) starts blinking
- Serial Monitor shows:

```
[CORE 0] Button 1 event sent to queue

========== BUTTON EVENT ==========
Button: 1
Detected by Core: 0
Processed by Core: 1
==================================
```

### Test 2: Counter Reset

**Press Button 2 (GPIO 19)**

**Result:**

- Shared LED flashes 3 times rapidly
- LED 1 (GPIO 2) stops, LED 2 (GPIO 4) starts blinking

### Test 3: Synchronized Monitoring

**Wait ~2.5 seconds**

**Result:**

- System statistics appear automatically:

```
----- SYNCHRONIZED EVENT -----
Core 0 Task Count: 12
Core 1 Task Count: 5
Shared Counter: 3
Free Heap: 245678 bytes
------------------------------
```

---

## 🎯 What's Happening?

| Feature       | Description                            |
| ------------- | -------------------------------------- |
| **Core 0**    | Runs fast LED blink + Button 1 monitor |
| **Core 1**    | Runs slow LED blink + Button 2 monitor |
| **Queue**     | Transfers button events between tasks  |
| **Mutex**     | Protects shared LED and Serial output  |
| **Semaphore** | Synchronizes monitoring task           |

---

## 🐛 Troubleshooting

### No LEDs Blinking

- ✓ Check wiring and resistor values (220-330Ω)
- ✓ Verify LEDs are correct polarity (long leg = +)
- ✓ Confirm firmware uploaded successfully

### Buttons Not Working

- ✓ Ensure buttons connect GPIO to GND when pressed
- ✓ Check Serial Monitor for button event messages
- ✓ Try holding button for 1 second

### Serial Monitor Blank

- ✓ Verify baud rate is 115200
- ✓ Press ESP32 reset button
- ✓ Check USB cable and connection

### Garbled Serial Output

- ✓ Close other programs using the COM port
- ✓ Disconnect and reconnect USB
- ✓ Set baud rate exactly to 115200

---

## 📊 System Architecture Summary

```
┌──────────────┐         ┌──────────────┐
│   CORE 0     │         │   CORE 1     │
│              │         │              │
│ Fast Blink   │         │ Slow Blink   │
│ Button 1     │         │ Button 2     │
└──────┬───────┘         └──────┬───────┘
       │                        │
       │     ┌──────────┐      │
       └────►│  QUEUE   │◄─────┘
             └────┬─────┘
                  │
       ┌──────────▼──────────┐
       │  Event Processor    │
       │  (Handles buttons)  │
       └─────────────────────┘
```

**Communication Methods:**

- **Queue**: Button events
- **Mutex**: Shared LED + Serial
- **Semaphore**: Task synchronization

---

## 🌟 Next Experiments

Once you've verified basic operation, try these:

### Experiment 1: Change Blink Speeds

Modify delays in the code:

```cpp
// In Task_Core0_BlinkCode
vTaskDelay(100 / portTICK_PERIOD_MS);  // Change from 200 to 100
```

### Experiment 2: Adjust Priorities

Change task priorities to see scheduling effects:

```cpp
xTaskCreatePinnedToCore(Task_Core0_BlinkCode, "Core0_Blink",
                        10000, NULL, 5, &Task_Core0_Blink, 0);
                        // Changed priority from 2 to 5
```

### Experiment 3: Monitor Stack Usage

Add to any task:

```cpp
UBaseType_t freeStack = uxTaskGetStackHighWaterMark(NULL);
safePrintInt("Free stack: ", freeStack);
```

---

## 📚 Learn More

For detailed information, see:

- **[README.md](README.md)** - Complete documentation
- **[CORE_CONCEPTS.md](CORE_CONCEPTS.md)** - Deep dive into dual-core architecture
- **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** - Common issues and solutions

---

## ✅ Success Checklist

- [ ] Hardware wired correctly (3 LEDs, 2 buttons)
- [ ] Firmware uploaded successfully
- [ ] LED 1 blinking fast (Core 0)
- [ ] LED 2 blinking slow (Core 1)
- [ ] Button 1 triggers shared LED flash
- [ ] Button 2 resets counter
- [ ] Serial Monitor shows system status
- [ ] Synchronized events appear every ~2.5 seconds

**All checked? Congratulations! Your dual-core ESP32 system is working! 🎉**

---

## 🚀 Performance Stats

**Expected Metrics:**

- **CPU Frequency:** 240 MHz
- **Free Heap:** ~240-250 KB (varies)
- **Task Switches:** ~1000/second
- **Queue Usage:** 0-2 events typically
- **Core Load:** Balanced 50/50

---

**Need Help?** Check [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for detailed solutions!
