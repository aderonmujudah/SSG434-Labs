/*
 * ESP32 Capacitive Touch Sensor Demo
 * Lab-17: Touch Detection with LED Feedback
 * 
 * Hardware Setup:
 * - Touch Sensor 1: GPIO 4 (T0) - Connect wire/metal pad
 * - Touch Sensor 2: GPIO 2 (T2) - Connect wire/metal pad
 * - Touch Sensor 3: GPIO 15 (T3) - Connect wire/metal pad
 * - LED 1: GPIO 13 (same as Lab-16)
 * - LED 2: GPIO 12 (same as Lab-16)
 * - LED 3: GPIO 14 (same as Lab-16)
 * 
 * Features:
 * - Real-time touch value monitoring
 * - Automatic baseline calibration
 * - Touch detection with adjustable thresholds
 * - Duration-based LED patterns (quick, 3s, 5s)
 * - Proximity/hover detection
 * - Adjustable sensitivity
 * - Multi-sensor support
 */

#include <Arduino.h>

// ============================================================================
// PIN DEFINITIONS
// ============================================================================

// Touch Sensor Pins (ESP32 Touch-capable GPIOs)
// Switched to pins that are less likely to interfere with boot/strapping.
#define TOUCH_PIN_1      4      // T0
#define TOUCH_PIN_2      27     // T7
#define TOUCH_PIN_3      32     // T9

// LED Pins (same as Lab-16)
#define LED_1            13
#define LED_2            12
#define LED_3            14

// ============================================================================
// CONFIGURATION
// ============================================================================

// Sensitivity Settings
#define TOUCH_DROP_THRESHOLD 8     // LED turns on when value drops by this amount

// Calibration
#define CALIBRATION_SAMPLES  50    // Number of samples for baseline
#define CALIBRATION_DELAY    20    // ms between samples
#define CALIBRATION_TIMEOUT  3000  // ms max per sensor calibration

// Monitoring
#define MONITOR_INTERVAL     100   // ms between value prints
#define DEBOUNCE_TIME        50    // ms debounce delay

// ============================================================================
// TOUCH SENSOR STRUCTURE
// ============================================================================

struct TouchSensor {
  uint8_t pin;              // GPIO pin number
  uint8_t touchNum;         // Touch channel (T0-T9)
  uint16_t baseline;        // Calibrated minimum value (untouched)
  uint16_t currentValue;    // Current touch reading
  bool isTouched;           // Current touch state
  unsigned long lastDebounceTime; // For debouncing
  uint8_t ledPin;           // Associated LED
  const char* name;         // Sensor name for display
};

// Three touch sensors
TouchSensor sensors[3] = {
  {TOUCH_PIN_1, 0, 0, 0, false, 0, LED_1, "Sensor 1"},
  {TOUCH_PIN_2, 7, 0, 0, false, 0, LED_2, "Sensor 2"},
  {TOUCH_PIN_3, 9, 0, 0, false, 0, LED_3, "Sensor 3"}
};

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

unsigned long lastMonitorTime = 0;
bool calibrationComplete = false;
bool monitoringEnabled = true;

