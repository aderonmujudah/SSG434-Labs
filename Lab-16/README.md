# Lab-16: ESP32 Dual-Core Programming

## 🎯 Overview

This lab demonstrates advanced ESP32 dual-core programming using FreeRTOS. Learn how to create tasks pinned to specific cores, implement inter-task communication with queues, protect shared resources with mutexes, and optimize multi-core performance.

### Learning Objectives

- ✅ Understand ESP32 dual-core architecture
- ✅ Create FreeRTOS tasks with core affinity
- ✅ Implement inter-task communication using queues
- ✅ Protect shared resources with mutexes
- ✅ Synchronize tasks across cores with semaphores
- ✅ Optimize task scheduling and priorities
- ✅ Monitor multi-core performance

---

## 🔧 Hardware Requirements

### Components

| Component               | Quantity | Notes                    |
| ----------------------- | -------- | ------------------------ |
| ESP32 Development Board | 1        | Any ESP32 with dual-core |
| LEDs                    | 3        | Standard 5mm LEDs        |
| Resistors (220Ω - 330Ω) | 3        | For LED current limiting |
| Push Buttons            | 2        | Momentary push buttons   |
| Breadboard              | 1        | For connections          |
| Jumper Wires            | Several  | Male-to-male             |

### Pin Configuration

| Component      | GPIO Pin | Description                        |
| -------------- | -------- | ---------------------------------- |
| LED 1 (Core 0) | 2        | Fast blink pattern (200ms)         |
| LED 2 (Core 1) | 4        | Slow blink pattern (500ms)         |
| LED 3 (Shared) | 5        | Mutex-protected shared LED         |
| Button 1       | 18       | Triggers event, flashes shared LED |
| Button 2       | 19       | Resets shared counter              |

---

## 🔌 Wiring Diagram

### LED Connections

```
ESP32                  LEDs
GPIO 2  ----[220Ω]----LED1----GND
GPIO 4  ----[220Ω]----LED2----GND
GPIO 5  ----[220Ω]----LED3----GND
```

### Button Connections

```
ESP32                  Buttons
GPIO 18 ----BTN1----GND  (Internal pull-up enabled)
GPIO 19 ----BTN2----GND  (Internal pull-up enabled)
```

**Note:** Buttons use internal pull-up resistors. Pressing the button connects the GPIO to GND (active LOW).

---

## 💻 Software Setup

### Required Libraries

All libraries are included with the ESP32 Arduino Core:

- **Arduino.h** - Core Arduino functions
- **FreeRTOS** - Built into ESP32 core (no separate install needed)

### Installation Steps

1. **Install ESP32 Board Support:**
   - Open Arduino IDE
   - Go to **File → Preferences**
   - Add to **Additional Board Manager URLs:**
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to **Tools → Board → Boards Manager**
   - Search for "esp32" and install **esp32 by Espressif Systems**

2. **Select Board:**
   - **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
   - **Tools → Upload Speed → 115200**
   - **Tools → Port → [Your ESP32 COM Port]**

---

## 🚀 Quick Start

### 1. Upload the Firmware

1. Open `ESP32_Dual_Core_Demo.ino` in Arduino IDE
2. Connect your ESP32 via USB
3. Click **Upload** button
4. Wait for compilation and upload to complete

### 2. Verify Operation

Open **Serial Monitor** (115200 baud). You should see:

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
  - Setup running on Core: 1

Creating Core 0 tasks...
Creating Core 1 tasks...
Creating shared tasks...

✓ All tasks created and started!

System Status:
  - LED on GPIO 2: Off by default, blinks after Button 1
  - LED on GPIO 4: Off by default, blinks after Button 2
  - LED on GPIO 5: Shared LED (blinks on button events)
  - Button on GPIO 18: Blink shared LED, start Core 0 LED
  - Button on GPIO 19: Blink shared LED, start Core 1 LED
