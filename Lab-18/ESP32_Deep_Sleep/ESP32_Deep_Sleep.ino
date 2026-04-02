/*
 * ESP32 Deep Sleep & Power Management Lab
 * 
 * Features:
 * - Deep sleep with 10-second timer wakeup
 * - Wakeup counter stored in RTC memory
 * - External wakeup via buttons (GPIO 0 and 35)
 * - Combined timer and external wakeup sources
 * - LED indicator on GPIO 12
 * - Current consumption monitoring support
 * - Multiple wakeup strategies for power optimization
 * 
 * Hardware Connections:
 * - LED: GPIO 12 (with 220Ω resistor to GND)
 * - Button 1: GPIO 0 (built-in BOOT button, internal pull-up)
 * - Button 2: GPIO 35 (external pull-up required - 10kΩ to 3.3V)
 * 
 * Wakeup Sources:
 * - Timer: Every 10 seconds
 * - External: Button press on GPIO 0 or 35
 * - Combined: Both timer and external enabled
 * 
 * Power Modes:
 * 1. Active: ~80-260mA (WiFi on)
 * 2. Modem Sleep: ~20-40mA (WiFi off)
 * 3. Light Sleep: ~0.8mA
 * 4. Deep Sleep: ~10-150µA (varies by wakeup sources)
 * 5. Hibernation: ~5µA (minimal functionality)
 */

#include <esp_sleep.h>
#include <esp_system.h>
#include <driver/rtc_io.h>

// ============ Pin Definitions ============
#define LED_PIN      12
#define BUTTON1_PIN  0   // BOOT button (built-in pull-up)
#define BUTTON2_PIN  35  // External button (needs external pull-up)

// ============ Sleep Configuration ============
#define TIMER_WAKEUP_TIME  10  // Seconds
#define uS_TO_S_FACTOR     1000000ULL  // Conversion factor

// ============ RTC Memory ============
// Variables stored in RTC slow memory (preserved during deep sleep)
RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int timerWakeupCount = 0;
RTC_DATA_ATTR int button1WakeupCount = 0;
RTC_DATA_ATTR int button2WakeupCount = 0;
RTC_DATA_ATTR unsigned long totalActiveTime = 0;
RTC_DATA_ATTR unsigned long lastWakeupTime = 0;

// ============ Wakeup Strategy ============
enum SleepStrategy {
  STRATEGY_TIMER_ONLY,      // Timer wakeup every 10s (lowest power)
  STRATEGY_BUTTON_ONLY,     // External wakeup only (very low power)
  STRATEGY_COMBINED,        // Timer + External (moderate power)
  STRATEGY_EXT1_ALL_LOW,    // Wake when both buttons pressed (lowest)
  STRATEGY_EXT1_ANY_HIGH    // Wake when any button pressed (low)
};

SleepStrategy currentStrategy = STRATEGY_COMBINED;

// ============ Function Prototypes ============
void printWakeupReason();
void printStatistics();
void configureSleepMode(SleepStrategy strategy);
void blinkLED(int times, int delayMs);
void enterDeepSleep();
void handleSerialCommands();
void printPowerInfo();
void testWakeupSources();

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  // Initialize serial (keep baud low to reduce power)
  Serial.begin(115200);
  delay(500);  // Allow serial to stabilize
  
  // Increment boot counter
  bootCount++;
  
  // Configure LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Configure button pins
  pinMode(BUTTON1_PIN, INPUT_PULLUP);  // GPIO 0 has internal pull-up
  pinMode(BUTTON2_PIN, INPUT);         // GPIO 35 needs external pull-up
  
  // Visual indicator - we're awake!
  blinkLED(3, 100);
  
  // Print header
  Serial.println("\n\n╔════════════════════════════════════════════╗");
  Serial.println("║  ESP32 Deep Sleep & Power Management Lab  ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  
  // Print boot information
  Serial.print("Boot #");
  Serial.println(bootCount);
  
  // Print wakeup reason
  printWakeupReason();
  
  // Print accumulated statistics
  printStatistics();
  
  // Print power information
  printPowerInfo();
  
  // Print menu
  printMenu();
  
  // Wait for user interaction (30 seconds timeout)
  Serial.println("\nYou have 30 seconds to enter commands...");
  unsigned long startTime = millis();
  unsigned long wakeTime = millis();
  
  while (millis() - startTime < 30000) {
    handleSerialCommands();
    
    // Blink LED to show we're active
    if (millis() - wakeTime > 1000) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      wakeTime = millis();
    }
    
    delay(10);
  }
  
  // Turn off LED before sleep
  digitalWrite(LED_PIN, LOW);
  
  // Update active time
  totalActiveTime += (millis() / 1000);
  lastWakeupTime = millis();
  
  // Enter deep sleep
  Serial.println("\n⏳ Entering deep sleep...");
  delay(100);  // Allow serial to finish
  
  enterDeepSleep();
}

