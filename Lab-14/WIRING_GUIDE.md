# Wiring Guide - ESP32 MQTT Weather Station

## 📐 Complete Wiring Diagram

```
                          ESP32-WROOM-32
                    ┌─────────────────────┐
                    │                     │
              3.3V ─┤ 3V3           GND   ├─ GND
   BME280_VCC ──────┤                     │
   DHT11_VCC ───────┤                     │
                    │                     │
              GND ──┤ GND            D21  ├─ BME280_SDA
   BME280_GND ──────┤                     │
   DHT11_GND ───────┤                D22  ├─ BME280_SCL
   LED_Cathode ─────┤                     │
                    │                D15  ├─ DHT11_DATA
                    │                     │
   [220Ω] ──────────┤ D12            D0   │
   LED_Anode        │                     │
                    │                     │
                    └─────────────────────┘
```

---

## 🔌 Component Connections

### 1️⃣ BME280 Sensor (Temperature, Humidity, Pressure)

**Connection Type:** I2C (Inter-Integrated Circuit)

| BME280 Pin | ESP32 Pin | Description         |
| ---------- | --------- | ------------------- |
| VCC (VIN)  | 3.3V      | Power supply (3.3V) |
| GND        | GND       | Ground              |
| SDA        | GPIO 21   | I2C Data line       |
| SCL        | GPIO 22   | I2C Clock line      |

**Notes:**

- ⚠️ Use 3.3V, NOT 5V (BME280 is 3.3V logic level)
- Default I2C address is usually 0x76 or 0x77
- Code auto-detects both addresses

**Detailed Breadboard Connection:**

```
BME280 Module → ESP32
─────────────────────
VCC (Red wire)    → 3.3V pin
GND (Black wire)  → GND pin
SDA (Blue wire)   → GPIO 21 (D21)
SCL (Yellow wire) → GPIO 22 (D22)
```

---

### 2️⃣ DHT11 Sensor (Temperature & Humidity)

**Connection Type:** Single-Wire Digital

| DHT11 Pin | ESP32 Pin | Description  |
| --------- | --------- | ------------ |
| VCC (+)   | 3.3V      | Power supply |
| DATA      | GPIO 15   | Data signal  |
| GND (-)   | GND       | Ground       |

**DHT11 Pin Layout (3-pin module):**

```
Front View:
   ┌───────┐
   │  ≡≡≡  │
   │  ≡≡≡  │
   │  ≡≡≡  │
   └─┬─┬─┬─┘
     │ │ │
     │ │ └── GND (-)
     │ └──── DATA (out)
     └────── VCC (+)
```

**4-Pin DHT11 Module:**

```
     ┌─────────┐
     │  DHT11  │
     │  ≡≡≡≡≡≡ │
     └┬─┬─┬─┬──┘
      │ │ │ │
      1 2 3 4

1. VCC  → 3.3V
2. DATA → GPIO 15
3. NC   → Not connected
4. GND  → GND
```

**Notes:**

- ⚠️ Can use 3.3V or 5V (5V for longer cable runs)
- Built-in pull-up resistor on most modules
- If using bare sensor, add 10kΩ pull-up resistor between DATA and VCC

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
         │  BME280    Position A1-A4       │
      3  │  ┌───┐                          │
      4  │  └───┘                          │
         │                                 │
      5  │  DHT11     Position E5-E7       │
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
• BME280: Row 1-4, connected to ESP32 via jumpers
• DHT11: Row 5-7, DATA to GPIO 15
• LED + Resistor: Rows 8-9, GPIO 12 → Resistor → LED → GND
```

---

## 🎨 Color-Coded Wiring (Recommended)

Use consistent wire colors for easy debugging:

| Wire Color | Purpose        | Example Connections                |
| ---------- | -------------- | ---------------------------------- |
| 🔴 Red     | Power (3.3V)   | ESP32 3.3V → BME280 VCC, DHT11 VCC |
| ⚫ Black   | Ground         | ESP32 GND → All GND connections    |
| 🔵 Blue    | I2C SDA        | ESP32 GPIO 21 → BME280 SDA         |
| 🟡 Yellow  | I2C SCL        | ESP32 GPIO 22 → BME280 SCL         |
| 🟢 Green   | Data Signal    | ESP32 GPIO 15 → DHT11 DATA         |
| 🟠 Orange  | Control Signal | ESP32 GPIO 12 → LED Resistor       |

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

### Step 4: Connect BME280 Sensor

1. Insert BME280 module into breadboard
2. Connect **BME280 VCC** to **+ rail** (3.3V)
3. Connect **BME280 GND** to **- rail**
4. Connect **BME280 SDA** to **ESP32 GPIO 21**
5. Connect **BME280 SCL** to **ESP32 GPIO 22**

### Step 5: Connect DHT11 Sensor

1. Insert DHT11 module into breadboard
2. Connect **DHT11 VCC (+)** to **+ rail** (3.3V)
3. Connect **DHT11 GND (-)** to **- rail**
4. Connect **DHT11 DATA** to **ESP32 GPIO 15**

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
- ✅ Ensure I2C connections are correct
- ✅ Verify no loose connections

---

## 📸 Visual Reference Guide

### ESP32 Pinout Reference

```
                    ESP32-WROOM-32 Pinout
                         (Top View)

         ┌─────────────────────────┐
     EN  ┤                         ├ D23
    VP36 ┤                         ├ D22 ← SCL (BME280)
    VN39 ┤                         ├ TX0
     D34 ┤                         ├ RX0
     D35 ┤                         ├ D21 ← SDA (BME280)
     D32 ┤       [USB PORT]        ├ GND
     D33 ┤                         ├ D19
     D25 ┤                         ├ D18
     D26 ┤                         ├ D5
     D27 ┤        ESP32            ├ D17
     D14 ┤                         ├ D16
     D12 ┤ ← LED (with resistor)   ├ D4
     GND ┤                         ├ D0
     D13 ┤                         ├ D2
     SD2 ┤                         ├ D15 ← DHT11 DATA
     SD3 ┤                         ├ 3V3 ← Power (+3.3V)
     CMD ┤                         ├ GND
     5V  └─────────────────────────┘ GND
