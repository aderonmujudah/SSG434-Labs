# Quick Start Guide

Get your ESP32 Bluetooth LED & Sensor Controller up and running in minutes!

## What You Need

### Hardware

- 1× ESP32 Development Board
- 2× LEDs (any color)
- 2× 220Ω Resistors
- 1× DHT11 Sensor
- 1× 10kΩ Resistor
- Breadboard & Jumper Wires
- USB Cable

### Software

- Arduino IDE with ESP32 Support
- Adafruit DHT Library
- Android Phone with Bluetooth
- Serial Bluetooth Terminal App

## 5-Minute Setup

### Step 1: Install Arduino Libraries (2 minutes)

1. Open Arduino IDE
2. Go to: **Tools → Manage Libraries**
3. Search and install:
   - `DHT sensor library` by Adafruit
   - `Adafruit Unified Sensor` (dependency)

### Step 2: Wire Your Circuit (2 minutes)

```
ESP32 Pin    →    Component
────────────────────────────
GPIO 2       →    LED1 (via 220Ω) → GND
GPIO 4       →    LED2 (via 220Ω) → GND
GPIO 15      →    DHT11 Data (+ 10kΩ to 3.3V)
3.3V         →    DHT11 VCC
GND          →    DHT11 GND
```

**LED Wiring:** Long leg (+) → Resistor → GPIO Pin | Short leg (-) → GND

See [WIRING_GUIDE.md](WIRING_GUIDE.md) for detailed diagrams.

### Step 3: Upload Code (1 minute)

1. Open `esp32_bluetooth_control.ino` in Arduino IDE
2. Select: **Tools → Board → ESP32 Dev Module**
3. Select your COM port: **Tools → Port → COM# (ESP32)**
4. Click **Upload** ⬆️
5. Wait for "Done uploading"

### Step 4: Connect from Phone (30 seconds)

**Android:**

1. Settings → Bluetooth → Turn ON
2. Scan and pair with "ESP32_BT_Controller"
3. Install "Serial Bluetooth Terminal" from Play Store
4. Open app → Menu → Devices → Select ESP32 → Connect

**Done!** 🎉

## Testing Your Setup

### Quick Test Commands

Type these in the Bluetooth terminal:

```
LED1:ON       ← LED 1 should light up
LED1:OFF      ← LED 1 should turn off
LED2:ON       ← LED 2 should light up
GET:SENSOR    ← Get temperature & humidity
AUTO:ON       ← Auto-send sensor data every 5 seconds
STATUS        ← See all status info
```

### Expected Responses

```
> LED1:ON
< OK: LED1 turned ON

> GET:SENSOR
< SENSOR:{"temp":25.5,"humidity":60.0,"unit":"C"}

> AUTO:ON
< OK: Auto-send enabled (5s interval)
< SENSOR:{"temp":25.5,"humidity":60.0,"unit":"C"}
< SENSOR:{"temp":25.5,"humidity":60.0,"unit":"C"}
... (continues)
```

## All Available Commands

| Command      | Action                     |
| ------------ | -------------------------- |
| `LED1:ON`    | Turn LED 1 ON              |
| `LED1:OFF`   | Turn LED 1 OFF             |
| `LED2:ON`    | Turn LED 2 ON              |
| `LED2:OFF`   | Turn LED 2 OFF             |
| `GET:SENSOR` | Get temperature & humidity |
| `AUTO:ON`    | Enable auto-send (5 sec)   |
| `AUTO:OFF`   | Disable auto-send          |
| `STATUS`     | Get full status report     |

## Troubleshooting

### Problem: LEDs don't work

**Fix:**

- Check LED polarity (long leg = +)
- Verify resistors are connected
- Test pins with Serial Monitor

### Problem: Sensor shows error

**Fix:**

- Check DHT11 wiring
- Ensure 10kΩ pull-up resistor on data pin
- Wait 2 seconds after power-on
- Replace DHT11 if defective

### Problem: Can't connect via Bluetooth

**Fix:**

- Check "ESP32_BT_Controller" appears in Bluetooth settings
- Pair device first in Android settings
- Restart ESP32
- Check Serial Monitor shows "Bluetooth device started"

### Problem: Upload fails

**Fix:**

- Hold BOOT button during upload
- Try different USB cable
- Lower upload speed: Tools → Upload Speed → 115200
- Check correct COM port selected

## Debug with Serial Monitor

1. Open Serial Monitor: **Tools → Serial Monitor**
2. Set baud rate to **115200**
3. You should see:
   ```
   Bluetooth device started, you can pair it with your phone!
   Device Name: ESP32_BT_Controller
   ```
4. Commands sent via Bluetooth appear here too

## Next Steps

### Customize Your Project

- **Change LED pins:** Edit `LED1_PIN` and `LED2_PIN` in code
- **Change Bluetooth name:** Edit `SerialBT.begin("YOUR_NAME")`
- **Change auto-send interval:** Edit `AUTO_SEND_INTERVAL` (milliseconds)
- **Add more LEDs:** Copy LED1 code pattern
- **Use DHT22:** Change `DHT_TYPE` to `DHT22`

### Build Your Own App

See [MOBILE_APP_GUIDE.md](MOBILE_APP_GUIDE.md) for:

- Complete Android app source code
- Flutter example
- Custom UI designs

### Advanced Features

- Add PWM for LED brightness control
- Add more sensors (ultrasonic, LDR, etc.)
- Implement data logging
- Create data visualization graphs
- Add password protection

## Project Structure

```
SSG434-12/
├── esp32_bluetooth_control.ino  ← Main ESP32 code (upload this)
├── README.md                    ← Full documentation
├── QUICK_START.md              ← This file
├── WIRING_GUIDE.md             ← Detailed wiring diagrams
└── MOBILE_APP_GUIDE.md         ← Android app development
```

## Pin Reference Card

Print this for quick reference:

```
┌─────────────────────────────────┐
│   ESP32 Bluetooth Controller    │
├─────────────────────────────────┤
│ GPIO 2   → LED 1 (220Ω → GND)  │
│ GPIO 4   → LED 2 (220Ω → GND)  │
│ GPIO 15  → DHT11 Data           │
│ 3.3V     → DHT11 VCC            │
│ GND      → DHT11 GND            │
├─────────────────────────────────┤
│ BT Name: ESP32_BT_Controller    │
│ Baud: 115200                    │
└─────────────────────────────────┘
```

## Support

- **Full Documentation:** [README.md](README.md)
- **Wiring Help:** [WIRING_GUIDE.md](WIRING_GUIDE.md)
- **App Development:** [MOBILE_APP_GUIDE.md](MOBILE_APP_GUIDE.md)

## Success Checklist

- [ ] Libraries installed ✓
- [ ] Circuit wired correctly ✓
- [ ] Code uploaded successfully ✓
- [ ] ESP32 paired with phone ✓
- [ ] Bluetooth app connected ✓
- [ ] LEDs respond to commands ✓
- [ ] Sensor data received ✓

**All checked? Congratulations! Your project is working!** 🎉🎊

---

**Need Help?** Check the troubleshooting section or review the detailed README.md

**Ready to Customize?** See the customization section above to make it your own!