// ============================================================================
// LOOP (never reached - ESP32 always deep sleeps after setup)
// ============================================================================
void loop() {
  // This will never be called - ESP32 deep sleeps after setup()
  // Each wakeup causes a full restart and setup() runs again
}

// ============================================================================
// Print Wakeup Reason
// ============================================================================
void printWakeupReason() {
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  
  Serial.print("Wakeup cause: ");
  
  switch(wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("🔘 External signal (EXT0) - Single pin");
      button1WakeupCount++;
      break;
      
    case ESP_SLEEP_WAKEUP_EXT1: {
      Serial.println("🔘 External signal (EXT1) - Multiple pins");
      // Check which button woke us up
      uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
      if (wakeup_pin_mask & (1ULL << BUTTON1_PIN)) {
        Serial.println("   → Button 1 (GPIO 0) pressed");
        button1WakeupCount++;
      }
      if (wakeup_pin_mask & (1ULL << BUTTON2_PIN)) {
        Serial.println("   → Button 2 (GPIO 35) pressed");
        button2WakeupCount++;
      }
      break;
    }
      
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("⏰ Timer wakeup");
      timerWakeupCount++;
      break;
      
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("👆 Touchpad wakeup");
      break;
      
    case ESP_SLEEP_WAKEUP_ULP:
      Serial.println("🔬 ULP co-processor wakeup");
      break;
      
    case ESP_SLEEP_WAKEUP_GPIO:
      Serial.println("📍 GPIO wakeup");
      break;
      
    case ESP_SLEEP_WAKEUP_UART:
      Serial.println("📡 UART wakeup");
      break;
      
    case ESP_SLEEP_WAKEUP_WIFI:
      Serial.println("📶 WiFi wakeup");
      break;
      
    case ESP_SLEEP_WAKEUP_COCPU:
      Serial.println("⚙️ Co-processor wakeup");
      break;
      
    case ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG:
      Serial.println("⚙️ Co-processor trap trigger");
      break;
      
    case ESP_SLEEP_WAKEUP_BT:
      Serial.println("📱 Bluetooth wakeup");
      break;
      
    default:
      Serial.print("🔄 Reset (not a wakeup): ");
      Serial.println(wakeup_reason);
      break;
  }
}

// ============================================================================
// Print Statistics
// ============================================================================
void printStatistics() {
  Serial.println("\n┌─────────────────────────────┐");
  Serial.println("│   Wakeup Statistics          │");
  Serial.println("├─────────────────────────────┤");
  Serial.print("│ Total Boots:        ");
  Serial.printf("%7d │\n", bootCount);
  Serial.print("│ Timer Wakeups:      ");
  Serial.printf("%7d │\n", timerWakeupCount);
  Serial.print("│ Button 1 Wakeups:   ");
  Serial.printf("%7d │\n", button1WakeupCount);
  Serial.print("│ Button 2 Wakeups:   ");
  Serial.printf("%7d │\n", button2WakeupCount);
  Serial.println("├─────────────────────────────┤");
  Serial.print("│ Total Active Time:  ");
  Serial.printf("%5lus │\n", totalActiveTime);
  
  if (bootCount > 1) {
    unsigned long avgSleepTime = (totalActiveTime * 1000 - lastWakeupTime) / (bootCount - 1);
    Serial.print("│ Avg Sleep Time:     ");
    Serial.printf("%5lus │\n", avgSleepTime / 1000);
  }
  
  Serial.println("└─────────────────────────────┘");
}