// Persist calibration across soft resets
RTC_DATA_ATTR bool hasCalibration = false;
RTC_DATA_ATTR uint16_t storedBaseline[3] = {0, 0, 0};

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Print header
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║  ESP32 Touch Sensor Demo              ║");
  Serial.println("║  Lab-17: Capacitive Touch Detection   ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Initialize LED pins
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  
  // Turn off all LEDs
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);
  
  Serial.println("✓ LEDs initialized");
  Serial.println();

  // Display touch pin information
  Serial.println("Touch Sensor Configuration:");
  for (int i = 0; i < 3; i++) {
    Serial.print("  - ");
    Serial.print(sensors[i].name);
    Serial.print(": GPIO ");
    Serial.print(sensors[i].pin);
    Serial.print(" (T");
    Serial.print(sensors[i].touchNum);
    Serial.print(") → LED GPIO ");
    Serial.println(sensors[i].ledPin);
  }
  Serial.println();

  // Initialize touch hardware BEFORE any touchRead() call.
  // Without this, touchRead() can block indefinitely and trigger a WDT reset.
  touchSetCycles(0x1000, 0x1000);
  delay(100);
  // Discard first reading on each pin (hardware warm-up)
  for (int i = 0; i < 3; i++) {
    touchRead(sensors[i].pin);
    delay(20);
  }
  Serial.println("✓ Touch hardware initialized");
  Serial.println();

  // Calibration — runs at most once per power cycle (guarded by hasCalibration in RTC RAM)
  if (hasCalibration) {
    // Restore calibration stored in RTC RAM (survives soft resets / sleep)
    for (int i = 0; i < 3; i++) {
      sensors[i].baseline = storedBaseline[i];
    }
    Serial.println("Using stored calibration (skipping recalibration)");
  } else {
    Serial.println("═══════════════════════════════════════");
    Serial.println("Starting Calibration...");
    Serial.println("═══════════════════════════════════════");
    Serial.println("DO NOT TOUCH sensors during calibration!");
    Serial.println();
    delay(2000);
    calibrateSensors();
    for (int i = 0; i < 3; i++) {
      storedBaseline[i] = sensors[i].baseline;
    }
    hasCalibration = true;
  }
  
  Serial.println();
  Serial.println("═══════════════════════════════════════");
  Serial.println("✓ Calibration Complete!");
  Serial.println("═══════════════════════════════════════");
  Serial.println();
  
  displayCalibrationResults();
  
  Serial.println();
  Serial.println("Touch Rule:");
  Serial.print("  • LED ON when value <= baseline - ");
  Serial.print(TOUCH_DROP_THRESHOLD);
  Serial.println();
  Serial.println();
  Serial.println("Commands:");
  Serial.println("  • Type 'r' to recalibrate");
  Serial.println("  • Type 'm' to toggle monitoring");
  Serial.println("  • Type 's' to show current values");
  Serial.println();
  Serial.println("═══════════════════════════════════════");
  Serial.println("System Ready - Touch sensors active!");
  Serial.println("═══════════════════════════════════════\n");
  
  calibrationComplete = true;
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  // Check for serial commands
  if (Serial.available()) {
    char cmd = Serial.read();
    handleCommand(cmd);
  }
  
  // Update all sensors
  for (int i = 0; i < 3; i++) {
    updateSensor(&sensors[i]);
  }
  
  // Periodic monitoring display
  if (monitoringEnabled && (millis() - lastMonitorTime > MONITOR_INTERVAL)) {
    displayTouchValues();
    lastMonitorTime = millis();
  }
  
  delay(10);  // Small delay for stability
}

// ============================================================================
// CALIBRATION FUNCTIONS
// ============================================================================

void calibrateSensors() {
  for (int i = 0; i < 3; i++) {
    calibrateSensor(&sensors[i]);
  }
}

void calibrateSensor(TouchSensor* sensor) {
  Serial.print("Calibrating ");
  Serial.print(sensor->name);
  Serial.print("... ");
  
  unsigned long startTime = millis();
  unsigned long sum = 0;
  uint16_t minVal = 65535;
  uint16_t samplesTaken = 0;
  
  // Collect samples
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    if (millis() - startTime > CALIBRATION_TIMEOUT) {
      Serial.print("!timeout!");
      break;
    }

    uint16_t value = touchRead(sensor->pin);
    sum += value;
    samplesTaken++;
    
    if (value < minVal) minVal = value;
    
    delay(CALIBRATION_DELAY);
    yield();
    
    // Visual progress indicator
    if (i % 10 == 0) {
      Serial.print(".");
    }
  }
  
  // Use the minimum observed value as the baseline
  if (samplesTaken == 0) {
    minVal = 0;
  }
  sensor->baseline = minVal;
  
  Serial.println(" Done!");
  Serial.print("    Baseline (min): ");
  Serial.print(sensor->baseline);
  Serial.println();
}

void displayCalibrationResults() {
  Serial.println("Calibration Results:");
  Serial.println();
  Serial.println("┌──────────┬──────────┐");
  Serial.println("│ Sensor   │ Baseline │");
  Serial.println("├──────────┼──────────┤");
  
  for (int i = 0; i < 3; i++) {
    Serial.print("│ ");
    Serial.print(sensors[i].name);
    Serial.print("  │ ");
    printPaddedNumber(sensors[i].baseline, 8);
    Serial.println(" │");
  }

  Serial.println("└──────────┴──────────┘");
}

// ============================================================================
// TOUCH DETECTION FUNCTIONS
// ============================================================================

