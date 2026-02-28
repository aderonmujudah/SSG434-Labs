/*
 * Simple Bluetooth Test - Diagnose Connection Issues
 * 
 * This code helps identify if Bluetooth is working at all.
 * Upload this, then try sending any character from your phone.
 * 
 * Expected behavior:
 * - Serial Monitor shows what you send
 * - Phone receives an echo back
 */

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please enable it in menuconfig
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n========================================");
  Serial.println("Bluetooth Test Started!");
  Serial.println("========================================");
  
  SerialBT.begin("ESP32_Test"); // Bluetooth device name
  
  Serial.println("Device Name: ESP32_Test");
  Serial.println("Waiting for phone to connect...");
  Serial.println("========================================\n");
}

void loop() {
  // Print status every 5 seconds
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 5000) {
    lastStatus = millis();
    Serial.println("[STATUS] Running... waiting for Bluetooth data");
    Serial.print("[STATUS] Connected clients: ");
    Serial.println(SerialBT.hasClient() ? "YES" : "NO");
  }
  
  // Check for incoming Bluetooth data
  if (SerialBT.available()) {
    char receivedChar = SerialBT.read();
    
    // Print to Serial Monitor
    Serial.println("\n>>> DATA RECEIVED! <<<");
    Serial.print("    Character: '");
    Serial.print(receivedChar);
    Serial.print("' (ASCII code: ");
    Serial.print((int)receivedChar);
    Serial.println(")");
    
    // Send echo back to phone
    SerialBT.print("Echo: ");
    SerialBT.println(receivedChar);
    SerialBT.println("Working! Bluetooth is OK!");
    
    Serial.println("    Echo sent back to phone");
    Serial.println(">>> SUCCESS! <<<\n");
  }
  
  delay(10);
}