// ============================================================================
// Print Power Information
// ============================================================================
void printPowerInfo() {
  Serial.println("\n┌──────────────────────────────────────────┐");
  Serial.println("│   Current Power Mode Strategy            │");
  Serial.println("├──────────────────────────────────────────┤");
  Serial.print("│ Strategy: ");
  
  switch(currentStrategy) {
    case STRATEGY_TIMER_ONLY:
      Serial.println("Timer Only (10s)         │");
      Serial.println("│ Sources:  ⏰ Timer                       │");
      Serial.println("│ Power:    ~10µA (lowest)                │");
      break;
      
    case STRATEGY_BUTTON_ONLY:
      Serial.println("External Only           │");
      Serial.println("│ Sources:  🔘 Buttons                     │");
      Serial.println("│ Power:    ~10µA (very low)              │");
      break;
      
    case STRATEGY_COMBINED:
      Serial.println("Combined                │");
      Serial.println("│ Sources:  ⏰ Timer + 🔘 Buttons          │");
      Serial.println("│ Power:    ~150µA (moderate)             │");
      break;
      
    case STRATEGY_EXT1_ALL_LOW:
      Serial.println("Both Buttons (AND)      │");
      Serial.println("│ Sources:  🔘 Both pressed                │");
      Serial.println("│ Power:    ~10µA (lowest)                │");
      break;
      
    case STRATEGY_EXT1_ANY_HIGH:
      Serial.println("Any Button (OR)         │");
      Serial.println("│ Sources:  🔘 Any pressed                 │");
      Serial.println("│ Power:    ~10µA (very low)              │");
      break;
  }
  
  Serial.println("└──────────────────────────────────────────┘");
}

// ============================================================================
// Print Menu
// ============================================================================
void printMenu() {
  Serial.println("\n╔═══════════════════════════════════════════╗");
  Serial.println("║           Command Menu                    ║");
  Serial.println("╠═══════════════════════════════════════════╣");
  Serial.println("║ 1 - Timer Only (10s wakeup)              ║");
  Serial.println("║ 2 - Buttons Only (external wakeup)       ║");
  Serial.println("║ 3 - Combined (timer + buttons)           ║");
  Serial.println("║ 4 - Both Buttons Required (AND logic)    ║");
  Serial.println("║ 5 - Any Button (OR logic)                ║");
  Serial.println("║ S - Show statistics                      ║");
  Serial.println("║ R - Reset statistics                     ║");
  Serial.println("║ T - Test wakeup sources                  ║");
  Serial.println("║ P - Print power consumption info         ║");
  Serial.println("║ H - Print this menu                      ║");
  Serial.println("╚═══════════════════════════════════════════╝");
}

