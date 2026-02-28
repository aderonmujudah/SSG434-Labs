# Wiring Diagram & Circuit Details

## Complete Circuit Diagram

```
                                  ESP32 Development Board
                                ┌─────────────────────┐
                                │                     │
                                │      ESP32-WROOM    │
                                │                     │
                                │                     │
    3.3V ────────────────┬──────┤ 3.3V            GND ├──────┬────────────────── GND
                         │      │                     │      │
                    ┌────┴────┐ │                     │      │
                    │ DHT11   │ │                     │      │
                    │         │ │                     │      │
                    │  1  2  3│ │                     │      │
                    └─┬──┬──┬─┘ │                     │      │
                      │  │  │   │                     │      │
          VCC ────────┘  │  └───┼──────────────── GND│      │
                         │      │                     │      │
                    10kΩ │      │                     │      │
          3.3V ──────▯▯▯─┘      │                     │      │
                  (pull-up)     │                     │      │
                                │                     │      │
          DATA ─────────────────┤ GPIO 15             │      │
                                │                     │      │
                                │                     │      │
          LED1 Control ─────────┤ GPIO 2              │      │
                                │                     │      │
                                │                     │      │
          LED2 Control ─────────┤ GPIO 4              │      │
                                │                     │      │
                                │                     │      │
                                │                USB  │      │
                                │               [====]│      │
                                │                     │      │
                                └─────────────────────┘      │
                                                             │
                                                             │
    LED 1 Circuit                                            │
    ─────────────────                                        │
                                                             │
    GPIO 2 ────▯▯▯▯────┬────▶|──────────────────────────────┘
              220Ω     │    LED1
                       │   (Anode)
                       │
                       │

    LED 2 Circuit
    ─────────────────

    GPIO 4 ────▯▯▯▯────┬────▶|──────────────────────────────┘
              220Ω     │    LED2
                       │   (Anode)
                       │

    Legend:
    ────    Wire connection
    ▯▯▯▯    Resistor
    ──▶|    LED (arrow pointing to anode/long leg)
```

## Breadboard Layout

```
Breadboard View (Top Down):

    5V Rail    3.3V Rail                                    GND Rail
    ═══════    ═════════                                    ════════
                  │                                              │
                  ├──────── DHT11 (VCC) ─── (DATA) ──── (GND) ──┤
                  │            │              │            │     │
                  │            │         10kΩ │            │     │
                  │            └──────────┴────┘           │     │
                  │                      │                 │     │
                  │                      │                 │     │
                  │                   GPIO 15              │     │
                  │                      │                 │     │
                  │                ESP32 Board             │     │
    ═══════       └─────────────┬────────┬──────┬──────────┴═════════
    5V Rail                     │        │      │             GND Rail
                             3.3V     GPIO 2  GPIO 4
                                        │      │
                                      220Ω   220Ω
                                        │      │
                                      LED1   LED2
                                        │      │
                                        └──────┴────────────────────┘
                                                                 GND Rail
```

## Detailed Component Connections

### DHT11 Temperature & Humidity Sensor

**DHT11 Pinout (facing you):**

```
     ┌─────────┐
     │  DHT11  │
     │  ┌───┐  │
     │  └───┘  │
     └─┬─┬─┬─┬─┘
       1 2 3 4
```

| Pin | Name | Connection                           | Notes        |
| --- | ---- | ------------------------------------ | ------------ |
| 1   | VCC  | ESP32 3.3V                           | Power supply |
| 2   | DATA | ESP32 GPIO 15 + 10kΩ pull-up to 3.3V | Signal pin   |
| 3   | NC   | Not connected                        | Not used     |
| 4   | GND  | ESP32 GND                            | Ground       |

**Important:** Pin 2 (DATA) needs a 10kΩ pull-up resistor to 3.3V. Some DHT11 modules have this built-in.

### LED 1 Connection

```
ESP32 GPIO 2 ──┬── 220Ω Resistor ──┬── LED Anode (+, long leg)
               │                    │
               └────────────────────┴── LED Cathode (-, short leg) ── GND
```

**LED Polarity:**

- Anode (+): Long leg, connects to resistor from GPIO
- Cathode (-): Short leg, flat side of LED casing, connects to GND

### LED 2 Connection

```
ESP32 GPIO 4 ──┬── 220Ω Resistor ──┬── LED Anode (+, long leg)
               │                    │
               └────────────────────┴── LED Cathode (-, short leg) ── GND
```

## Resistor Color Codes

### 220Ω Resistor (for LEDs)

```
┌─────────────────┐
│ Red Red Brown   │  = 220Ω
│ ▐█▌ ▐█▌ ▐█▌     │
└─────────────────┘
 (2) (2) (×10)
```

**Bands:** Red - Red - Brown - Gold (5% tolerance)

### 10kΩ Resistor (for DHT11 pull-up)

```
┌─────────────────┐
│ Brown Black Orange│ = 10,000Ω (10kΩ)
│  ▐█▌  ▐█▌  ▐█▌   │
└─────────────────┘
  (1)  (0)  (×1k)
```