```

### 3. Test the System

**Visual Verification:**

- All LEDs are OFF at startup
- Press Button 1: shared LED blinks 3 times, Core 0 LED starts blinking
- Press Button 2: shared LED blinks 3 times, Core 1 LED starts blinking

**Button Tests:**

- Press Button 1 (GPIO 18): Shared LED flashes 3 times, Core 0 LED starts
- Press Button 2 (GPIO 19): Shared LED flashes 3 times, Core 1 LED starts

---

## 📊 System Architecture

### Task Overview

| Task Name      | Core | Priority | Stack | Function                         |
| -------------- | ---- | -------- | ----- | -------------------------------- |
| Core0_Blink    | 0    | 2        | 10KB  | Fast LED blinking (200ms)        |
| Core0_Button   | 0    | 3        | 10KB  | Button 1 monitoring              |
| Core1_Blink    | 1    | 2        | 10KB  | Slow LED blinking (500ms)        |
| Core1_Button   | 1    | 3        | 10KB  | Button 2 monitoring              |
| EventProcessor | Any  | 4        | 10KB  | Process button events from queue |
| SystemMonitor  | Any  | 1        | 10KB  | Display system statistics        |

### Communication Mechanisms

#### 1. **Queue** - Button Events

- **Type:** Button event structure
- **Size:** 10 elements
- **Purpose:** Button tasks send events, EventProcessor receives
- **Direction:** Core 0/1 → Event Processor

#### 2. **Mutexes** - Resource Protection

- **serialMutex:** Protects Serial.print() calls
- **sharedLEDMutex:** Protects shared LED state
- **counterMutex:** Protects shared counter variable

#### 3. **Semaphore** - Synchronization

- **syncSemaphore:** Binary semaphore for task coordination
- **Purpose:** Core 1 blink task signals Monitor task

---

## 🔄 Data Flow Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                         CORE 0                              │
│  ┌──────────────┐              ┌──────────────┐            │
│  │ Blink Task   │──Toggle──────│ Shared LED   │◄─┐         │
│  │  (GPIO 2)    │    (Mutex)   │  (GPIO 5)    │  │         │
│  └──────────────┘              └──────────────┘  │         │
│         │                              ▲          │         │
│         │                              │ Mutex    │         │
│         └────────Increment─────────────┤          │         │
│                 (Counter)              │          │         │
│  ┌──────────────┐              ┌──────┴──────┐   │         │
│  │ Button Task  │──Event────►  │   Queue     │   │         │
│  │  (GPIO 18)   │              │ (10 events) │   │         │
│  └──────────────┘              └─────────────┘   │         │
└──────────────────────────────────────────────────┼─────────┘
                                                    │
                                                    │ Mutex
┌─────────────────────────────────────────────────┼─────────┐
│                         CORE 1                   │         │
│  ┌──────────────┐              ┌────────────┐   │         │
│  │ Blink Task   │──Signal────► │  Semaphore │   │         │
│  │  (GPIO 4)    │              │   (Sync)   │   │         │
│  └──────────────┘              └──────┬─────┘   │         │
│         │                             │          │         │
│         └────────Increment────────────┤          │         │
│                 (Counter)             │          │         │
│  ┌──────────────┐              ┌─────▼──────┐   │         │
│  │ Button Task  │──Event────►  │   Queue    │   │         │
│  │  (GPIO 19)   │              │            │   │         │
│  └──────────────┘              └────┬───────┘   │         │
└───────────────────────────────────┼─┴───────────┼─────────┘
                                    │             │
                    ┌───────────────▼─────────────▼─────────┐
                    │      Event Processor Task             │
                    │   (Runs on available core)            │
                    │  ┌─────────────────────────────┐      │
                    │  │ • Process button events     │      │
                    │  │ • Flash shared LED (Button1)│      │
                    │  │ • Switch active LED (Button2)│     │
                    │  └─────────────────────────────┘      │
                    └────────────────────────────────────────┘
                                    │
                    ┌───────────────▼────────────────────────┐
                    │      System Monitor Task              │
                    │  (Triggered by semaphore)             │
                    │  ┌─────────────────────────────┐      │
                    │  │ • Display task counts       │      │
                    │  │ • Show shared counter       │      │
                    │  │ • Report memory usage       │      │
                    │  └─────────────────────────────┘      │
                    └────────────────────────────────────────┘
```

---

## 🧪 Testing Procedures

### Test 1: Core Affinity Verification

**Steps:**

1. Upload firmware and open Serial Monitor
2. Observe the startup messages showing which core each task is assigned to
3. Verify LED blinking patterns are independent

**Expected Results:**

- LEDs remain off until a button is pressed
- Button 1 starts Core 0 LED blinking
- Button 2 switches blinking to Core 1 LED

---

### Test 2: Inter-Task Communication (Queue)

**Steps:**

1. Press Button 1 (GPIO 18)
2. Observe Serial Monitor output

**Expected Results:**

```
[CORE 0] Button 1 event sent to queue

========== BUTTON EVENT ==========
Button: 1
Press Time: 12345
Detected by Core: 0
Processed by Core: 1
Shared Counter: 42
==================================
```