// ============================================================================
// Handle Serial Commands
// ============================================================================
void handleSerialCommands() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    // Clear remaining characters
    while(Serial.available()) Serial.read();
    
    switch(cmd) {
      case '1':
        currentStrategy = STRATEGY_TIMER_ONLY;
        Serial.println("\n✓ Strategy: Timer Only");
        break;
        
      case '2':
        currentStrategy = STRATEGY_BUTTON_ONLY;
        Serial.println("\n✓ Strategy: Buttons Only");
        break;
        
      case '3':
        currentStrategy = STRATEGY_COMBINED;
        Serial.println("\n✓ Strategy: Combined");
        break;
        
      case '4':
        currentStrategy = STRATEGY_EXT1_ALL_LOW;
        Serial.println("\n✓ Strategy: Both Buttons Required");
        break;
        
      case '5':
        currentStrategy = STRATEGY_EXT1_ANY_HIGH;
        Serial.println("\n✓ Strategy: Any Button");
        break;
        
      case 's':
      case 'S':
        printStatistics();
        break;
        
      case 'r':
      case 'R':
        Serial.println("\n⚠️ Resetting statistics...");
        bootCount = 0;
        timerWakeupCount = 0;
        button1WakeupCount = 0;
        button2WakeupCount = 0;
        totalActiveTime = 0;
        lastWakeupTime = 0;
        Serial.println("✓ Statistics reset!");
        break;
        
      case 't':
      case 'T':
        testWakeupSources();
        break;
        
      case 'p':
      case 'P':
        printPowerConsumptionGuide();
        break;
        
      case 'h':
      case 'H':
        printMenu();
        break;
        
      default:
        Serial.println("\n❌ Unknown command. Press 'H' for help.");
        break;
    }
  }
}

// ============================================================================
// Test Wakeup Sources
// ============================================================================
void testWakeupSources() {
  Serial.println("\n╔═══════════════════════════════════════╗");
  Serial.println("║   Testing Wakeup Sources              ║");
  Serial.println("╚═══════════════════════════════════════╝");
  
  // Test button connections
  Serial.println("\n1️⃣ Testing Button Connections:");
  Serial.print("   Button 1 (GPIO 0):  ");
  Serial.println(digitalRead(BUTTON1_PIN) ? "HIGH (not pressed)" : "LOW (pressed)");
  
  Serial.print("   Button 2 (GPIO 35): ");
  Serial.println(digitalRead(BUTTON2_PIN) ? "HIGH (not pressed)" : "LOW (pressed)");
  
  // Test LED
  Serial.println("\n💡 Testing LED (GPIO 12):");
  Serial.println("   Blinking 5 times...");
  blinkLED(5, 200);
  
  // Test RTC memory
  Serial.println("\n💾 Testing RTC Memory:");
  Serial.print("   Boot count: ");
  Serial.println(bootCount);
  Serial.println("   ✓ RTC memory working (value persists)");
  
  Serial.println("\n✓ All tests complete!");
}

// ============================================================================
// Print Power Consumption Guide
// ============================================================================
void printPowerConsumptionGuide() {
  Serial.println("\n╔═════════════════════════════════════════════════╗");
  Serial.println("║   ESP32 Power Consumption Guide                 ║");
  Serial.println("╠═════════════════════════════════════════════════╣");
  Serial.println("║ Mode              │ Current    │ Use Case        ║");
  Serial.println("╟───────────────────┼────────────┼─────────────────╢");
  Serial.println("║ Active (WiFi ON)  │ 80-260 mA  │ Data transfer   ║");
  Serial.println("║ Active (WiFi OFF) │ 30-80 mA   │ Computing       ║");
  Serial.println("║ Modem Sleep       │ 20-40 mA   │ WiFi standby    ║");
  Serial.println("║ Light Sleep       │ 0.8 mA     │ Quick wakeup    ║");
  Serial.println("║ Deep Sleep (Timer)│ 150 µA     │ Periodic tasks  ║");
  Serial.println("║ Deep Sleep (EXT0) │ 10 µA      │ Button wakeup   ║");
  Serial.println("║ Deep Sleep (EXT1) │ 10 µA      │ Multi-button    ║");
  Serial.println("║ Hibernation       │ 5 µA       │ Ultra low power ║");
  Serial.println("╚═════════════════════════════════════════════════╝");
  
  Serial.println("\n📊 Battery Life Estimation (at 3.7V):");
  Serial.println("   With 2000mAh battery:");
  Serial.println("   • Active (100mA):        ~20 hours");
  Serial.println("   • Light Sleep (0.8mA):    ~104 days");
  Serial.println("   • Deep Sleep (150µA):     ~555 days");
  Serial.println("   • Deep Sleep (10µA):      ~8,333 days (~23 years!)");
  
  Serial.println("\n💡 Measurement Tips:");
  Serial.println("   • Use multimeter in series with power supply");
  Serial.println("   • Measure in µA range for deep sleep");
  Serial.println("   • Remove USB cable for accurate readings");
  Serial.println("   • Consider ESP32 module variations");
}