**Bands:** Brown - Black - Orange - Gold (5% tolerance)

## Power Requirements

| Component  | Voltage           | Current                   | Notes                     |
| ---------- | ----------------- | ------------------------- | ------------------------- |
| ESP32      | 3.3V (5V to VIN)  | ~250mA active, ~80mA idle | USB provides 5V           |
| DHT11      | 3.3V - 5V         | 0.5-2.5mA                 | Works fine on 3.3V        |
| LED (each) | 2-3V forward drop | ~10-20mA                  | With 220Ω resistor        |
| **Total**  | 5V USB            | ~300mA max                | Standard USB port is fine |

## Step-by-Step Wiring Instructions

### Step 1: Power Rails Setup

1. Connect ESP32 **3.3V** pin to breadboard positive (+) rail (red)
2. Connect ESP32 **GND** pin to breadboard negative (-) rail (blue/black)

### Step 2: DHT11 Sensor

1. Insert DHT11 sensor into breadboard
2. Connect DHT11 **Pin 1 (VCC)** to positive rail (3.3V)
3. Connect DHT11 **Pin 4 (GND)** to negative rail (GND)
4. Connect DHT11 **Pin 2 (DATA)** to ESP32 **GPIO 15**
5. Connect **10kΩ resistor** between Pin 2 (DATA) and positive rail (3.3V) - this is the pull-up resistor

### Step 3: LED 1

1. Insert LED 1 into breadboard (note polarity!)
2. Connect LED **anode (+, long leg)** to one end of **220Ω resistor**
3. Connect other end of resistor to ESP32 **GPIO 2**
4. Connect LED **cathode (-, short leg)** to negative rail (GND)

### Step 4: LED 2

1. Insert LED 2 into breadboard (note polarity!)
2. Connect LED **anode (+, long leg)** to one end of **220Ω resistor**
3. Connect other end of resistor to ESP32 **GPIO 4**
4. Connect LED **cathode (-, short leg)** to negative rail (GND)

### Step 5: USB Connection

1. Connect ESP32 to computer via USB cable for programming and power

## Testing Individual Components

### Test 1: LED Test

Upload this simple blink code to test each LED:

```cpp
void setup() {
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
}

void loop() {
  digitalWrite(2, HIGH); // LED1 on
  delay(500);
  digitalWrite(2, LOW);  // LED1 off
  digitalWrite(4, HIGH); // LED2 on
  delay(500);
  digitalWrite(4, LOW);  // LED2 off
  delay(500);
}
```

### Test 2: DHT11 Test

Upload DHT11 example from Adafruit library:

- File → Examples → DHT sensor library → DHTtester
- Change pin to 15
- Open Serial Monitor to see readings

### Test 3: Bluetooth Test

Upload simple Bluetooth echo code:

```cpp
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Test");
  Serial.println("Bluetooth Started");
}

void loop() {
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
}
```

## Common Wiring Mistakes

❌ **Wrong LED polarity** - LED won't light up
✅ Check: Long leg = anode (+), short leg = cathode (-)

❌ **No pull-up resistor on DHT11** - Unreliable readings
✅ Add 10kΩ resistor between DATA and 3.3V

❌ **DHT11 powered with 5V** - Can damage sensor over time
✅ Use 3.3V from ESP32 3.3V pin

❌ **No current-limiting resistor on LEDs** - Can damage LED or ESP32
✅ Always use 220Ω (or similar) resistor with LEDs

❌ **Wrong GPIO pins** - Components won't respond
✅ Match code pin numbers with physical connections

## Safety Notes

⚠️ **Never connect LEDs directly to GPIO without current-limiting resistors**
⚠️ **Don't exceed 40mA per GPIO pin**
⚠️ **Use 3.3V for DHT11, not 5V (though some tolerate 5V)**
⚠️ **Double-check polarity before powering on**
⚠️ **Disconnect power before changing wiring**

## Photos Reference

When looking at your breadboard, it should look like this:

```
┌──────────────── ESP32 ────────────────┐
│  ┌────────────────────────────────┐   │
│  │                                │   │     DHT11
│  │         ESP32-WROOM            │   │      ╔═╗
│  │                                │   │      ╚═╝
│  └────────────────────────────────┘   │      │││
│   │  │  │  │  │  │  │  │  │  │  │    │      └┬┘
└───┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼────┘       │
    │  │  │ 15  │  4  2  │  │  │ 3.3V GND      │
    │  │  │  │  │  │  │  │  │  │  │  │         │
    │  │  └──┴──┼──│──│──┘  │  └──┴────────────┘
    │  │        │  │  │     │
    │  │       10k │  │     │
    │  └────────┴──┘  │     │
    │                 │     │
    │     LED1       LED2   │
    │      ○          ○     │
    │      │          │     │
    │     220Ω       220Ω   │
    │      │          │     │
    └──────┴──────────┴─────┘
            GND Rail
```

---

**Double-check all connections before uploading code!**
