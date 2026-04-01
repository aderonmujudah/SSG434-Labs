# Lab-16 Troubleshooting Guide

## 🔍 Common Issues and Solutions

This guide covers common problems you may encounter with ESP32 dual-core programming and FreeRTOS tasks.

---

## 📑 Table of Contents

1. [Hardware Issues](#hardware-issues)
2. [Upload and Compilation Issues](#upload-and-compilation-issues)
3. [Task Creation and Management](#task-creation-and-management)
4. [Watchdog Timer Resets](#watchdog-timer-resets)
5. [Memory Issues](#memory-issues)
6. [Synchronization Problems](#synchronization-problems)
7. [Serial Communication Issues](#serial-communication-issues)
8. [Performance Problems](#performance-problems)
9. [Core-Specific Issues](#core-specific-issues)

---

## 🔧 Hardware Issues

### Issue 1: LEDs Not Blinking

**Symptoms:**

- No LED activity after upload
- One or more LEDs remain off

**Possible Causes:**

1. **Incorrect wiring**
   - ✓ Check LED polarity (long leg = positive/anode)
   - ✓ Verify resistor values (220-330Ω)
   - ✓ Ensure resistor is in series with LED
   - ✓ Check ground connections

2. **Wrong GPIO pins**

   ```cpp
   // Verify these match your wiring:
   #define LED_CORE0    2
   #define LED_CORE1    4
   #define LED_SHARED   5
   ```

3. **Damaged components**
   - Test LED with multimeter (diode test mode)
   - Test resistor with multimeter (should read 220-330Ω)
   - Try swapping components

**Solutions:**

```
Test Circuit:
ESP32 3.3V ──[220Ω]──LED──GND
                      ↑
               Should light up
```

**Quick Test Code:**

```cpp
void setup() {
   pinMode(2, OUTPUT);
    for(;;) {
      digitalWrite(2, HIGH);
        delay(500);
      digitalWrite(2, LOW);
        delay(500);
    }
}
void loop() {}
```

---

### Issue 2: Buttons Not Responding

**Symptoms:**

- Button presses not detected
- No serial output when pressing buttons

**Possible Causes:**

1. **Incorrect wiring**
   - ✓ Button should connect GPIO to GND when pressed
   - ✓ Internal pull-up is enabled in code (no external resistor needed)

2. **Wrong GPIO configuration**

   ```cpp
   pinMode(BUTTON1, INPUT_PULLUP);  // Must be INPUT_PULLUP
   pinMode(BUTTON2, INPUT_PULLUP);
   ```

3. **Debouncing issues**
   - Very short button presses may be missed
   - Hold button for at least 100ms

**Solutions:**

1. **Test button with multimeter:**

   ```
   Continuity mode:
   - Unpressed: Open circuit (no beep)
   - Pressed: Short circuit (beep)
   ```

2. **Verify button reading:**

   ```cpp
   void setup() {
       Serial.begin(115200);
       pinMode(18, INPUT_PULLUP);
   }

   void loop() {
       Serial.println(digitalRead(18));
       delay(100);
   }
   // Should print: 1 (unpressed), 0 (pressed)
   ```

3. **Check debounce timing:**
   ```cpp
   const unsigned long debounceDelay = 50;  // Increase if needed
   ```

---

## 💻 Upload and Compilation Issues

### Issue 3: Compilation Errors

**Error:** `'xTaskCreatePinnedToCore' was not declared`

**Solution:**

```cpp
// Add at top of file:
#include <Arduino.h>
```

---

**Error:** `'portTICK_PERIOD_MS' undeclared`

**Solution:**

- Ensure ESP32 board is selected
- Go to: **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
- FreeRTOS is built into ESP32 core

---

**Error:** `undefined reference to 'vTaskDelay'`

**Solution:**

- FreeRTOS is automatically included with ESP32
- If error persists, reinstall ESP32 board support:
  ```
  Tools → Board → Boards Manager
  Search "esp32" → Uninstall → Reinstall
  ```

---

### Issue 4: Upload Failures

**Error:** `Failed to connect to ESP32`

**Solutions:**

1. **Hold BOOT button during upload:**
   - Press and hold BOOT button
   - Click Upload
   - Release BOOT when "Connecting..." appears

2. **Check USB cable:**
   - Use a data cable (not charge-only)
   - Try different USB port
   - Try different cable

3. **Verify serial port:**

   ```
   Tools → Port → [Select correct COM port]
   ```

4. **Reduce upload speed:**

   ```
   Tools → Upload Speed → 115200
   ```

5. **Power cycle:**
   - Unplug USB
   - Wait 5 seconds
   - Reconnect

---

## 📋 Task Creation and Management

### Issue 5: Tasks Not Starting

**Symptoms:**

- Serial messages show "Creating tasks..."
- But no "task started" messages
- LEDs don't blink

**Possible Causes:**

1. **Insufficient stack size**

   ```cpp
   // Too small:
   xTaskCreate(MyTask, "Task", 512, NULL, 1, NULL);  // ⚠️ May crash

   // Better:
   xTaskCreate(MyTask, "Task", 4096, NULL, 1, NULL);  // ✓ Safer
   ```

2. **Task function returns (never do this!)**

   ```cpp
   // WRONG:
   void MyTask(void *param) {
       pinMode(13, OUTPUT);
       digitalWrite(13, HIGH);
   }  // ⚠️ Task ends immediately!

   // CORRECT:
   void MyTask(void *param) {
       pinMode(13, OUTPUT);
       for(;;) {  // Infinite loop
           digitalWrite(13, HIGH);
           vTaskDelay(1000 / portTICK_PERIOD_MS);
           digitalWrite(13, LOW);
           vTaskDelay(1000 / portTICK_PERIOD_MS);
       }
   }
   ```

3. **Task creation failed (out of memory)**

   ```cpp
   TaskHandle_t task;
   BaseType_t result = xTaskCreate(MyTask, "Task", 10000, NULL, 1, &task);

   if (result != pdPASS) {
       Serial.println("ERROR: Task creation failed!");
       Serial.print("Free heap: ");
       Serial.println(ESP.getFreeHeap());
   }
   ```

**Solutions:**

1. **Add error checking:**

   ```cpp
   if (xTaskCreatePinnedToCore(...) != pdPASS) {
       Serial.println("Task creation failed!");
       while(1);  // Stop here for debugging
   }
   ```

2. **Monitor task with handle:**

   ```cpp
   TaskHandle_t myTask;
   xTaskCreate(MyTask, "Task", 4096, NULL, 1, &myTask);

   // Later, check if running:
   eTaskState state = eTaskGetState(myTask);
   if (state == eDeleted || state == eInvalid) {
       Serial.println("Task died!");
   }
   ```

---

### Issue 6: Task Priorities Not Working as Expected

**Symptoms:**

- Lower priority task seems to run more often
- High priority task not preempting

**Possible Causes:**

1. **High priority task is blocked:**

   ```cpp
   // High priority, but always waiting:
   void HighPriorityTask(void *param) {
       for(;;) {
           xQueueReceive(queue, &data, portMAX_DELAY);  // Blocked here
           // Only runs when data available
       }
   }
   ```

   - **This is correct behavior!** Blocked tasks don't run.

2. **Same priority tasks round-robin:**

   ```cpp
   xTaskCreate(Task1, "T1", 4096, NULL, 5, NULL);
   xTaskCreate(Task2, "T2", 4096, NULL, 5, NULL);
   // Both priority 5 → share CPU equally
   ```

3. **Pinned to different cores:**
   ```cpp
   xTaskCreatePinnedToCore(Task1, "T1", 4096, NULL, 10, NULL, 0);  // Core 0
   xTaskCreatePinnedToCore(Task2, "T2", 4096, NULL, 5, NULL, 1);   // Core 1
   // Don't compete - different cores!
   ```

**Solutions:**

1. **Verify priorities:**

   ```cpp
   Serial.print("Task priority: ");
   Serial.println(uxTaskPriorityGet(taskHandle));
   ```

2. **List all tasks:**
   ```cpp
   char taskList[512];
   vTaskList(taskList);
   Serial.println(taskList);
   // Shows: Name, State, Priority, Stack, Number
   ```

---

## ⚠️ Watchdog Timer Resets

### Issue 7: ESP32 Keeps Rebooting

**Symptoms:**

- ESP32 resets every few seconds
- Serial output shows:
  ```
  Task watchdog timeout - core 0
  abort() was called at PC 0x... on core 0
  Backtrace: ...
  ```

**Cause:** Task running too long without yielding to scheduler

**Common Mistakes:**

1. **No delay in task:**

   ```cpp
   // BAD:
   void MyTask(void *param) {
       for(;;) {
           doWork();
           // ⚠️ No delay! Starves watchdog task
       }
   }

   // GOOD:
   void MyTask(void *param) {
       for(;;) {
           doWork();
           vTaskDelay(1 / portTICK_PERIOD_MS);  // Even 1ms is enough!
       }
   }
   ```

2. **Long blocking operations:**

   ```cpp
   // BAD:
   while(digitalRead(BUTTON) == HIGH) {
       // ⚠️ Could block forever if button stuck
   }

   // GOOD:
   unsigned long timeout = millis() + 5000;  // 5 second timeout
   while(digitalRead(BUTTON) == HIGH && millis() < timeout) {
       vTaskDelay(10 / portTICK_PERIOD_MS);
   }
   ```

3. **Using `delay()` instead of `vTaskDelay()`:**

   ```cpp
   // BAD in tasks:
   delay(1000);  // ⚠️ Blocks, doesn't yield

   // GOOD:
   vTaskDelay(1000 / portTICK_PERIOD_MS);  // Yields to scheduler
   ```

**Solutions:**

1. **Add minimum delay:**

   ```cpp
   for(;;) {
       work();
       vTaskDelay(1 / portTICK_PERIOD_MS);  // 1ms minimum
   }
   ```

2. **Break long operations:**

   ```cpp
   for(int i = 0; i < 10000; i++) {
       processItem(i);

       if (i % 100 == 0) {
           vTaskDelay(1 / portTICK_PERIOD_MS);  // Yield every 100 iterations
       }
   }
   ```

3. **Disable watchdog (NOT RECOMMENDED):**
   ```cpp
   // Only for debugging!
   disableCore0WDT();
   disableCore1WDT();
   ```

---

## 💾 Memory Issues

### Issue 8: Stack Overflow

**Error:**

```
***ERROR*** A stack overflow in task [TaskName] has been detected.
abort() was called
```

**Cause:** Task used more stack than allocated

**Solutions:**

1. **Increase stack size:**

   ```cpp
   // Was:
   xTaskCreate(MyTask, "Task", 2048, NULL, 1, NULL);

   // Try:
   xTaskCreate(MyTask, "Task", 4096, NULL, 1, NULL);
   // Or even:
   xTaskCreate(MyTask, "Task", 8192, NULL, 1, NULL);
   ```

2. **Monitor stack usage:**

   ```cpp
   void MyTask(void *param) {
       for(;;) {
           doWork();

           // Check stack usage
           UBaseType_t freeStack = uxTaskGetStackHighWaterMark(NULL);
           if (freeStack < 512) {
               Serial.print("WARNING: Low stack! ");
               Serial.println(freeStack);
           }

           vTaskDelay(1000 / portTICK_PERIOD_MS);
       }
   }
   ```

3. **Reduce stack usage:**

   ```cpp
   // AVOID large local arrays:
   void MyTask(void *param) {
       char buffer[4096];  // ⚠️ Uses 4KB of stack!
       // ...
   }

   // BETTER - allocate on heap:
   void MyTask(void *param) {
       char* buffer = (char*)malloc(4096);
       if (buffer != NULL) {
           // Use buffer
           free(buffer);
       }
   }

   // BEST - static allocation:
   static char buffer[4096];  // Outside task, in global scope
   ```

---

### Issue 9: Heap Exhaustion

**Symptoms:**

- Task creation fails
- `malloc()` returns NULL
- ESP32 crashes

**Diagnosis:**

```cpp
void checkMemory() {
    Serial.print("Free heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.print(" bytes (");
    Serial.print(ESP.getHeapSize());
    Serial.println(" total)");

    Serial.print("Largest block: ");
    Serial.println(ESP.getMaxAllocHeap());
}
```

**Solutions:**

1. **Reduce stack sizes:**

   ```cpp
   // Audit all tasks:
   xTaskCreate(Task1, "T1", 10000, ...);  // Do I really need 10KB?

   // Optimize:
   xTaskCreate(Task1, "T1", 4096, ...);   // 4KB often enough
   ```

2. **Free unused memory:**

   ```cpp
   void MyTask(void *param) {
       char* buffer = (char*)malloc(1024);

       doWork(buffer);

       free(buffer);  // ✓ Don't forget!
       buffer = NULL;
   }
   ```

3. **Reduce number of tasks:**

   ```cpp
   // Instead of 10 tasks doing similar work:
   for(int i = 0; i < 10; i++) {
       xTaskCreate(SensorTask, name, 4096, ...);  // 10 * 4KB = 40KB!
   }

   // Use one task processing queue:
   xTaskCreate(ProcessQueue, "Processor", 4096, ...);  // Just 4KB
   ```

---

## 🔒 Synchronization Problems

### Issue 10: Race Conditions / Data Corruption

**Symptoms:**

- Shared counter has wrong value
- Serial output is garbled
- Intermittent strange behavior

**Cause:** Multiple tasks accessing shared resource without protection

**Example:**

```cpp
// WRONG:
int sharedCounter = 0;

void Task1(void *param) {
    for(;;) {
        sharedCounter++;  // ⚠️ NOT ATOMIC!
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void Task2(void *param) {
    for(;;) {
        sharedCounter++;  // ⚠️ RACE CONDITION!
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
```

**What happens:**

```
Task 1 reads: counter = 5
(Context switch to Task 2)
Task 2 reads: counter = 5
Task 2 writes: counter = 6
(Context switch to Task 1)
Task 1 writes: counter = 6  ← Should be 7!
```

**Solutions:**

1. **Use mutex:**

   ```cpp
   // CORRECT:
   SemaphoreHandle_t counterMutex;
   int sharedCounter = 0;

   void setup() {
       counterMutex = xSemaphoreCreateMutex();
   }

   void incrementCounter() {
       if (xSemaphoreTake(counterMutex, portMAX_DELAY) == pdTRUE) {
           sharedCounter++;
           xSemaphoreGive(counterMutex);
       }
   }
   ```

2. **Use atomic operations (for simple types):**

   ```cpp
   #include <atomic>
   std::atomic<int> sharedCounter(0);

   void Task1(void *param) {
       for(;;) {
           sharedCounter++;  // ✓ Thread-safe
           vTaskDelay(10 / portTICK_PERIOD_MS);
       }
   }
   ```

---

### Issue 11: Deadlock

**Symptoms:**

- System hangs
- Tasks stop responding
- No output

**Cause:** Two tasks waiting for each other's resources

**Example:**

```cpp
SemaphoreHandle_t mutexA, mutexB;

// Task 1:
xSemaphoreTake(mutexA, portMAX_DELAY);  // Gets A
xSemaphoreTake(mutexB, portMAX_DELAY);  // Waits for B (Task 2 has it)

// Task 2:
xSemaphoreTake(mutexB, portMAX_DELAY);  // Gets B
xSemaphoreTake(mutexA, portMAX_DELAY);  // Waits for A (Task 1 has it)

// ⚠️ DEADLOCK! Both waiting forever
```

**Prevention:**

1. **Always lock in same order:**

   ```cpp
   // Both tasks:
   xSemaphoreTake(mutexA, portMAX_DELAY);  // Always A first
   xSemaphoreTake(mutexB, portMAX_DELAY);  // Then B
   // Work...
   xSemaphoreGive(mutexB);
   xSemaphoreGive(mutexA);
   ```

2. **Use timeout:**

   ```cpp
   if (xSemaphoreTake(mutexA, pdMS_TO_TICKS(1000)) != pdTRUE) {
       Serial.println("Timeout! Possible deadlock");
       // Handle error
   }
   ```

3. **Minimize critical sections:**

   ```cpp
   // BAD - Long critical section:
   xSemaphoreTake(mutex, portMAX_DELAY);
   longOperation();          // Others wait a long time
   anotherLongOperation();
   xSemaphoreGive(mutex);

   // GOOD - Short critical section:
   prepareData();            // Outside mutex
   xSemaphoreTake(mutex, portMAX_DELAY);
   quickUpdate();            // Quick operation
   xSemaphoreGive(mutex);
   processResults();         // Outside mutex
   ```

---

## 📡 Serial Communication Issues

### Issue 12: Garbled Serial Output

**Symptoms:**

- Mixed/overlapping text
- Random characters
- Unreadable messages

**Cause:** Multiple tasks printing simultaneously without protection

**Solution:**

```cpp
SemaphoreHandle_t serialMutex;

void setup() {
    Serial.begin(115200);
    serialMutex = xSemaphoreCreateMutex();
}

void safePrint(const char* message) {
    if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
        Serial.print(message);
        xSemaphoreGive(serialMutex);
    }
}

// Use safePrint() instead of Serial.print() in tasks
```

---

### Issue 13: Serial Monitor Shows Nothing

**Checks:**

1. **Verify baud rate:**

   ```cpp
   Serial.begin(115200);  // Must match Serial Monitor setting
   ```

2. **Add startup delay:**

   ```cpp
   void setup() {
       Serial.begin(115200);
       delay(1000);  // Wait for Serial to initialize
       Serial.println("Starting...");
   }
   ```

3. **Check USB connection:**
   - Try different COM port
   - USB cable must support data (not charge-only)

4. **Reset ESP32:**
   - Press EN/RST button
   - Close and reopen Serial Monitor

---

## 🚀 Performance Problems

### Issue 14: Tasks Running Slower Than Expected

**Diagnosis:**

1. **Check task execution time:**

   ```cpp
   void MyTask(void *param) {
       for(;;) {
           unsigned long start = micros();

           doWork();

           unsigned long duration = micros() - start;
           Serial.print("Execution time: ");
           Serial.print(duration);
           Serial.println(" μs");

           vTaskDelay(100 / portTICK_PERIOD_MS);
       }
   }
   ```

2. **Monitor CPU usage:**
   ```cpp
   char statsBuffer[512];
   vTaskGetRunTimeStats(statsBuffer);
   Serial.println(statsBuffer);
   ```

**Common Causes:**

1. **Low priority getting starved:**

   ```cpp
   // Increase priority:
   vTaskPrioritySet(taskHandle, 5);  // Was 1, now 5
   ```

2. **Core overloaded:**

   ```cpp
   // Move task to other core:
   xTaskCreatePinnedToCore(Task, "Task", 4096, NULL, 2, NULL, 1);  // Core 1
   ```

3. **Blocking on I/O:**
   ```cpp
   // Use non-blocking alternatives:
   if (Serial.available()) {
       char c = Serial.read();  // Non-blocking
   }
   ```

---

### Issue 15: Uneven Core Load

**Symptoms:**

- One core at 100%, other idle
- Performance not as expected

**Diagnosis:**

```cpp
void MonitorTask(void *param) {
    for(;;) {
        Serial.print("Task running on core: ");
        Serial.println(xPortGetCoreID());

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
```

**Solutions:**

1. **Balance task distribution:**

   ```cpp
   // Before (all on Core 1):
   xTaskCreatePinnedToCore(Task1, "T1", 4096, NULL, 2, NULL, 1);
   xTaskCreatePinnedToCore(Task2, "T2", 4096, NULL, 2, NULL, 1);
   xTaskCreatePinnedToCore(Task3, "T3", 4096, NULL, 2, NULL, 1);

   // After (distributed):
   xTaskCreatePinnedToCore(Task1, "T1", 4096, NULL, 2, NULL, 0);  // Core 0
   xTaskCreatePinnedToCore(Task2, "T2", 4096, NULL, 2, NULL, 1);  // Core 1
   xTaskCreate(Task3, "T3", 4096, NULL, 2, NULL);  // Either core
   ```

2. **Let scheduler decide:**
   ```cpp
   // Don't pin if no reason:
   xTaskCreate(FlexibleTask, "Flex", 4096, NULL, 2, NULL);
   ```

---

## 🖥️ Core-Specific Issues

### Issue 16: WiFi Interferes with Core 0 Tasks

**Symptoms:**

- Core 0 tasks lag when WiFi active
- Jittery performance

**Cause:** WiFi stack runs on Core 0 by default

**Solutions:**

1. **Move time-critical tasks to Core 1:**

   ```cpp
   xTaskCreatePinnedToCore(RealTimeTask, "RT", 4096, NULL, 3, NULL, 1);
   ```

2. **Reduce WiFi overhead:**

   ```cpp
   WiFi.setSleep(true);  // Enable WiFi power saving
   ```

3. **Lower WiFi task priority:**
   ```cpp
   // WiFi internal tasks typically priority 23
   // Make your critical tasks priority 24
   xTaskCreatePinnedToCore(Critical, "Crit", 4096, NULL, 24, NULL, 0);
   ```

---

## 📊 Debugging Tools

### Useful Functions

```cpp
// Task information
char taskList[512];
vTaskList(taskList);
Serial.println(taskList);

// Runtime statistics
char statsBuffer[512];
vTaskGetRunTimeStats(statsBuffer);
Serial.println(statsBuffer);

// Task state
eTaskState state = eTaskGetState(taskHandle);
// eRunning, eReady, eBlocked, eSuspended, eDeleted

// Current core
int core = xPortGetCoreID();

// Free stack
UBaseType_t freeStack = uxTaskGetStackHighWaterMark(taskHandle);

// Task priority
UBaseType_t priority = uxTaskPriorityGet(taskHandle);

// Heap info
size_t freeHeap = ESP.getFreeHeap();
size_t heapSize = ESP.getHeapSize();
size_t maxBlock = ESP.getMaxAllocHeap();
```

---

## 🆘Getting More Help

If your issue isn't covered here:

1. **Enable debug output:**

   ```
   Tools → Core Debug Level→ Debug
   ```

2. **Check ESP32 forums:**
   - [ESP32 Forum](https://esp32.com)
   - [Arduino Forum](https://forum.arduino.cc)

3. **Review FreeRTOS docs:**
   - [FreeRTOS.org](https://www.freertos.org)

4. **Examine example code:**
   ```
   File → Examples → ESP32 → FreeRTOS
   ```

---

## ✅ Prevention Checklist

Before asking for help, verify:

- [ ] Correct ESP32 board selected in Tools menu
- [ ] Firmware uploads successfully
- [ ] Serial Monitor baud rate is 115200
- [ ] All tasks have infinite loops with delays
- [ ] Stack sizes are reasonable (4096+ bytes)
- [ ] Shared resources protected by mutexes
- [ ] No `delay()` in tasks (use `vTaskDelay()`)
- [ ] USB cable supports data transfer
- [ ] LEDs are correct polarity
- [ ] Latest ESP32 core installed

---

**Still stuck? Check [CORE_CONCEPTS.md](CORE_CONCEPTS.md) for deeper understanding or [README.md](README.md) for complete documentation!**
