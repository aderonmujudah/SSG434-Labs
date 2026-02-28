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
#define TOUCH_PIN_1      4      // T0
#define TOUCH_PIN_2      2      // T2
#define TOUCH_PIN_3      15     // T3

// LED Pins (same as Lab-16)
#define LED_1            13
#define LED_2            12
#define LED_3            14

// ============================================================================
// CONFIGURATION
// ============================================================================

// Sensitivity Settings (lower = more sensitive)
#define TOUCH_THRESHOLD_PERCENT  70    // % of baseline for touch detection
#define HOVER_THRESHOLD_PERCENT  85    // % of baseline for proximity detection

// Touch Duration Thresholds (milliseconds)
#define QUICK_PRESS_MAX     1000   // < 1 second
#define MEDIUM_PRESS_MIN    1000   // 1-3 seconds
#define MEDIUM_PRESS_MAX    3000
#define LONG_PRESS_MIN      3000   // 3-5 seconds
#define LONG_PRESS_MAX      5000
#define VERY_LONG_PRESS_MIN 5000   // > 5 seconds

// Calibration
#define CALIBRATION_SAMPLES  50    // Number of samples for baseline
#define CALIBRATION_DELAY    20    // ms between samples

// Monitoring
#define MONITOR_INTERVAL     100   // ms between value prints
#define DEBOUNCE_TIME        50    // ms debounce delay

// ============================================================================
// TOUCH SENSOR STRUCTURE
// ============================================================================

struct TouchSensor {
  uint8_t pin;              // GPIO pin number
  uint8_t touchNum;         // Touch channel (T0-T9)
  uint16_t baseline;        // Calibrated baseline (untouched)
  uint16_t currentValue;    // Current touch reading
  uint16_t touchThreshold;  // Threshold for touch detection
  uint16_t hoverThreshold;  // Threshold for proximity detection
  bool isTouched;           // Current touch state
  bool wasHovering;         // Previous hover state
  unsigned long touchStartTime;  // When touch began
  unsigned long lastDebounceTime; // For debouncing
  uint8_t ledPin;           // Associated LED
  const char* name;         // Sensor name for display
};

// Three touch sensors
TouchSensor sensors[3] = {
  {TOUCH_PIN_1, 0, 0, 0, 0, 0, false, false, 0, 0, LED_1, "Sensor 1"},
  {TOUCH_PIN_2, 2, 0, 0, 0, 0, false, false, 0, 0, LED_2, "Sensor 2"},
  {TOUCH_PIN_3, 3, 0, 0, 0, 0, false, false, 0, 0, LED_3, "Sensor 3"}
};

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

unsigned long lastMonitorTime = 0;
bool calibrationComplete = false;
bool monitoringEnabled = true;

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
  
  // Perform calibration
  Serial.println("═══════════════════════════════════════");
  Serial.println("Starting Calibration...");
  Serial.println("═══════════════════════════════════════");
  Serial.println("⚠️  DO NOT TOUCH sensors during calibration!");
  Serial.println();
  
  delay(2000);  // Give user time to read warning
  
  calibrateSensors();
  
  Serial.println();
  Serial.println("═══════════════════════════════════════");
  Serial.println("✓ Calibration Complete!");
  Serial.println("═══════════════════════════════════════");
  Serial.println();
  
  displayCalibrationResults();
  
  Serial.println();
  Serial.println("Touch Pattern Guide:");
  Serial.println("  • Quick Tap (<1s):    Single blink");
  Serial.println("  • Medium Hold (1-3s): Double blink");
  Serial.println("  • Long Hold (3-5s):   Triple blink");
  Serial.println("  • Very Long (>5s):    Rapid flash");
  Serial.println("  • Hover (proximity):  Dim LED");
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
  
  unsigned long sum = 0;
  uint16_t minVal = 65535;
  uint16_t maxVal = 0;
  
  // Collect samples
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    uint16_t value = touchRead(sensor->pin);
    sum += value;
    
    if (value < minVal) minVal = value;
    if (value > maxVal) maxVal = value;
    
    delay(CALIBRATION_DELAY);
    
    // Visual progress indicator
    if (i % 10 == 0) {
      Serial.print(".");
    }
  }
  
  // Calculate baseline (average)
  sensor->baseline = sum / CALIBRATION_SAMPLES;
  
  // Calculate thresholds
  sensor->touchThreshold = (sensor->baseline * TOUCH_THRESHOLD_PERCENT) / 100;
  sensor->hoverThreshold = (sensor->baseline * HOVER_THRESHOLD_PERCENT) / 100;
  
  Serial.println(" Done!");
  Serial.print("    Baseline: ");
  Serial.print(sensor->baseline);
  Serial.print(" (Range: ");
  Serial.print(minVal);
  Serial.print("-");
  Serial.print(maxVal);
  Serial.println(")");
}

