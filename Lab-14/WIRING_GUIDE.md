# Wiring Guide - ESP32 MQTT Analog Sensor Station

## 📐 Complete Wiring Diagram

```
                          ESP32-WROOM-32
                      ┌─────────────────────┐
                      │                     │
                  3.3V ─┤ 3V3           GND   ├─ GND
           HW495_VCC ───────┤                     │
           HW485_VCC ───────┤                     │
                      │                     │
                  GND ──┤ GND            D34  ├─ HW495_AO
           HW495_GND ───────┤                     │
           HW485_GND ───────┤                D35  ├─ HW485_AO
           LED_Cathode ─────┤                     │
                      │                     │
           [220Ω] ──────────┤ D12            D0   │
           LED_Anode        │                     │
                      │                     │
                      └─────────────────────┘
```

---

## 🔌 Component Connections

### 1️⃣ HW-495 Hall Sensor (Magnetic Field)

**Connection Type:** Analog (ADC)

| HW-495 Pin | ESP32 Pin | Description         |
| ---------- | --------- | ------------------- |
| VCC        | 3.3V      | Power supply (3.3V) |
| GND        | GND       | Ground              |
| AO         | GPIO 34   | Analog output       |

**Notes:**

- ⚠️ Use 3.3V, NOT 5V (ESP32 ADC is 3.3V)
- GPIO 34 is input-only (safe for ADC)

**Detailed Breadboard Connection:**

```
HW-495 Module → ESP32
─────────────────────
VCC (Red wire)   → 3.3V pin
GND (Black wire) → GND pin
AO (Green wire)  → GPIO 34 (D34)
```

---

### 2️⃣ HW-485 Microphone Module (Sound Intensity)

**Connection Type:** Analog (ADC)

| HW-485 Pin | ESP32 Pin | Description         |
| ---------- | --------- | ------------------- |
| VCC        | 3.3V      | Power supply (3.3V) |
| GND        | GND       | Ground              |
| AO         | GPIO 35   | Analog output       |

**Notes:**

- ⚠️ Use 3.3V, NOT 5V (ESP32 ADC is 3.3V)
- Keep AO wire short to reduce noise

**Detailed Breadboard Connection:**

```
HW-485 Module → ESP32
─────────────────────
VCC (Red wire)   → 3.3V pin
GND (Black wire) → GND pin
AO (Green wire)  → GPIO 35 (D35)
```

---

### 3️⃣ LED Indicator

**Connection Type:** Digital Output

| Component       | ESP32 Pin | Connection        |
| --------------- | --------- | ----------------- |
| LED Anode (+)   | GPIO 12   | Via 220Ω resistor |
| LED Cathode (-) | GND       | Direct            |

**LED Circuit:**

```
ESP32 GPIO 12 ──→ [220Ω Resistor] ──→ LED (+) ──→ LED (-) ──→ GND
```

**LED Polarity:**

```
    LED Symbol:

    Anode (+)     Cathode (-)
        │             │
        ▼             │
       ───►│──────────┘
            │
         (Triangle points to cathode)

    Physical LED:

    Longer leg = Anode (+)
    Shorter leg = Cathode (-)
    Flat edge on LED = Cathode (-)
```

**Notes:**

- ⚠️ Always use current-limiting resistor (220Ω recommended)
- Without resistor, LED may burn out
- Check LED polarity carefully

---

## 🍞 Breadboard Layout

```
                    Breadboard Layout
     ═══════════════════════════════════════════
     Rail   A  B  C  D  E    F  G  H  I  J   Rail
     ─── ┌─────────────────────────────────┐ ───
      +  │                                 │  +
     ─── │  Power Rails                    │ ───
      -  │                                 │  -
     ─── └─────────────────────────────────┘ ───
         ┌─────────────────────────────────┐
      1  │  [ESP32 Module Pins]            │
      2  │                                 │
            │  HW-495   Position A1-A3        │
        3  │  ┌───┐                          │
        4  │  └───┘                          │
            │                                 │
        5  │  HW-485   Position E5-E7        │
        6  │  ┌───┐                          │
        7  │  └───┘                          │
         │                                 │
      8  │  [220Ω]    Position B8          │
      9  │  LED        Position C9         │
         │                                 │
     10  │                                 │
         └─────────────────────────────────┘

Connection Summary:
• Power Rails: 3.3V from ESP32 to + rail, GND to - rail
• HW-495: Row 1-3, AO to GPIO 34
• HW-485: Row 5-7, AO to GPIO 35
• LED + Resistor: Rows 8-9, GPIO 12 → Resistor → LED → GND
```

