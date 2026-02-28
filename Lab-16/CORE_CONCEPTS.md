# ESP32 Dual-Core Architecture - Core Concepts

## 📖 Table of Contents

1. [ESP32 Architecture Overview](#esp32-architecture-overview)
2. [FreeRTOS Fundamentals](#freertos-fundamentals)
3. [Core Affinity and Task Pinning](#core-affinity-and-task-pinning)
4. [Inter-Task Communication](#inter-task-communication)
5. [Synchronization Mechanisms](#synchronization-mechanisms)
6. [Task Scheduling and Priorities](#task-scheduling-and-priorities)
7. [Memory Management](#memory-management)
8. [Best Practices](#best-practices)
9. [Performance Optimization](#performance-optimization)

---

## 🏗️ ESP32 Architecture Overview

### Dual-Core Processor

The ESP32 features a **dual-core Xtensa LX6 microprocessor** with:

```
┌─────────────────────────────────────────────┐
│              ESP32 SoC                      │
│                                             │
│  ┌────────────┐         ┌────────────┐    │
│  │   Core 0   │         │   Core 1   │    │
│  │  (PRO_CPU) │         │  (APP_CPU) │    │
│  │            │         │            │    │
│  │ 240 MHz    │         │ 240 MHz    │    │
│  └─────┬──────┘         └─────┬──────┘    │
│        │                      │            │
│        └──────────┬───────────┘            │
│                   │                        │
│        ┌──────────▼──────────┐            │
│        │   Shared Memory     │            │
│        │   (320 KB SRAM)     │            │
│        └─────────────────────┘            │
│                                             │
│  ┌──────────────────────────────────────┐ │
│  │     Peripherals (WiFi, BLE, I2C,    │ │
│  │     SPI, UART, GPIO, etc.)          │ │
│  └──────────────────────────────────────┘ │
└─────────────────────────────────────────────┘
```

### Core Characteristics

| Feature             | Core 0 (PRO_CPU)            | Core 1 (APP_CPU)            |
| ------------------- | --------------------------- | --------------------------- |
| **Purpose**         | Protocol operations         | Application code            |
| **Default Use**     | WiFi/BLE stack              | Arduino `loop()`            |
| **Clock Speed**     | 240 MHz (default)           | 240 MHz (default)           |
| **Instruction Set** | Xtensa LX6                  | Xtensa LX6                  |
| **FPU**             | Yes                         | Yes                         |
| **Cache**           | 16 KB I-Cache, 8 KB D-Cache | 16 KB I-Cache, 8 KB D-Cache |

### Memory Architecture

**Shared Resources:**

- **SRAM:** 320 KB shared between cores
- **Flash:** Up to 16 MB (external, memory-mapped)
- **ROM:** 448 KB (bootloader, crypto functions)

**Key Point:** All RAM is shared - proper synchronization is essential!

---

## ⚙️ FreeRTOS Fundamentals

### What is FreeRTOS?

**FreeRTOS** (Free Real-Time Operating System) is a real-time kernel that provides:

- **Multitasking**: Run multiple tasks concurrently
- **Task Scheduling**: Priority-based preemptive scheduler
- **Inter-Task Communication**: Queues, semaphores, mutexes
- **Memory Management**: Dynamic allocation for tasks and objects

### Task Anatomy

A FreeRTOS task on ESP32:

```cpp
void MyTask(void * parameter) {
    // Initialization (runs once)
    pinMode(LED_PIN, OUTPUT);
    int counter = 0;

    // Main loop (runs forever)
    for(;;) {
        // Task work
        digitalWrite(LED_PIN, HIGH);
        counter++;

        // MUST include delay to yield to other tasks
        vTaskDelay(100 / portTICK_PERIOD_MS);

        digitalWrite(LED_PIN, LOW);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    // Tasks should never return!
    // If they do, must call vTaskDelete(NULL)
}
```

**Critical Rule:** Every task must include `vTaskDelay()` or blocking operation to prevent watchdog resets!

### Task States

```
┌──────────────┐
│   RUNNING    │◄─── Currently executing on CPU
└──────┬───────┘
       │
       ├─► vTaskDelay() ──┐
       │                   │
       ├─► Blocked on I/O ─┼──► ┌────────────┐
       │                   └────►│  BLOCKED   │
       │                         └────────────┘
       │                                │
       └─► Ready to run ──────┐         │ Timeout/Unblock
                               │         │
                        ┌──────▼─────────▼────┐
                        │      READY          │
                        └─────────────────────┘
                                 ▲
                                 │
                          Scheduler selects
                          highest priority
```

---

## 🎯 Core Affinity and Task Pinning

### Why Pin Tasks to Cores?

**Advantages:**

1. **Predictable Performance**: Task always runs on same core
2. **Cache Optimization**: Better cache hit rates
3. **Load Balancing**: Distribute work evenly
4. **Isolation**: Separate time-critical from background tasks

**Disadvantages:**

1. **Inflexibility**: Can't adapt to changing loads
2. **Complexity**: Must manually balance cores

### Creating Pinned Tasks

```cpp
TaskHandle_t myTask;

// Pinned to Core 0
xTaskCreatePinnedToCore(
    MyTaskFunction,     // Function pointer
    "MyTask",           // Name (for debugging)
    10000,              // Stack size in bytes
    NULL,               // Parameters to pass
    2,                  // Priority (0-24)
    &myTask,            // Task handle
    0                   // Core ID: 0 or 1
);

// NOT pinned (can run on either core)
xTaskCreate(
    AnotherTask,        // No core specified
    "FlexibleTask",     // Scheduler decides
    4096,
    NULL,
    1,
    NULL
);
```

### Core Selection Strategy

**Core 0:** Best for:

- WiFi/BLE communication (already handles protocol stack)
- Network I/O tasks
- Fast, time-critical operations

**Core 1:** Best for:

- Sensor reading
- Data processing
- User interface tasks
- Arduino compatibility (loop runs here)

**Either Core:** Best for:

- Event processing
- Background tasks
- Flexible workloads

### Example: Load Balancing

```cpp
// Core 0: Fast sensor polling
xTaskCreatePinnedToCore(ReadAccelerometer, "Accel", 4096, NULL, 3, NULL, 0);
xTaskCreatePinnedToCore(PublishMQTT, "MQTT", 8192, NULL, 3, NULL, 0);

// Core 1: Data processing
xTaskCreatePinnedToCore(ProcessAI, "AI", 16384, NULL, 2, NULL, 1);
xTaskCreatePinnedToCore(UpdateDisplay, "Display", 4096, NULL, 2, NULL, 1);

// Either: Background maintenance
xTaskCreate(LoggingTask, "Logger", 4096, NULL, 1, NULL);
```

---

## 📡 Inter-Task Communication

### 1. Queues

**Purpose:** Pass data between tasks (producer-consumer pattern)

**Characteristics:**

- Thread-safe (no mutex needed)
- FIFO order
- Blocking/non-blocking options
- Copy data (not pointers)

```cpp
// Create queue
QueueHandle_t dataQueue;
dataQueue = xQueueCreate(10, sizeof(int));  // 10 items, int-sized

// Producer task
void ProducerTask(void *param) {
    for(;;) {
        int data = readSensor();

        // Send (non-blocking with 0 timeout)
        if (xQueueSend(dataQueue, &data, 0) != pdTRUE) {
            Serial.println("Queue full!");
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// Consumer task
void ConsumerTask(void *param) {
    int receivedData;

    for(;;) {
        // Wait indefinitely for data
        if (xQueueReceive(dataQueue, &receivedData, portMAX_DELAY) == pdTRUE) {
            processData(receivedData);
        }
    }
}
```

**When to Use:**

- ✅ Passing sensor data
- ✅ Button events
- ✅ Command messages
- ❌ Large data (use pointers + mutex instead)
- ❌ Simple flags (use task notifications)

---

### 2. Shared Variables

**Problem:** Race conditions without protection!

```cpp
// WRONG - Race condition!
volatile int sharedCounter = 0;

void Task1(void *param) {
    for(;;) {
        sharedCounter++;  // ⚠️ NOT ATOMIC!
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void Task2(void *param) {
    for(;;) {
        sharedCounter++;  // ⚠️ NOT ATOMIC!
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// What happens:
// Task 1 reads 5, increments to 6
// (Context switch!)
// Task 2 reads 5, increments to 6
// Both write 6 instead of 7!
```

**Solution:** Use mutex protection (see below)

---

### 3. Task Notifications

**Purpose:** Lightweight signaling between tasks

**Advantages:**

- Very fast (no queue overhead)
- Low memory usage
- Built into task control block

**Limitations:**

- Only one notification value per task
- No data payload
- Point-to-point only

```cpp
TaskHandle_t targetTask;

// Sender
xTaskNotifyGive(targetTask);  // Increment notification value

// Receiver
uint32_t notificationValue = ulTaskNotifyTake(
    pdTRUE,          // Clear value after reading
    portMAX_DELAY    // Wait forever
);
```

**Use Cases:**

- ISR → Task signaling
- Simple event flags
- Lightweight synchronization

---

## 🔒 Synchronization Mechanisms

### 1. Mutexes (Mutual Exclusion)

**Purpose:** Protect shared resources from concurrent access

```cpp
SemaphoreHandle_t serialMutex;

void setup() {
    serialMutex = xSemaphoreCreateMutex();
}

void safePrint(const char* msg) {
    // Lock (wait forever if needed)
    if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
        // Critical section - only one task here at a time
        Serial.print(msg);

        // Unlock
        xSemaphoreGive(serialMutex);
    }
}
```

**Key Concepts:**

| Concept                  | Description                                    |
| ------------------------ | ---------------------------------------------- |
| **Critical Section**     | Code that accesses shared resource             |
| **Lock**                 | `xSemaphoreTake()` - wait for exclusive access |
| **Unlock**               | `xSemaphoreGive()` - release for others        |
| **Blocking**             | Waits if resource already locked               |
| **Priority Inheritance** | Prevents priority inversion                    |

**Protected Resources:**

- ✅ Serial port (only one print at a time)
- ✅ Shared variables (counters, flags)
- ✅ Hardware peripherals (I2C, SPI)
- ✅ File systems
- ❌ Fast operations (use atomics instead)

**Deadlock Prevention:**

```cpp
// BAD - Can deadlock!
void Task1() {
    xSemaphoreTake(mutexA, portMAX_DELAY);
    xSemaphoreTake(mutexB, portMAX_DELAY);  // Wait for B
    // ...
}

void Task2() {
    xSemaphoreTake(mutexB, portMAX_DELAY);
    xSemaphoreTake(mutexA, portMAX_DELAY);  // Wait for A (deadlock!)
    // ...
}

// GOOD - Always lock in same order
void BothTasks() {
    xSemaphoreTake(mutexA, portMAX_DELAY);  // Always A first
    xSemaphoreTake(mutexB, portMAX_DELAY);  // Then B
    // ...
    xSemaphoreGive(mutexB);
    xSemaphoreGive(mutexA);
}
```

---

### 2. Binary Semaphores

**Purpose:** Signal events between tasks

```cpp
SemaphoreHandle_t eventSemaphore;

void setup() {
    eventSemaphore = xSemaphoreCreateBinary();
}

// Task 1: Wait for event
void WaitTask(void *param) {
    for(;;) {
        // Block until signaled
        if (xSemaphoreTake(eventSemaphore, portMAX_DELAY) == pdTRUE) {
            Serial.println("Event received!");
            handleEvent();
        }
    }
}

// Task 2: Signal event
void SignalTask(void *param) {
    for(;;) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        xSemaphoreGive(eventSemaphore);  // Wake up waiting task
    }
}
```

**Mutex vs. Binary Semaphore:**

| Feature                  | Mutex                     | Binary Semaphore     |
| ------------------------ | ------------------------- | -------------------- |
| **Purpose**              | Resource protection       | Event signaling      |
| **Ownership**            | Yes (only taker can give) | No (anyone can give) |
| **Priority Inheritance** | Yes                       | No                   |
| **Recursive**            | Yes                       | No                   |
| **Initial State**        | Available (1)             | Not available (0)    |

---

### 3. Counting Semaphores

**Purpose:** Track available resources (e.g., buffer slots)

```cpp
SemaphoreHandle_t bufferSemaphore;

void setup() {
    bufferSemaphore = xSemaphoreCreateCounting(5, 5);  // Max 5, start at 5
}

void ProducerTask(void *param) {
    for(;;) {
        // Wait for free buffer slot
        if (xSemaphoreTake(bufferSemaphore, portMAX_DELAY) == pdTRUE) {
            writeToBuffer();
        }
    }
}

void ConsumerTask(void *param) {
    for(;;) {
        readFromBuffer();
        xSemaphoreGive(bufferSemaphore);  // Free a slot
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
```

---

## ⏱️ Task Scheduling and Priorities

### Priority Levels

ESP32 FreeRTOS priorities: **0 (lowest) to 24 (highest)**

```
Priority 24  ├──► Critical (interrupt-like)
Priority 20  ├──► Very High (time-critical operations)
Priority 15  ├──► High (button handlers, fast I/O)
Priority 10  ├──► Medium (sensor reading, processing)
Priority 5   ├──► Low (logging, background tasks)
Priority 1   ├──► Idle work (cleanup, statistics)
Priority 0   └──► Idle task (DO NOT USE)
```

### Preemptive Scheduling

Higher priority tasks **always** preempt lower priority:

```
Timeline:
┌─────────────────────────────────────────────────┐
│ Low Priority Task (P1) ------------------------►│
│         ▲         │                              │
│         │         │ High Priority Ready          │
│         │         ▼                              │
│         │  ┌──────────────┐                      │
│         │  │ High (P5)    │◄─── Preempts       │
│         │  └──────────────┘                      │
│         │         │                              │
│         │         │ Blocks/Completes             │
│         │         ▼                              │
│ Low Priority Resumes ──────────────────────────►│
└─────────────────────────────────────────────────┘
```

### Round-Robin for Equal Priorities

Tasks with same priority share CPU time:

```
Priority 2: Task A ──┬──► 1ms ──┬──► 1ms ──┬──►
Priority 2: Task B ──┼──────────┼──► 1ms ──┼──►
Priority 2: Task C ──┼──────────┼──────────┼──► 1ms
                     │          │          │
                   Time slicing (configurable)
```

### Task Delays

**`vTaskDelay()` vs `delay()`:**

```cpp
// vTaskDelay() - Yields to scheduler (GOOD)
void Task1(void *param) {
    for(;;) {
        doWork();
        vTaskDelay(100 / portTICK_PERIOD_MS);  // Other tasks run
    }
}

// delay() - Blocks entire core (BAD for tasks)
void Task2(void *param) {
    for(;;) {
        doWork();
        delay(100);  // ⚠️ Wastes CPU, delays watchdog feed
    }
}
```

**Use `vTaskDelay()` in tasks, `delay()` only in `setup()`!**

---

## 💾 Memory Management

### Stack Allocation

Each task has its own stack:

```cpp
xTaskCreate(
    MyTask,
    "Task",
    4096,      // ◄── Stack size in BYTES
    NULL,
    1,
    NULL
);
```

**Stack Size Guidelines:**

| Task Type          | Typical Stack | Notes                    |
| ------------------ | ------------- | ------------------------ |
| Simple LED blink   | 2048          | Minimal local variables  |
| Button handler     | 2048-4096     | Includes debounce logic  |
| WiFi operations    | 4096-8192     | Network buffers          |
| JSON parsing       | 4096-8192     | Temporary string storage |
| Complex processing | 8192-16384    | Large arrays, recursion  |

**Monitor Stack Usage:**

```cpp
void MonitorTask(void *param) {
    for(;;) {
        UBaseType_t freeStack = uxTaskGetStackHighWaterMark(NULL);
        Serial.print("Unused stack: ");
        Serial.print(freeStack);
        Serial.println(" bytes");

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
```

**Stack Overflow Protection:**

```
// Enable in Arduino IDE:
Tools → Core Debug Level → Debug
Tools → Stack Smashing Protection → Enable
```

---

### Heap Memory

**Available:**

- DRAM: ~160 KB (varies by configuration)
- PSRAM: Up to 4 MB (if available on board)

**Check Free Heap:**

```cpp
size_t freeHeap = ESP.getFreeHeap();
size_t maxBlock = ESP.getMaxAllocHeap();

Serial.print("Free: ");
Serial.print(freeHeap);
Serial.print(" bytes, Largest block: ");
Serial.println(maxBlock);
```

---

## ✅ Best Practices

### 1. Core Assignment Strategy

```cpp
// WiFi-intensive tasks on Core 0 (shares with WiFi stack)
xTaskCreatePinnedToCore(MQTTPublish, "MQTT", 8192, NULL, 3, NULL, 0);
xTaskCreatePinnedToCore(WebServer, "HTTP", 8192, NULL, 2, NULL, 0);

// Data processing on Core 1 (isolated from WiFi)
xTaskCreatePinnedToCore(ImageProcess, "Image", 16384, NULL, 2, NULL, 1);
xTaskCreatePinnedToCore(AI_Inference, "AI", 20480, NULL, 2, NULL, 1);

// Flexible background tasks (no affinity)
xTaskCreate(SystemMonitor, "Monitor", 4096, NULL, 1, NULL);
```

---

### 2. Priority Assignment

**Guideline:** Higher priority = more time-critical

```cpp
// Critical: Hardware interrupt processing
xTaskCreate(InterruptHandler, "ISR", 4096, NULL, 24, NULL);

// High: User responsiveness
xTaskCreate(ButtonHandler, "Button", 4096, NULL, 20, NULL);

// Medium: Active data processing
xTaskCreate(SensorRead, "Sensor", 4096, NULL, 10, NULL);

// Low: Background maintenance
xTaskCreate(Logger, "Log", 4096, NULL, 3, NULL);

// Lowest: Statistics, idle work
xTaskCreate(Stats, "Stats", 4096, NULL, 1, NULL);
```

---

### 3. Mutex Usage Patterns

```cpp
// Pattern 1: Simple lock
if (xSemaphoreTake(mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
    criticalOperation();
    xSemaphoreGive(mutex);
}

// Pattern 2: Early return handling
bool success = false;
if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
    if (condition) {
        doWork();
        success = true;
    }
    xSemaphoreGive(mutex);  // Always unlock!
}
return success;

// Pattern 3: Wrapper function
void safeOperation() {
    xSemaphoreTake(mutex, portMAX_DELAY);
    unsafeOperation();
    xSemaphoreGive(mutex);
}
```

---

### 4. Queue Sizing

**Considerations:**

- Too small: Events dropped
- Too large: Wasted RAM

```cpp
// Fast producer, slow consumer: Larger queue
QueueHandle_t sensorData = xQueueCreate(50, sizeof(SensorReading));

// Infrequent events: Small queue
QueueHandle_t buttonEvents = xQueueCreate(5, sizeof(ButtonEvent));

// Real-time responsive: Tiny queue
QueueHandle_t commands = xQueueCreate(1, sizeof(Command));
```

---

## 🚀 Performance Optimization

### 1. Task Priorities for Balanced Load

```cpp
// Equal priorities = time-sliced
xTaskCreatePinnedToCore(LED1_Blink, "LED1", 2048, NULL, 2, NULL, 0);
xTaskCreatePinnedToCore(LED2_Blink, "LED2", 2048, NULL, 2, NULL, 0);
// Each gets 50% of Core 0

// Unequal priorities = dominant task
xTaskCreatePinnedToCore(Critical, "Crit", 4096, NULL, 10, NULL, 1);
xTaskCreatePinnedToCore(Background, "BG", 4096, NULL, 1, NULL, 1);
// Critical preempts Background frequently
```

---

### 2. Avoid Busy-Waiting

```cpp
// BAD - Wastes CPU
void BadTask(void *param) {
    for(;;) {
        if (dataReady) {  // ⚠️ Polling constantly
            processData();
        }
        // No delay!
    }
}

// GOOD - Event-driven
void GoodTask(void *param) {
    for(;;) {
        if (xQueueReceive(dataQueue, &data, portMAX_DELAY) == pdTRUE) {
            processData(data);
        }
        // Blocks when no data, other tasks run
    }
}
```

---

### 3. Cache-Friendly Code

**Principle:** Keep frequently-accessed data together

```cpp
// BAD - Poor cache locality
int array1[1000];
int array2[1000];

void process() {
    for(int i = 0; i < 1000; i++) {
        array2[i] = array1[i] * 2;  // Jumps between arrays
    }
}

// GOOD - Sequential access
struct DataPair {
    int input;
    int output;
};
DataPair data[1000];

void process() {
    for(int i = 0; i < 1000; i++) {
        data[i].output = data[i].input * 2;  // Sequential
    }
}
```

---

### 4. Monitoring Performance

```cpp
void PerformanceTask(void *param) {
    for(;;) {
        // CPU frequency
        Serial.print("CPU: ");
        Serial.print(getCpuFrequencyMhz());
        Serial.println(" MHz");

        // Task stats
        char statsBuffer[512];
        vTaskGetRunTimeStats(statsBuffer);
        Serial.println(statsBuffer);

        // Heap
        Serial.print("Free heap: ");
        Serial.println(ESP.getFreeHeap());

        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
```

---

## 📚 Summary

### Key Takeaways

1. **Core Affinity**: Pin time-critical tasks to specific cores
2. **Priorities**: Higher priority always preempts lower
3. **Queues**: Best for data passing between tasks
4. **Mutexes**: Essential for shared resource protection
5. **Delays**: Always use `vTaskDelay()` in tasks, not `delay()`
6. **Stack**: Monitor usage, allocate conservatively
7. **Balance**: Distribute tasks evenly across cores

### Decision Matrix

| Scenario             | Solution                   |
| -------------------- | -------------------------- |
| Pass sensor data     | Queue                      |
| Protect Serial port  | Mutex                      |
| Signal event         | Binary semaphore           |
| Wake task from ISR   | Task notification          |
| Track resource count | Counting semaphore         |
| Time-critical task   | High priority + Core 0     |
| Background task      | Low priority + Either core |

---

**Ready to implement? Return to [README.md](README.md) for practical examples!**