void updateSensor(TouchSensor* sensor) {
  // Read current touch value
  sensor->currentValue = touchRead(sensor->pin);
  
  // Detect touch state based on drop from calibrated minimum
  bool isCurrentlyTouched = sensor->currentValue + TOUCH_DROP_THRESHOLD <= sensor->baseline;
  
  // Debounce logic
  if (isCurrentlyTouched != sensor->isTouched) {
    if ((millis() - sensor->lastDebounceTime) > DEBOUNCE_TIME) {
      sensor->isTouched = isCurrentlyTouched;
      sensor->lastDebounceTime = millis();

      Serial.print("\n[");
      Serial.print(isCurrentlyTouched ? "TOUCH" : "RELEASE");
      Serial.print("] ");
      Serial.print(sensor->name);
      Serial.print(" (value: ");
      Serial.print(sensor->currentValue);
      Serial.println(")");
    }
  }
  
  // LED mirrors touch state
  digitalWrite(sensor->ledPin, sensor->isTouched ? HIGH : LOW);
}

// ============================================================================
// MONITORING AND DISPLAY
// ============================================================================

void displayTouchValues() {
  // Clear previous line (move cursor up and clear)
  Serial.print("\r");
  
  // Display header on first run
  static bool headerDisplayed = false;
  if (!headerDisplayed) {
    Serial.print("Touch Values: ");
    headerDisplayed = true;
  }
  
  // Display values for all sensors
  Serial.print("S1:");
  printPaddedNumber(sensors[0].currentValue, 4);
  Serial.print(" ");
  printTouchIndicator(&sensors[0]);
  Serial.print(" | S2:");
  printPaddedNumber(sensors[1].currentValue, 4);
  Serial.print(" ");
  printTouchIndicator(&sensors[1]);
  Serial.print(" | S3:");
  printPaddedNumber(sensors[2].currentValue, 4);
  Serial.print(" ");
  printTouchIndicator(&sensors[2]);
  Serial.print("   ");  // Clear any extra characters
}

void printTouchIndicator(TouchSensor* sensor) {
  if (sensor->isTouched) {
    Serial.print("[TOUCH]");
  } else {
    Serial.print("[     ]");
  }
}

void printPaddedNumber(uint16_t num, int width) {
  char buffer[10];
  sprintf(buffer, "%*d", width, num);
  Serial.print(buffer);
}

// ============================================================================
// COMMAND HANDLING
// ============================================================================

void handleCommand(char cmd) {
  switch (cmd) {
    case 'r':
    case 'R':
      Serial.println("\n\n═══════════════════════════════════════");
      Serial.println("Manual recalibration...");
      Serial.println("═══════════════════════════════════════");
      Serial.println("DO NOT TOUCH sensors!");
      Serial.println();
      delay(2000);
      calibrateSensors();
      for (int i = 0; i < 3; i++) {
        storedBaseline[i] = sensors[i].baseline;
      }
      hasCalibration = true;
      Serial.println("\n✓ Calibration complete!\n");
      displayCalibrationResults();
      Serial.println();
      break;
      
    case 'm':
    case 'M':
      monitoringEnabled = !monitoringEnabled;
      Serial.println();
      Serial.print("Monitoring: ");
      Serial.println(monitoringEnabled ? "ON" : "OFF");
      Serial.println();
      break;
      
    case 's':
    case 'S':
      Serial.println("\n═══════════════════════════════════════");
      Serial.println("Current Touch Values:");
      Serial.println("═══════════════════════════════════════");
      for (int i = 0; i < 3; i++) {
        Serial.print(sensors[i].name);
        Serial.print(":");
        Serial.println();
        Serial.print("  Current:  ");
        Serial.println(sensors[i].currentValue);
        Serial.print("  Baseline: ");
        Serial.println(sensors[i].baseline);
        Serial.print("  State:    ");
        if (sensors[i].isTouched) {
          Serial.println("TOUCHED");
        } else {
          Serial.println("IDLE");
        }
        Serial.println();
      }
      Serial.println("═══════════════════════════════════════\n");
      break;
      
    case 'h':
    case 'H':
    case '?':
      printHelp();
      break;
  }
}

void printHelp() {
  Serial.println("\n═══════════════════════════════════════");
  Serial.println("Available Commands:");
  Serial.println("═══════════════════════════════════════");
  Serial.println("  r - Recalibrate sensors");
  Serial.println("  m - Toggle value monitoring");
  Serial.println("  s - Show current values");
  Serial.println("  h - Show this help");
  Serial.println("═══════════════════════════════════════\n");
}

// ============================================================================
