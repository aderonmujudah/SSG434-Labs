/*
 * ESP32 Dual-Core Programming Demo
 * Lab-16: FreeRTOS Multi-Core Task Management
 * 
 * Hardware Setup:
 * - LED 1 (Core 0): GPIO 13
 * - LED 2 (Core 1): GPIO 12
 * - LED 3 (Shared): GPIO 14
 * - Button 1: GPIO 18 (with internal pull-up)
 * - Button 2: GPIO 19 (with internal pull-up)
 * 
 * Features:
 * - Tasks pinned to specific cores
 * - Inter-task communication with queues
 * - Mutex for shared resource protection
 * - Task synchronization with semaphores
 * - Core affinity demonstration
 * - Multi-core performance optimization
 */

#include <Arduino.h>

// Pin Definitions
#define LED_CORE0    13    // LED controlled by Core 0
#define LED_CORE1    12    // LED controlled by Core 1
#define LED_SHARED   14    // LED shared between cores (mutex protected)
#define BUTTON1      18    // Button for Core 0
#define BUTTON2      19    // Button for Core 1

// Task Handles
TaskHandle_t Task_Core0_Blink;
TaskHandle_t Task_Core0_Button;
TaskHandle_t Task_Core1_Blink;
TaskHandle_t Task_Core1_Button;
TaskHandle_t Task_EventProcessor;
TaskHandle_t Task_Monitor;

// Queue Handle for Button Events
QueueHandle_t buttonQueue;

// Mutex for shared resources
SemaphoreHandle_t serialMutex;
SemaphoreHandle_t sharedLEDMutex;
SemaphoreHandle_t counterMutex;

// Semaphore for task synchronization
SemaphoreHandle_t syncSemaphore;

// Shared variables (protected by mutex)
volatile int sharedCounter = 0;
volatile bool sharedLEDState = false;

// Button event structure
struct ButtonEvent {
  uint8_t buttonNumber;
  unsigned long pressTime;
  int coreId;
};

// Button debounce variables
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
const unsigned long debounceDelay = 50;
bool lastButtonState1 = HIGH;
bool lastButtonState2 = HIGH;

// Statistics tracking
volatile unsigned long core0TaskCount = 0;
volatile unsigned long core1TaskCount = 0;

//=============================================================================
// UTILITY FUNCTIONS
//=============================================================================

// Thread-safe serial print
void safePrint(const char* message) {
  if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
    Serial.print(message);
    xSemaphoreGive(serialMutex);
  }
}

void safePrintln(const char* message) {
  if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
    Serial.println(message);
    xSemaphoreGive(serialMutex);
  }
}

void safePrintInt(const char* prefix, int value) {
  if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
    Serial.print(prefix);
    Serial.println(value);
    xSemaphoreGive(serialMutex);
  }
}

// Thread-safe shared LED control
void setSharedLED(bool state) {
  if (xSemaphoreTake(sharedLEDMutex, portMAX_DELAY) == pdTRUE) {
    sharedLEDState = state;
    digitalWrite(LED_SHARED, state ? HIGH : LOW);
    xSemaphoreGive(sharedLEDMutex);
  }
}

// Thread-safe counter increment
void incrementCounter() {
  if (xSemaphoreTake(counterMutex, portMAX_DELAY) == pdTRUE) {
    sharedCounter++;
    xSemaphoreGive(counterMutex);
  }
}

int getCounter() {
  int value = 0;
  if (xSemaphoreTake(counterMutex, portMAX_DELAY) == pdTRUE) {
    value = sharedCounter;
    xSemaphoreGive(counterMutex);
  }
  return value;
}

//=============================================================================
// CORE 0 TASKS
//=============================================================================