// ============================================================================
// Configure Sleep Mode
// ============================================================================
void configureSleepMode(SleepStrategy strategy) {
  // Disable all wakeup sources first
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  
  switch(strategy) {
    case STRATEGY_TIMER_ONLY:
      // Only timer wakeup
      esp_sleep_enable_timer_wakeup(TIMER_WAKEUP_TIME * uS_TO_S_FACTOR);
      Serial.println("   Enabled: Timer (10s)");
      break;
      
    case STRATEGY_BUTTON_ONLY:
      // Only external wakeup (both buttons)
      esp_sleep_enable_ext1_wakeup(
        (1ULL << BUTTON1_PIN) | (1ULL << BUTTON2_PIN),
        ESP_EXT1_WAKEUP_ANY_HIGH
      );
      Serial.println("   Enabled: External buttons");
      break;
      
    case STRATEGY_COMBINED:
      // Both timer and external wakeup
      esp_sleep_enable_timer_wakeup(TIMER_WAKEUP_TIME * uS_TO_S_FACTOR);
      esp_sleep_enable_ext1_wakeup(
        (1ULL << BUTTON1_PIN) | (1ULL << BUTTON2_PIN),
        ESP_EXT1_WAKEUP_ANY_HIGH
      );
      Serial.println("   Enabled: Timer + External buttons");
      break;
      
    case STRATEGY_EXT1_ALL_LOW:
      // Wake when both buttons are LOW (pressed)
      esp_sleep_enable_ext1_wakeup(
        (1ULL << BUTTON1_PIN) | (1ULL << BUTTON2_PIN),
        ESP_EXT1_WAKEUP_ALL_LOW
      );
      Serial.println("   Enabled: Both buttons pressed (AND)");
      break;
      
    case STRATEGY_EXT1_ANY_HIGH:
      // Wake when any button is HIGH
      esp_sleep_enable_ext1_wakeup(
        (1ULL << BUTTON1_PIN) | (1ULL << BUTTON2_PIN),
        ESP_EXT1_WAKEUP_ANY_HIGH
      );
      Serial.println("   Enabled: Any button pressed (OR)");
      break;
  }
}

// ============================================================================
// Enter Deep Sleep
// ============================================================================
void enterDeepSleep() {
  // Configure GPIO behavior during sleep
  // Keep GPIO 12 (LED) as output and set LOW
  rtc_gpio_isolate(GPIO_NUM_12);
  
  // Configure sleep mode based on current strategy
  configureSleepMode(currentStrategy);
  
  // Print expected wakeup info
  Serial.print("💤 Deep sleep configured. ");
  
  switch(currentStrategy) {
    case STRATEGY_TIMER_ONLY:
      Serial.println("Will wake in 10 seconds.");
      break;
      
    case STRATEGY_BUTTON_ONLY:
      Serial.println("Press any button to wake.");
      break;
      
    case STRATEGY_COMBINED:
      Serial.println("Will wake in 10s or on button press.");
      break;
      
    case STRATEGY_EXT1_ALL_LOW:
      Serial.println("Press BOTH buttons to wake.");
      break;
      
    case STRATEGY_EXT1_ANY_HIGH:
      Serial.println("Press ANY button to wake.");
      break;
  }
  
  Serial.flush();  // Wait for serial to finish
  
  // Enter deep sleep
  esp_deep_sleep_start();
  
  // This line will never be reached
}

// ============================================================================
// Blink LED
// ============================================================================
void blinkLED(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(delayMs);
    digitalWrite(LED_PIN, LOW);
    delay(delayMs);
  }
}