---

## 🎨 Color-Coded Wiring (Recommended)

Use consistent wire colors for easy debugging:

| Wire Color | Purpose        | Example Connections                 |
| ---------- | -------------- | ----------------------------------- |
| 🔴 Red     | Power (3.3V)   | ESP32 3.3V → HW-495 VCC, HW-485 VCC |
| ⚫ Black   | Ground         | ESP32 GND → All GND connections     |
| 🟢 Green   | Analog Signal  | ESP32 GPIO 34 → HW-495 AO           |
| 🔵 Blue    | Analog Signal  | ESP32 GPIO 35 → HW-485 AO           |
| 🟠 Orange  | Control Signal | ESP32 GPIO 12 → LED Resistor        |

---

## 🔧 Step-by-Step Assembly Guide

### Step 1: Prepare Components

1. ✅ Gather all components
2. ✅ Check ESP32 board for damage
3. ✅ Verify sensor modules are working
4. ✅ Test LED with battery (check polarity)

### Step 2: Insert ESP32 into Breadboard

1. Place ESP32 across center gap of breadboard
2. Ensure both sides have accessible rows
3. Press firmly but gently

### Step 3: Setup Power Rails

1. Connect ESP32 **3.3V** pin to breadboard **+ rail** (red wire)
2. Connect ESP32 **GND** pin to breadboard **- rail** (black wire)
3. ⚠️ Double-check: Use 3.3V, NOT 5V for sensors

### Step 4: Connect HW-495 Hall Sensor

1. Insert HW-495 module into breadboard
2. Connect **HW-495 VCC** to **+ rail** (3.3V)
3. Connect **HW-495 GND** to **- rail**
4. Connect **HW-495 AO** to **ESP32 GPIO 34**

### Step 5: Connect HW-485 Microphone Module

1. Insert HW-485 module into breadboard
2. Connect **HW-485 VCC** to **+ rail** (3.3V)
3. Connect **HW-485 GND** to **- rail**
4. Connect **HW-485 AO** to **ESP32 GPIO 35**

### Step 6: Connect LED

1. Insert **220Ω resistor** into breadboard
2. Connect one end of resistor to **ESP32 GPIO 12**
3. Connect other end of resistor to **LED anode (+)** [longer leg]
4. Connect **LED cathode (-)** [shorter leg] to **- rail** (GND)
5. ⚠️ Verify LED polarity before powering on

### Step 7: Final Check

Before powering on:

- ✅ Verify all power connections (3.3V, GND)
- ✅ Check no short circuits between power rails
- ✅ Confirm LED polarity
- ✅ Ensure analog connections are correct (GPIO 34/35)
- ✅ Verify no loose connections

---

## 📸 Visual Reference Guide

### ESP32 Pinout Reference

```
                    ESP32-WROOM-32 Pinout
                         (Top View)

         ┌─────────────────────────┐
     EN  ┤                         ├ D23
    VP36 ┤                         ├ D22
    VN39 ┤                         ├ TX0
     D34 ┤ ← HW-495 AO              ├ RX0
     D35 ┤ ← HW-485 AO              ├ D21
     D32 ┤       [USB PORT]        ├ GND
     D33 ┤                         ├ D19
     D25 ┤                         ├ D18
     D26 ┤                         ├ D5
     D27 ┤        ESP32            ├ D17
     D14 ┤                         ├ D16
     D12 ┤ ← LED (with resistor)   ├ D4
     GND ┤                         ├ D0
     D13 ┤                         ├ D2
    SD2 ┤                         ├ D15
     SD3 ┤                         ├ 3V3 ← Power (+3.3V)
     CMD ┤                         ├ GND
     5V  └─────────────────────────┘ GND
```

### ADC Input Notes

```
ADC Inputs (Analog Sensors):
────────────────────────────
    ESP32          Sensor
    GPIO 34  ───→  HW-495 AO
    GPIO 35  ───→  HW-485 AO
    3.3V     ───→  VCC
    GND      ───→  GND

Use 3.3V sensors and keep analog wires short to reduce noise.
```