void displayCalibrationResults() {
  Serial.println("Calibration Results:");
  Serial.println();
  Serial.println("┌──────────┬──────────┬────────────┬────────────┐");
  Serial.println("│ Sensor   │ Baseline │ Touch (<)  │ Hover (<)  │");
  Serial.println("├──────────┼──────────┼────────────┼────────────┤");
  
  for (int i = 0; i < 3; i++) {
    Serial.print("│ ");
    Serial.print(sensors[i].name);
    Serial.print("  │ ");
    printPaddedNumber(sensors[i].baseline, 8);
    Serial.print(" │ ");
    printPaddedNumber(sensors[i].touchThreshold, 10);
    Serial.print(" │ ");
    printPaddedNumber(sensors[i].hoverThreshold, 10);
    Serial.println(" │");
  }
  
  Serial.println("└──────────┴──────────┴────────────┴────────────┘");
}

// ============================================================================
// TOUCH DETECTION FUNCTIONS
// ============================================================================

void updateSensor(TouchSensor* sensor) {
  // Read current touch value
  sensor->currentValue = touchRead(sensor->pin);
  
  // Detect touch state
  bool isCurrentlyTouched = sensor->currentValue < sensor->touchThreshold;
  
  // Debounce logic
  if (isCurrentlyTouched != sensor->isTouched) {
    if ((millis() - sensor->lastDebounceTime) > DEBOUNCE_TIME) {
      // State change confirmed
      if (isCurrentlyTouched && !sensor->isTouched) {
        // Touch started
        handleTouchStart(sensor);
      } else if (!isCurrentlyTouched && sensor->isTouched) {
        // Touch released
        handleTouchRelease(sensor);
      }
      
      sensor->isTouched = isCurrentlyTouched;
      sensor->lastDebounceTime = millis();
    }
  }
  
  // Check for hovering (proximity detection)
  bool isHovering = !sensor->isTouched && 
                    sensor->currentValue < sensor->hoverThreshold &&
                    sensor->currentValue >= sensor->touchThreshold;
  
  if (isHovering != sensor->wasHovering) {
    if (isHovering) {
      handleHoverStart(sensor);
    } else {
      handleHoverEnd(sensor);
    }
    sensor->wasHovering = isHovering;
  }
}

void handleTouchStart(TouchSensor* sensor) {
  sensor->touchStartTime = millis();
  
  Serial.print("\n[TOUCH START] ");
  Serial.print(sensor->name);
  Serial.print(" touched (value: ");
  Serial.print(sensor->currentValue);
  Serial.println(")");
  
  // Turn on LED immediately
  digitalWrite(sensor->ledPin, HIGH);
}

void handleTouchRelease(TouchSensor* sensor) {
  unsigned long touchDuration = millis() - sensor->touchStartTime;
  
  Serial.print("[TOUCH END] ");
  Serial.print(sensor->name);
  Serial.print(" released after ");
  Serial.print(touchDuration);
  Serial.println(" ms");
  
  // Determine touch duration category and trigger pattern
  if (touchDuration < QUICK_PRESS_MAX) {
    Serial.println("  → Quick tap detected");
    playPattern(sensor->ledPin, PATTERN_QUICK);
  } else if (touchDuration < MEDIUM_PRESS_MAX) {
    Serial.println("  → Medium hold detected");
    playPattern(sensor->ledPin, PATTERN_MEDIUM);
  } else if (touchDuration < LONG_PRESS_MAX) {
    Serial.println("  → Long hold detected");
    playPattern(sensor->ledPin, PATTERN_LONG);
  } else {
    Serial.println("  → Very long hold detected");
    playPattern(sensor->ledPin, PATTERN_VERY_LONG);
  }
  
  Serial.println();
}