- Shared LED (GPIO 5) flashes 3 times
- Core 0 LED (GPIO 2) starts blinking
- Event processed by EventProcessor (may run on Core 1)

---

### Test 3: Mutex Protection

**Steps:**

1. Press both buttons rapidly and repeatedly
2. Observe Serial Monitor output

**Expected Results:**

- No corrupted or interleaved Serial messages
- Counter increments correctly without race conditions
- Shared LED state remains consistent

---

### Test 4: Task Synchronization

**Steps:**

1. Wait and observe Serial Monitor
2. Every 5 blinks of Core 1 LED triggers system report

**Expected Results:**

```
----- SYNCHRONIZED EVENT -----
Core 0 Task Count: 123
Core 1 Task Count: 45
Shared Counter: 12
Free Heap: 245678 bytes
------------------------------
```

---

### Test 5: Priority and Scheduling

**Steps:**

1. Press Button 1 while monitoring LEDs
2. Observe response time

**Expected Results:**

- Button is detected immediately despite other running tasks
- Higher priority (3) button tasks respond faster than blink tasks (priority 2)
- Event processor (priority 4) processes events without delay

---

## 🎓 Key Concepts Explained

### 1. Core Affinity

Tasks can be pinned to specific cores for performance optimization:

```cpp
xTaskCreatePinnedToCore(
    TaskFunction,    // Function to execute
    "TaskName",      // Name for debugging
    stackSize,       // Stack size in bytes
    parameters,      // Parameters to pass
    priority,        // Task priority (0-24)
    &taskHandle,     // Task handle
    coreID           // 0 or 1 for Core 0 or Core 1
);
```

**When to use:**

- **Core 0:** Often handles WiFi/Bluetooth stack
- **Core 1:** Arduino loop() runs here by default
- Pin CPU-intensive tasks to spread load

---

### 2. FreeRTOS Queues

Queues provide thread-safe message passing between tasks:

```cpp
// Create queue
QueueHandle_t queue = xQueueCreate(size, itemSize);

// Send to queue (non-blocking)
xQueueSend(queue, &data, 0);

// Receive from queue (blocking)
xQueueReceive(queue, &buffer, portMAX_DELAY);
```

**Advantages:**

- Thread-safe without explicit locking
- FIFO order guaranteed
- Blocking/non-blocking options

---

### 3. Mutexes (Mutual Exclusion)

Mutexes protect shared resources from concurrent access:

```cpp
// Create mutex
SemaphoreHandle_t mutex = xSemaphoreCreateMutex();

// Lock resource
if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
    // Critical section - protected code
    sharedVariable++;

    // Unlock resource
    xSemaphoreGive(mutex);
}
```

**Use cases:**

- Serial port (only one task should print at a time)
- Shared variables
- Hardware peripherals

---

### 4. Semaphores for Synchronization

Binary semaphores coordinate task execution:

```cpp
// Create binary semaphore
SemaphoreHandle_t sem = xSemaphoreCreateBinary();

// Task 1: Signal event
xSemaphoreGive(sem);

// Task 2: Wait for event
xSemaphoreTake(sem, portMAX_DELAY);
```

---

### 5. Task Priorities

Higher priority tasks preempt lower priority tasks:

| Priority | Behavior                        |
| -------- | ------------------------------- |
| 0        | Idle task priority (lowest)     |
| 1-10     | Low priority (background tasks) |
| 11-20    | Medium priority (normal tasks)  |
| 21-24    | High priority (time-critical)   |

**In this lab:**

- Priority 4: Event processor (most important)
- Priority 3: Button monitors (responsive)
- Priority 2: LED blink tasks (regular)
- Priority 1: System monitor (background)

---

## 🔍 Code Highlights

### Thread-Safe Serial Communication

```cpp
void safePrintln(const char* message) {
  if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
    Serial.println(message);
    xSemaphoreGive(serialMutex);
  }
}
```

### Button Event Structure

```cpp
struct ButtonEvent {
  uint8_t buttonNumber;    // Which button was pressed
  unsigned long pressTime;  // When it was pressed
  int coreId;              // Which core detected it
};
```

### Core-Specific Task

```cpp
void Task_Core0_BlinkCode(void * parameter) {
  pinMode(LED_CORE0, OUTPUT);

  for(;;) {
    digitalWrite(LED_CORE0, HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    digitalWrite(LED_CORE0, LOW);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // Additional task logic...
  }
}
```

---

## 🐛 Troubleshooting

### Issue: Tasks Not Running Independently