---

## ⚠️ Common Wiring Mistakes

### ❌ Mistake 1: Using 5V for analog sensors

**Problem:** ESP32 ADC input can be damaged or saturated by 5V
**Solution:** Always use 3.3V for HW-495 and HW-485

### ❌ Mistake 2: Reversed LED Polarity

**Problem:** LED doesn't light up
**Solution:** Longer leg (+) to resistor, shorter leg (-) to GND

### ❌ Mistake 3: No Resistor with LED

**Problem:** LED burns out, GPIO pin may be damaged
**Solution:** Always use 220Ω resistor in series with LED

### ❌ Mistake 4: Wrong AO pin

**Problem:** Flat sensor readings
**Solution:** HW-495 AO → GPIO 34, HW-485 AO → GPIO 35

### ❌ Mistake 5: Loose Connections

**Problem:** Intermittent sensor readings
**Solution:** Ensure all wires are firmly inserted

---

## 🧪 Connection Testing

### Test 1: Power Test

```cpp
// Upload this simple sketch first
void setup() {
    pinMode(12, OUTPUT);
}
void loop() {
    digitalWrite(12, HIGH);
    delay(500);
    digitalWrite(12, LOW);
    delay(500);
}
```

**Expected:** LED blinks every 500ms

### Test 2: Analog Read Test

```cpp
void setup() {
    Serial.begin(115200);
    analogReadResolution(12);
}

void loop() {
    int hall = analogRead(34);
    int mic = analogRead(35);
    Serial.print("Hall: ");
    Serial.print(hall);
    Serial.print(" | Mic: ");
    Serial.println(mic);
    delay(500);
}
```

**Expected Output:** Values should change when you move a magnet near the hall sensor or make noise near the microphone.

---

## 📦 Wire Management Tips

1. **Use appropriate wire lengths** - Not too long, not too short
2. **Route wires neatly** - Avoid crossing wires when possible
3. **Group by function** - Keep power wires together, signal wires together
4. **Label connections** - Use tape or labels for complex setups
5. **Strain relief** - Don't pull on sensor modules

---

## 🔌 Alternative Connection Methods

### Using JST Connectors

- More permanent connection
- Easier to disconnect/reconnect
- Recommended for projects in enclosures

### Using Dupont Wires

- Quick prototyping
- Easy to reconfigure
- Can connect directly to sensor modules

### Soldered Connections

- Most reliable for permanent installations
- Use for production devices
- Apply heat shrink tubing for insulation

---

## 📏 Wire Gauge Recommendations

| Connection Type   | Wire Gauge | Notes                 |
| ----------------- | ---------- | --------------------- |
| Power (3.3V, GND) | 22-24 AWG  | Thicker for stability |
| Signal (ADC)      | 24-26 AWG  | Thinner is fine       |
| LED               | 26-28 AWG  | Low current           |

---

## 🛡️ Safety Checklist

Before powering on:

- [ ] No crossed wires between 3.3V and GND
- [ ] LED has current-limiting resistor
- [ ] All sensors using 3.3V (not 5V)
- [ ] ESP32 not connected to USB while using external power
- [ ] All connections are firm and secure
- [ ] Breadboard is on non-conductive surface

---

## 🔍 Troubleshooting Connection Issues

| Symptom               | Possible Cause               | Solution                                |
| --------------------- | ---------------------------- | --------------------------------------- |
| Hall sensor flat      | Wrong AO pin                 | Verify GPIO 34 connection               |
| Mic sensor flat       | Wrong AO pin                 | Verify GPIO 35 connection               |
| LED not lighting      | Reversed polarity            | Swap LED leads                          |
| LED very dim          | No current-limiting resistor | Check resistor value (220Ω)             |
| ESP32 won't boot      | Short circuit                | Disconnect everything, test ESP32 alone |
| Intermittent readings | Loose wires                  | Press all connections firmly            |

---

## 📚 Additional Resources

- [ESP32 Pinout Reference](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
- [ESP32 ADC Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html)
- [Breadboard Basics](https://learn.sparkfun.com/tutorials/how-to-use-a-breadboard)

---

**Wiring Complete! Ready to Upload Code! 🎉**