void handleHoverStart(TouchSensor* sensor) {
  Serial.print("[PROXIMITY] ");
  Serial.print(sensor->name);
  Serial.print(" - Hovering detected (value: ");
  Serial.print(sensor->currentValue);
  Serial.println(")");
  
  // Dim LED for hover effect (using PWM)
  analogWrite(sensor->ledPin, 50);  // Low brightness
}

void handleHoverEnd(TouchSensor* sensor) {
  Serial.print("[PROXIMITY] ");
  Serial.print(sensor->name);
  Serial.println(" - Hover ended");
  
  // Turn off LED
  digitalWrite(sensor->ledPin, LOW);
}

// ============================================================================
// LED PATTERN FUNCTIONS
// ============================================================================

enum Pattern {
  PATTERN_QUICK,      // Single blink
  PATTERN_MEDIUM,     // Double blink
  PATTERN_LONG,       // Triple blink
  PATTERN_VERY_LONG   // Rapid flash
};

void playPattern(uint8_t ledPin, Pattern pattern) {
  // Turn off LED first
  digitalWrite(ledPin, LOW);
  delay(200);
  
  switch (pattern) {
    case PATTERN_QUICK:
      // Single blink
      digitalWrite(ledPin, HIGH);
      delay(300);
      digitalWrite(ledPin, LOW);
      break;
      
    case PATTERN_MEDIUM:
      // Double blink
      for (int i = 0; i < 2; i++) {
        digitalWrite(ledPin, HIGH);
        delay(200);
        digitalWrite(ledPin, LOW);
        delay(200);
      }
      break;
      
    case PATTERN_LONG:
      // Triple blink
      for (int i = 0; i < 3; i++) {
        digitalWrite(ledPin, HIGH);
        delay(200);
        digitalWrite(ledPin, LOW);
        delay(200);
      }
      break;
      
    case PATTERN_VERY_LONG:
      // Rapid flash (10 times)
      for (int i = 0; i < 10; i++) {
        digitalWrite(ledPin, HIGH);
        delay(50);
        digitalWrite(ledPin, LOW);
        delay(50);
      }
      break;
  }
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
  } else if (sensor->wasHovering) {
    Serial.print("[HOVER]");
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
      Serial.println("Recalibrating sensors...");
      Serial.println("═══════════════════════════════════════");
      Serial.println("⚠️  DO NOT TOUCH sensors!");
      Serial.println();
      delay(2000);
      calibrateSensors();
      Serial.println("\n✓ Recalibration complete!\n");
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
        Serial.print("  Touch <:  ");
        Serial.println(sensors[i].touchThreshold);
        Serial.print("  Hover <:  ");
        Serial.println(sensors[i].hoverThreshold);
        Serial.print("  State:    ");
        if (sensors[i].isTouched) {
          Serial.println("TOUCHED");
        } else if (sensors[i].wasHovering) {
          Serial.println("HOVERING");
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
// SENSITIVITY ADJUSTMENT (Advanced)
// ============================================================================

// Call this function to adjust sensitivity dynamically
void adjustSensitivity(int percentChange) {
  Serial.println("\n═══════════════════════════════════════");
  Serial.print("Adjusting sensitivity by ");
  Serial.print(percentChange);
  Serial.println("%");
  Serial.println("═══════════════════════════════════════");
  
  for (int i = 0; i < 3; i++) {
    int newTouchPercent = TOUCH_THRESHOLD_PERCENT + percentChange;
    int newHoverPercent = HOVER_THRESHOLD_PERCENT + percentChange;
    
    // Clamp values
    newTouchPercent = constrain(newTouchPercent, 50, 90);
    newHoverPercent = constrain(newHoverPercent, 70, 95);
    
    sensors[i].touchThreshold = (sensors[i].baseline * newTouchPercent) / 100;
    sensors[i].hoverThreshold = (sensors[i].baseline * newHoverPercent) / 100;
    
    Serial.print(sensors[i].name);
    Serial.print(": Touch<");
    Serial.print(sensors[i].touchThreshold);
    Serial.print(", Hover<");
    Serial.println(sensors[i].hoverThreshold);
  }
  
  Serial.println("═══════════════════════════════════════\n");
}

// Example usage (uncomment and call from loop or command):
// adjustSensitivity(-5);  // Make more sensitive (lower threshold)
// adjustSensitivity(+5);  // Make less sensitive (higher threshold)