// Task 1: LED Blinking on Core 0 (Fast Pattern)
void Task_Core0_BlinkCode(void * parameter) {
  safePrintln("[CORE 0] Blink task started");
  
  pinMode(LED_CORE0, OUTPUT);
  
  for(;;) {
    // Fast blink pattern: 200ms ON, 200ms OFF
    digitalWrite(LED_CORE0, HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    
    digitalWrite(LED_CORE0, LOW);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    
    core0TaskCount++;
    
    // Every 10 blinks, toggle shared LED
    if (core0TaskCount % 10 == 0) {
      setSharedLED(!sharedLEDState);
      incrementCounter();
    }
  }
}

// Task 2: Button Monitor on Core 0
void Task_Core0_ButtonCode(void * parameter) {
  safePrintln("[CORE 0] Button monitor task started");
  
  pinMode(BUTTON1, INPUT_PULLUP);
  
  for(;;) {
    bool reading = digitalRead(BUTTON1);
    
    // Debounce logic
    if (reading != lastButtonState1) {
      lastDebounceTime1 = millis();
    }
    
    if ((millis() - lastDebounceTime1) > debounceDelay) {
      if (reading == LOW) {  // Button pressed (active LOW)
        // Create button event
        ButtonEvent event;
        event.buttonNumber = 1;
        event.pressTime = millis();
        event.coreId = xPortGetCoreID();
        
        // Send to queue (non-blocking)
        if (xQueueSend(buttonQueue, &event, 0) == pdTRUE) {
          safePrintln("[CORE 0] Button 1 event sent to queue");
        } else {
          safePrintln("[CORE 0] Queue full! Event dropped");
        }
        
        // Wait for button release
        while(digitalRead(BUTTON1) == LOW) {
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
      }
    }
    
    lastButtonState1 = reading;
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

//=============================================================================
// CORE 1 TASKS
//=============================================================================

// Task 3: LED Blinking on Core 1 (Slow Pattern)
void Task_Core1_BlinkCode(void * parameter) {
  safePrintln("[CORE 1] Blink task started");
  
  pinMode(LED_CORE1, OUTPUT);
  
  for(;;) {
    // Slow blink pattern: 500ms ON, 500ms OFF
    digitalWrite(LED_CORE1, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    
    digitalWrite(LED_CORE1, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    
    core1TaskCount++;
    
    // Every 5 blinks, signal synchronization
    if (core1TaskCount % 5 == 0) {
      xSemaphoreGive(syncSemaphore);
    }
  }
}

// Task 4: Button Monitor on Core 1
void Task_Core1_ButtonCode(void * parameter) {
  safePrintln("[CORE 1] Button monitor task started");
  
  pinMode(BUTTON2, INPUT_PULLUP);
  
  for(;;) {
    bool reading = digitalRead(BUTTON2);
    
    // Debounce logic
    if (reading != lastButtonState2) {
      lastDebounceTime2 = millis();
    }
    
    if ((millis() - lastDebounceTime2) > debounceDelay) {
      if (reading == LOW) {  // Button pressed (active LOW)
        // Create button event
        ButtonEvent event;
        event.buttonNumber = 2;
        event.pressTime = millis();
        event.coreId = xPortGetCoreID();
        
        // Send to queue (non-blocking)
        if (xQueueSend(buttonQueue, &event, 0) == pdTRUE) {
          safePrintln("[CORE 1] Button 2 event sent to queue");
        } else {
          safePrintln("[CORE 1] Queue full! Event dropped");
        }
        
        // Wait for button release
        while(digitalRead(BUTTON2) == LOW) {
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
      }
    }
    
    lastButtonState2 = reading;
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

//=============================================================================
// SHARED TASKS (Can run on either core)
//=============================================================================

// Task 5: Event Processor (processes button events from queue)
void Task_EventProcessorCode(void * parameter) {
  safePrintln("[EVENT] Event processor task started");
  
  ButtonEvent event;
  
  for(;;) {
    // Wait for button events from queue
    if (xQueueReceive(buttonQueue, &event, portMAX_DELAY) == pdTRUE) {
      // Process the event
      if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
        Serial.println("\n========== BUTTON EVENT ==========");
        Serial.print("Button: ");
        Serial.println(event.buttonNumber);
        Serial.print("Press Time: ");
        Serial.println(event.pressTime);
        Serial.print("Detected by Core: ");
        Serial.println(event.coreId);
        Serial.print("Processed by Core: ");
        Serial.println(xPortGetCoreID());
        Serial.print("Shared Counter: ");
        Serial.println(getCounter());
        Serial.println("==================================\n");
        xSemaphoreGive(serialMutex);
      }
      
      // Perform action based on button
      if (event.buttonNumber == 1) {
        // Button 1: Quick flash shared LED
        for(int i = 0; i < 3; i++) {
          setSharedLED(true);
          vTaskDelay(100 / portTICK_PERIOD_MS);
          setSharedLED(false);
          vTaskDelay(100 / portTICK_PERIOD_MS);
        }
      } else if (event.buttonNumber == 2) {
        // Button 2: Reset counter
        if (xSemaphoreTake(counterMutex, portMAX_DELAY) == pdTRUE) {
          sharedCounter = 0;
          xSemaphoreGive(counterMutex);
        }
        safePrintln("[EVENT] Counter reset to 0");
      }
    }
  }
}

// Task 6: System Monitor (demonstrates task synchronization)
void Task_MonitorCode(void * parameter) {
  safePrintln("[MONITOR] System monitor task started");
  
  for(;;) {
    // Wait for synchronization signal from Core 1 blink task
    if (xSemaphoreTake(syncSemaphore, pdMS_TO_TICKS(5000)) == pdTRUE) {
      safePrintln("\n----- SYNCHRONIZED EVENT -----");
      
      if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
        Serial.print("Core 0 Task Count: ");
        Serial.println(core0TaskCount);
        Serial.print("Core 1 Task Count: ");
        Serial.println(core1TaskCount);
        Serial.print("Shared Counter: ");
        Serial.println(getCounter());
        Serial.print("Free Heap: ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");
        Serial.println("------------------------------\n");
        xSemaphoreGive(serialMutex);
      }
    }
  }
}

//=============================================================================
// ARDUINO SETUP
//=============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║  ESP32 Dual-Core Programming Demo     ║");
  Serial.println("║  Lab-16: FreeRTOS Multi-Core Tasks    ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Initialize shared LED
  pinMode(LED_SHARED, OUTPUT);
  digitalWrite(LED_SHARED, LOW);
  
  // Create Mutex for shared resources
  serialMutex = xSemaphoreCreateMutex();
  sharedLEDMutex = xSemaphoreCreateMutex();
  counterMutex = xSemaphoreCreateMutex();
  
  if (serialMutex == NULL || sharedLEDMutex == NULL || counterMutex == NULL) {
    Serial.println("ERROR: Failed to create mutexes!");
    while(1);
  }
  
  // Create binary semaphore for synchronization
  syncSemaphore = xSemaphoreCreateBinary();
  
  if (syncSemaphore == NULL) {
    Serial.println("ERROR: Failed to create sync semaphore!");
    while(1);
  }
  
  // Create queue for button events (max 10 events)
  buttonQueue = xQueueCreate(10, sizeof(ButtonEvent));
  
  if (buttonQueue == NULL) {
    Serial.println("ERROR: Failed to create button queue!");
    while(1);
  }
  
  Serial.println("✓ Mutexes created");
  Serial.println("✓ Semaphore created");
  Serial.println("✓ Queue created");
  Serial.println();
  
  // Display core information
  Serial.println("CPU Configuration:");
  Serial.print("  - CPU Frequency: ");
  Serial.print(getCpuFrequencyMhz());
  Serial.println(" MHz");
  Serial.print("  - Number of Cores: ");
  Serial.println(ESP.getChipCores());
  Serial.print("  - Setup running on Core: ");
  Serial.println(xPortGetCoreID());
  Serial.println();
  
  // Create tasks pinned to Core 0
  Serial.println("Creating Core 0 tasks...");
  xTaskCreatePinnedToCore(
    Task_Core0_BlinkCode,      // Task function
    "Core0_Blink",             // Task name
    10000,                     // Stack size (bytes)
    NULL,                      // Parameters
    2,                         // Priority (0-24, higher = more priority)
    &Task_Core0_Blink,         // Task handle
    0                          // Core ID (0)
  );
  
  xTaskCreatePinnedToCore(
    Task_Core0_ButtonCode,
    "Core0_Button",
    10000,
    NULL,
    3,                         // Higher priority for button responsiveness
    &Task_Core0_Button,
    0
  );
  
  // Create tasks pinned to Core 1
  Serial.println("Creating Core 1 tasks...");
  xTaskCreatePinnedToCore(
    Task_Core1_BlinkCode,
    "Core1_Blink",
    10000,
    NULL,
    2,
    &Task_Core1_Blink,
    1                          // Core ID (1)
  );
  
  xTaskCreatePinnedToCore(
    Task_Core1_ButtonCode,
    "Core1_Button",
    10000,
    NULL,
    3,
    &Task_Core1_Button,
    1
  );
  
  // Create shared tasks (not pinned to specific core)
  Serial.println("Creating shared tasks...");
  xTaskCreate(
    Task_EventProcessorCode,
    "EventProcessor",
    10000,
    NULL,
    4,                         // Highest priority for event processing
    &Task_EventProcessor
  );
  
  xTaskCreate(
    Task_MonitorCode,
    "SystemMonitor",
    10000,
    NULL,
    1,                         // Lowest priority
    &Task_Monitor
  );
  
  Serial.println();
  Serial.println("✓ All tasks created and started!");
  Serial.println();
  Serial.println("System Status:");
  Serial.println("  - LED on GPIO 13: Fast blink (Core 0)");
  Serial.println("  - LED on GPIO 12: Slow blink (Core 1)");
  Serial.println("  - LED on GPIO 14: Shared (mutex protected)");
  Serial.println("  - Button on GPIO 18: Triggers event & flashes shared LED");
  Serial.println("  - Button on GPIO 19: Resets counter");
  Serial.println();
  Serial.println("Press buttons to see inter-task communication!");
  Serial.println("═══════════════════════════════════════════\n");
}

//=============================================================================
// ARDUINO LOOP
//=============================================================================

void loop() {
  // Empty loop - all work is done in FreeRTOS tasks
  // The loop task runs on Core 1 by default
  
  // You can add additional code here if needed
  // This demonstrates that Arduino loop() still exists alongside FreeRTOS tasks
  
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