```

### I2C Bus Configuration

```
I2C Bus (BME280):
─────────────────
   ESP32          BME280
   GPIO 21  ───→  SDA (Data)
   GPIO 22  ───→  SCL (Clock)
   3.3V     ───→  VCC
   GND      ───→  GND

Both SDA and SCL are bidirectional
Multiple I2C devices can share same bus
```

---

## ⚠️ Common Wiring Mistakes

### ❌ Mistake 1: Using 5V for BME280

**Problem:** BME280 is damaged by 5V
**Solution:** Always use 3.3V for BME280

### ❌ Mistake 2: Reversed LED Polarity

**Problem:** LED doesn't light up
**Solution:** Longer leg (+) to resistor, shorter leg (-) to GND

### ❌ Mistake 3: No Resistor with LED

**Problem:** LED burns out, GPIO pin may be damaged
**Solution:** Always use 220Ω resistor in series with LED

### ❌ Mistake 4: Swapped SDA/SCL

**Problem:** BME280 not detected
**Solution:** SDA→GPIO 21, SCL→GPIO 22

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

### Test 2: I2C Scanner

```cpp
#include <Wire.h>

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);
    Serial.println("I2C Scanner");
}

void loop() {
    for(byte i = 1; i < 127; i++) {
        Wire.beginTransmission(i);
        if(Wire.endTransmission() == 0) {
            Serial.print("Device found at 0x");
            Serial.println(i, HEX);
        }
    }
    delay(5000);
}
```

**Expected Output:** `Device found at 0x76` or `0x77` (BME280)

### Test 3: DHT11 Test

```cpp
#include <DHT.h>
#define DHT_PIN 15
DHT dht(DHT_PIN, DHT11);

void setup() {
    Serial.begin(115200);
    dht.begin();
}

void loop() {
    float temp = dht.readTemperature();
    Serial.print("Temperature: ");
    Serial.println(temp);
    delay(2000);
}
```

**Expected Output:** Temperature readings (not NaN)

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
| Signal (I2C, DHT) | 24-26 AWG  | Thinner is fine       |
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
| BME280 not detected   | Wrong I2C pins               | Verify GPIO 21 (SDA), GPIO 22 (SCL)     |
| DHT11 returns NaN     | Loose DATA connection        | Check GPIO 15 connection                |
| LED not lighting      | Reversed polarity            | Swap LED leads                          |
| LED very dim          | No current-limiting resistor | Check resistor value (220Ω)             |
| ESP32 won't boot      | Short circuit                | Disconnect everything, test ESP32 alone |
| Intermittent readings | Loose wires                  | Press all connections firmly            |

---

## 📚 Additional Resources

- [ESP32 Pinout Reference](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
- [BME280 Sensor Guide](https://learn.adafruit.com/adafruit-bme280-humidity-barometric-pressure-temperature-sensor-breakout)
- [DHT11 Sensor Tutorial](https://learn.adafruit.com/dht)
- [Breadboard Basics](https://learn.sparkfun.com/tutorials/how-to-use-a-breadboard)

---

**Wiring Complete! Ready to Upload Code! 🎉**