**Symptoms:**

- LEDs blink in sync
- One core appears idle

**Solutions:**

1. Verify core affinity in task creation (check last parameter)
2. Ensure sufficient stack size (minimum 2048 bytes)
3. Check task priorities are set correctly

---

### Issue: Queue Overflow

**Symptoms:**

- "Queue full! Event dropped" messages
- Button events not processed

**Solutions:**

1. Increase queue size: `xQueueCreate(20, sizeof(ButtonEvent))`
2. Process events faster in EventProcessor
3. Add overflow handling logic

---

### Issue: Mutex Deadlock

**Symptoms:**

- System hangs
- Tasks stop responding

**Solutions:**

1. Always release mutexes with `xSemaphoreGive()`
2. Use timeout instead of `portMAX_DELAY` for debugging
3. Avoid taking multiple mutexes in nested fashion

---

### Issue: Watchdog Timer Reset

**Symptoms:**

- ESP32 reboots unexpectedly
- "Task watchdog" error messages

**Solutions:**

1. Add `vTaskDelay()` in all infinite loops
2. Don't block tasks for too long
3. Increase watchdog timeout if necessary

---

### Issue: Corrupted Serial Output

**Symptoms:**

- Garbled text in Serial Monitor
- Mixed/overlapping messages

**Solutions:**

1. Ensure all Serial calls use `serialMutex`
2. Check baud rate matches (115200)
3. Verify USB cable quality

---

## 📈 Performance Optimization

### 1. Stack Size Tuning

Monitor stack usage:

```cpp
UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(NULL);
Serial.print("Unused stack: ");
Serial.println(highWaterMark);
```

Reduce stack size if usage is low to save memory.

---

### 2. Task Priority Balancing

**Guidelines:**

- Time-critical tasks (buttons): High priority (3-4)
- Regular tasks (LED blinking): Medium priority (2)
- Background tasks (monitoring): Low priority (1)

---

### 3. Core Load Balancing

Distribute tasks evenly:

- Core 0: 2 tasks (blink + button)
- Core 1: 2 tasks (blink + button)
- Shared: 2 tasks (processor + monitor)

---

### 4. Queue Size Optimization

Balance memory vs. responsiveness:

- Small queue (5-10): Low memory, risk of overflow
- Large queue (20+): More memory, handles bursts better

---

## 🌟 Advanced Experiments

### Experiment 1: Add More Sensors

Add a sensor reading task on Core 1:

```cpp
xTaskCreatePinnedToCore(ReadSensor, "Sensor", 4096, NULL, 2, NULL, 1);
```

### Experiment 2: Implement Task Notifications

Replace semaphore with task notifications for faster signaling:

```cpp
xTaskNotifyGive(targetTask);
ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
```

### Experiment 3: CPU Utilization Monitoring

Track task execution time:

```cpp
vTaskGetRunTimeStats(statsBuffer);
Serial.println(statsBuffer);
```

### Experiment 4: Software Timers

Add periodic tasks without dedicated task:

```cpp
TimerHandle_t timer = xTimerCreate("Timer", pdMS_TO_TICKS(1000),
                                    pdTRUE, 0, timerCallback);
xTimerStart(timer, 0);
```

---

## 📚 Additional Resources

### Documentation

- [FreeRTOS API Reference](https://www.freertos.org/a00106.html)
- [ESP32 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/)

### Tutorials

- FreeRTOS Task Management
- ESP32 Dual-Core Architecture
- Real-Time Operating Systems Basics

---

## 📋 Summary

### What You Learned

✅ Created FreeRTOS tasks with core affinity  
✅ Implemented inter-task communication with queues  
✅ Protected shared resources with mutexes  
✅ Synchronized tasks with semaphores  
✅ Optimized task priorities and scheduling  
✅ Monitored multi-core performance  
✅ Handled button events across cores

### Key Takeaways

1. **Core Affinity**: Pin tasks to specific cores for predictable performance
2. **Queues**: Best for passing data between tasks
3. **Mutexes**: Essential for protecting shared resources
4. **Priorities**: Higher priority tasks preempt lower ones
5. **Stack Management**: Monitor and optimize task stack sizes
6. **Synchronization**: Use semaphores for event signaling

---

## 🎯 Next Steps

- **Lab-17:** Explore WiFi + multi-core optimization
- **Lab-18:** Implement BLE on Core 0 + sensor processing on Core 1
- **Lab-19:** Build complete IoT system with multi-core architecture

---

**Happy Multi-Core Programming! 🚀**
