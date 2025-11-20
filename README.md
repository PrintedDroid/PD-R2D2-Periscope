# R2-D2 Periscope LED Controller v2.1
**Advanced ESP32-C3 based LED controller for Star Wars R2-D2 Periscope builds**

## 🤖 Project Overview

This LED controller brings your R2-D2 Periscope to life with stunning light effects, multiple animation modes, and flexible control options. Designed for builders who demand professional results with authentic R2-D2 behavior.

### Key Features

- **✨ 6 Independent LED Groups** - Main (9), Top (7), Bottom (8), Left/Right Sides (9 each), Back (3)
- **🎨 16+ Animation Effects** - Per group with effects like pulse, chase, fire, rainbow, strobe, and more
- **🌈 10 Color Palette** - Red, Yellow, Green, Cyan, Blue, Magenta, Orange, Purple, White, Pink
- **⚡ Variable Speed Control** - 0-9 speed settings for all effects
- **📡 Dual Control Modes** - Serial commands OR Uppity Spinner hardware interface
- **🎬 20+ Pre-programmed Sequences** - R2-D2 startup, police lights, alarm, Knight Rider, fire, and more
- **🔄 Auto-Demo Mode** - Cycles through all sequences automatically
- **💡 Status LED** - Visual feedback with 1-second heartbeat
- **🎯 Effect Auto-Change** - Dynamic animation switching (Effect 99)

---

## ⚠️ CRITICAL - FastLED Library Version

**COMPATIBLE: FastLED 3.9.0 - 3.9.10 ONLY**

**WARNING:** Versions > 3.9.10 cause timing and display issues!

### Known Issues with FastLED > 3.9.10:
- ❌ Timing problems with multiple LED strips
- ❌ Random flickering and color issues
- ❌ RMT channel conflicts on ESP32-C3
- ❌ Unstable behavior with WS2812B LEDs
- ❌ Complete failure to light LEDs

### Installing Correct Version:

**Arduino IDE:**
1. Go to: Sketch → Include Library → Manage Libraries
2. Search for "FastLED"
3. Click "Select version" dropdown
4. Choose version **3.9.0** (recommended) or up to **3.9.10**
5. Click "Install"

**PlatformIO:**
Add to `platformio.ini`:
```ini
lib_deps =
    fastled/FastLED@^3.9.0
```

---

## 🔧 Hardware Requirements

### Core Components
- **Lolin C3 Mini (ESP32-C3)** - Primary microcontroller
  - ESP32-C3 RISC-V chip
  - USB-C interface
  - Compact form factor
- **6x WS2812B LED Strips** - Addressable RGB LEDs
  - Main: 9 LEDs
  - Top: 7 LEDs
  - Bottom: 8 LEDs
  - Left Side: 9 LEDs
  - Right Side: 9 LEDs
  - Back: 3 LEDs
  - **Total: 45 LEDs**
- **5V Power Supply** - Adequate for all LEDs (minimum 3A recommended)

### Optional Components
- **Uppity Spinner Interface** - Hardware control without serial connection

### Recommended Source
- **Printed-Droid.com Periscope Kit** - Complete periscope with LED integration

## 📋 Pin Configuration

ESP32-C3 (Lolin C3 Mini) Pin Assignments:

```
GPIO1  → Status LED (internal)
GPIO3  → Bottom LED Strip (8 LEDs)
GPIO4  → Left Side LED Strip (9 LEDs)
GPIO5  → Main LED Strip (9 LEDs)
GPIO6  → Top LED Strip (7 LEDs)
GPIO7  → Right Side LED Strip (9 LEDs)
GPIO8  → SDA (Uppity Spinner Pin B)
GPIO9  → SCL (Uppity Spinner Pin C)
GPIO10 → Back LED Strip (3 LEDs)
GPIO20 → RX (Uppity Spinner Pin A / Serial RX)
```

### Mode-Specific Pins

**Serial Mode (Default):**
- GPIO20: RX for serial commands

**Uppity Spinner Mode:**
- GPIO20 (RX): Uppity Pin A
- GPIO8 (SDA): Uppity Pin B
- GPIO9 (SCL): Uppity Pin C
- **Note:** Serial communication is DISABLED in Uppity Spinner mode

## 🔌 Power Requirements

- **Main Supply**: 5V/3A minimum
- **LEDs**: ~2700mA at full brightness (45 LEDs × 60mA)
- **ESP32-C3**: 3.3V internal regulation (~200mA)
- **Status LED**: Minimal current (<1mA)

### Power Calculation:
```
45 LEDs × 60mA (max per LED) = 2700mA
ESP32-C3 overhead: 200mA
Total maximum: ~3A at 5V
Typical operation (brightness 80): ~1.5-2A
```

**⚠️ Safety Warning**: Ensure adequate current capacity and proper fusing for safety.

## 🚀 Installation

### Method 1: Arduino IDE Setup

1. **Install ESP32 board support:**
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Tools → Board → Board Manager → Search "ESP32" → Install

2. **Board Configuration for Lolin C3 Mini:**
   - **Board**: "LOLIN C3 Mini"
   - **USB CDC On Boot**: "Enabled"
   - **CPU Frequency**: "160MHz (WiFi)"
   - **Flash Size**: "4MB (3MB APP/1MB SPIFFS)"
   - **Upload Speed**: "921600"

3. **Install Required Library:**
   - Sketch → Include Library → Manage Libraries
   - Search "FastLED"
   - Install version **3.9.0** (CRITICAL!)

### Method 2: PlatformIO (Recommended)

**Why PlatformIO?**
- ✅ Automatic library version management
- ✅ Professional IDE integration (VS Code)
- ✅ Faster builds and better error messages

**Setup Steps:**

1. **Install VS Code + PlatformIO Extension**
   - Download and install [Visual Studio Code](https://code.visualstudio.com/)
   - Open VS Code → Extensions (Ctrl+Shift+X)
   - Search "PlatformIO IDE" → Install
   - Restart VS Code

2. **Configure platformio.ini:**
   ```ini
   [env:lolin_c3_mini]
   platform = espressif32
   board = lolin_c3_mini
   framework = arduino
   lib_deps =
       fastled/FastLED@^3.9.0
   monitor_speed = 9600
   ```

3. **Compile & Upload:**
   ```bash
   pio run                    # Compile project
   pio run -t upload          # Upload to ESP32-C3
   pio device monitor         # Open serial monitor
   ```

## 🎮 Operating Modes

### Serial Mode (Default)

Control via USB serial connection at 9600 baud.

#### Command Format:
```
[Target][Effect][Color][Speed]
```

#### Examples:
```
M185    = Main LEDs, Effect 1 (pulse), White (8), Speed 5
T249    = Top LEDs, Effect 2 (run), Blue (4), Speed 9 (fastest)
A643    = All LEDs, Effect 6 (split), Blue (4), Speed 3
M12     = Main LEDs, Effect 12 (rainbow), auto color
Q4      = Sequence 4 (Police lights)
ON      = Enable all LEDs
OFF     = Disable all LEDs
?       = Show status and help
```

#### Targets:
- **M**: Main LEDs (9 LEDs)
- **T**: Top LEDs (7 LEDs)
- **B**: Bottom LEDs (8 LEDs)
- **S**: Both Side LEDs (18 LEDs)
- **L**: Left LEDs only (9 LEDs)
- **R**: Right LEDs only (9 LEDs)
- **K**: Back LEDs (3 LEDs)
- **A**: All LEDs (45 LEDs)
- **X**: All OFF
- **Q[0-20]**: Sequences (predefined light shows)

#### Effects (Per LED Group):

**Main LEDs (M) - 16 Effects:**
1. Pulse all LEDs
2. CW run 1 pixel
3. CW run 2 pixels
4. CW run 3 pixels
5. CW run 4 pixels
6. CW split 2
7. CW split 3
8. CW split 4
9. Strobe
10. Smooth pulse
11. Theater chase
12. Rainbow
13. Fire effect
14. Circle chase
15. Center expand
16. Spiral out

**Top/Bottom/Side LEDs (T/B/S/L/R) - 11 Effects:**
1. Pulse
2. Run
3. To center
4. Random sparkle
5. Strobe
6. Breathe
7. Wave
8. Alternate rows
9. Bounce
10. Fill from center
11. Knight Rider (Top only)

**Back LEDs (K) - 6 Effects:**
1. Simple on
2. Random colors
3. All on
4. Sparkle
5. Chase
6. Rainbow

**Special:**
- **0**: OFF
- **99**: Auto-change (cycles through all effects every 10 seconds)

#### Colors (0-9):
- **0**: Red
- **1**: Yellow
- **2**: Green
- **3**: Cyan
- **4**: Blue
- **5**: Magenta
- **6**: Orange
- **7**: Purple
- **8**: White
- **9**: Pink

#### Speed (0-9):
- **0**: Slowest
- **9**: Fastest

### Uppity Spinner Mode

Control via 3-pin hardware interface (8 states).

#### Enabling Uppity Spinner Mode:

Edit the sketch and uncomment:
```cpp
#define UPPITY_SPINNER_MODE
```

#### Pin Connections:
- **Pin A**: GPIO20 (RX)
- **Pin B**: GPIO8 (SDA)
- **Pin C**: GPIO9 (SCL)

#### Input States (3-bit = 8 states):

| State | A | B | C | Default Sequence |
|-------|---|---|---|------------------|
| 0 | 0 | 0 | 0 | All OFF |
| 1 | 0 | 0 | 1 | All OFF (always) |
| 2 | 0 | 1 | 0 | Sequence 4 (Police) |
| 3 | 0 | 1 | 1 | Sequence 1 (Party) |
| 4 | 1 | 0 | 0 | Sequence 13 (Fire) |
| 5 | 1 | 0 | 1 | Sequence 11 (Calm Blue) |
| 6 | 1 | 1 | 0 | Sequence 20 (Auto Demo) |
| 7 | 1 | 1 | 1 | Sequence 6 (Knight Rider) |

#### Customizing Sequences:

Edit these defines in the sketch:
```cpp
#define UPPITY_STATE_0_SEQUENCE -1  // -1 = off, 0-20 = sequence
#define UPPITY_STATE_2_SEQUENCE 4   // Police lights
#define UPPITY_STATE_3_SEQUENCE 1   // Party mode
#define UPPITY_STATE_4_SEQUENCE 13  // Fire mode
#define UPPITY_STATE_5_SEQUENCE 11  // Calm blue
#define UPPITY_STATE_6_SEQUENCE 20  // Auto demo
#define UPPITY_STATE_7_SEQUENCE 6   // Knight Rider
```

**Note:** Serial communication is DISABLED in Uppity Spinner mode (RX pin is used for input).

## 🎬 Pre-Programmed Sequences

### Q0: Original R2-D2 Startup
Classic R2-D2 power-up sequence with white pulsing and scanning.

### Q1: Party Mode
Rainbow effects with auto-cycling colors.

### Q2: Bright Pulse
Maximum brightness white pulse - searchlight mode.

### Q3: Communication Mode
Fast white strobes and chases simulating data transmission.

### Q4: Police Lights
Red/blue alternating strobes with chase effects.

### Q5: Alarm/Warning
Red strobes and bouncing effects for alert state.

### Q6: Knight Rider
Classic KITT scanner effect with red split animations.

### Q7: Searchlight Scanning
All LEDs white, maximum visibility mode.

### Q8: Stealth Search Mode
Slow red circle chase and scanning - low visibility.

### Q9: Dive
Blue spiral and breathe effects simulating underwater.

### Q10: Surface
Cyan expanding and wave effects.

### Q11: Calm Blue
Gentle blue breathing - idle/standby mode.

### Q12: Boot-up/System Check
Sequential green activation of all LED groups.

### Q13: Fire
Realistic fire effect with flickering orange/red.

### Q14: Celebration/Victory
Auto-cycling rainbow effects for success.

### Q15: Energy Charging
Blue waves and pulses simulating power-up.

### Q16: Hyperdrive/Warp
Fast white spirals and chases - high energy mode.

### Q17: Malfunction
Red/yellow alternating strobes indicating error state.

### Q18: Scan Complete
Green expanding effects confirming successful scan.

### Q19: Sonar Ping
Cyan center expanding pulses.

### Q20: Auto Demo Mode
Cycles through ALL sequences (0-19) every 15 seconds. Perfect for demonstrations!

## 🔧 Configuration

### Brightness Adjustment

Edit in sketch:
```cpp
#define BRIGHTNESS 80   // 0-255 (default: 80)
```

**Brightness Guide:**
- 0-50: Dim (indoor, low power)
- 80: Default (balanced)
- 150: Bright (outdoor)
- 255: Maximum (requires adequate power supply)

### Color Order

If colors appear wrong, adjust:
```cpp
#define COLOR_ORDER GRB  // Try RGB or BGR if colors incorrect
```

### Status LED Brightness

```cpp
#define STATUS_LED_BRIGHTNESS 64  // 0-255
```

## 🛠️ Troubleshooting

### LEDs Not Working

✅ **Check power supply:**
- Verify 5V supply with adequate current (3A minimum)
- Check all ground connections

✅ **Verify FastLED version:**
- Must be 3.9.0 - 3.9.10
- Newer versions have known issues with ESP32-C3

✅ **Check pin connections:**
```
Main:   GPIO5
Top:    GPIO6
Bottom: GPIO3
Left:   GPIO4
Right:  GPIO7
Back:   GPIO10
```

✅ **Test with simple command:**
```
M388    (Main all white, fast)
A388    (All LEDs white, fast)
```

### Random Flickering or Wrong Colors

❌ **Most likely cause: FastLED version > 3.9.10**

**Solution:**
1. Uninstall current FastLED library
2. Install FastLED 3.9.0 specifically
3. Re-upload sketch

✅ **If using correct version, try:**
- Change COLOR_ORDER (GRB → RGB → BGR)
- Reduce BRIGHTNESS value
- Check power supply stability
- Add capacitors (1000µF) across power lines

### Serial Communication Not Working

✅ **Check baud rate:** Must be 9600
✅ **Enable USB CDC On Boot** in Arduino IDE
✅ **Try different USB cable** (some are power-only)
✅ **Press Reset button** on Lolin C3 Mini

### Uppity Spinner Mode Not Responding

✅ **Verify mode is enabled:**
```cpp
#define UPPITY_SPINNER_MODE  // Uncommented
```

✅ **Check pin connections:**
- A = GPIO20 (RX)
- B = GPIO8 (SDA)
- C = GPIO9 (SCL)

✅ **Use INPUT_PULLUP:** Pins use internal pull-ups, connect to GND for logic 1

✅ **Visual confirmation:** White flash on startup indicates Uppity mode active

### Status LED Not Blinking

✅ **Normal operation:** 1 second on, 1 second off
✅ **If solid or off:** Check GPIO1 connection
✅ **If too bright:** Adjust STATUS_LED_BRIGHTNESS

## 💡 Status LED Indicators

The status LED (GPIO1) provides system feedback:

**Normal Operation:**
- **1 Hz blink** (1 sec on, 1 sec off): System running normally

**Boot Sequence:**
- **Solid ON** briefly: System initializing
- **Switches to 1 Hz blink**: Initialization complete

**Note:** In Uppity Spinner mode, a white flash on all LEDs indicates mode activation.

## 🎨 Effect Showcase

### Fire Effect (Effect 13)
Realistic fire simulation using heat diffusion algorithm. Best with orange/red colors.
```
M13     (Main fire with auto colors)
Q13     (Full fire sequence)
```

### Rainbow Effect (Effect 12)
Smooth color cycling through full spectrum. Speed controls rotation rate.
```
M129    (Main rainbow, fastest)
A125    (All LEDs rainbow, medium)
```

### Knight Rider (Top Effect 11)
Classic KITT scanner with single pixel bounce. Top LEDs only.
```
T1109   (Top Knight Rider, red, fastest)
Q6      (Full Knight Rider sequence)
```

### Strobe Effect (Effect 9 Main, 5 Others)
High-speed on/off flashing. Speed controls flash rate.
```
M989    (Main strobe white, fastest)
Q3      (Communication mode - multiple strobes)
```

### Smooth Pulse (Effect 10)
Gentle breathing effect using beatsin8. Adjustable speed.
```
M1085   (Main smooth pulse white, medium)
Q11     (Calm blue - smooth pulse all LEDs)
```

## 📊 Performance Specifications

| Specification | Value |
|---------------|-------|
| Total LEDs | 45 |
| Refresh Rate | ~60 FPS |
| Effect Count | 40+ (across all groups) |
| Sequences | 21 (0-20) |
| Colors | 10 predefined |
| Speed Levels | 10 (0-9) |
| Control Modes | 2 (Serial + Uppity) |
| Power Consumption | 1.5-2A typical @ 5V |
| Baud Rate | 9600 (Serial mode) |
| Boot Time | <2 seconds |

## 🔐 Safety Considerations

### Electrical Safety
- **Power Supply**: Use regulated 5V supply with adequate current rating (3A minimum)
- **Fusing**: Install appropriate fuse (3-5A) for LED circuits
- **Wiring**: Use adequate wire gauge (22-24 AWG minimum for LED strips)
- **Heat Management**: Ensure proper ventilation, monitor ESP32-C3 temperature

### Operational Safety
- **Brightness**: Avoid prolonged operation at maximum brightness (thermal concerns)
- **Sequences**: Some strobe effects may trigger photosensitivity - use caution
- **Power-On**: Always connect power before USB to avoid back-powering

## 🚀 Advanced Customization

### Creating Custom Sequences

Edit `processSequence()` function in the sketch:

```cpp
case 21: // Your custom sequence
  processCommand("M689");   // Main chase white fast
  processCommand("T189");   // Top pulse white medium
  processCommand("S549");   // Sides strobe blue fast
  processCommand("B689");   // Bottom comet white fast
  processCommand("K389");   // Back all on white fast
  break;
```

### Adjusting Effect Timing

Modify speed ranges in `processCommand()`:

```cpp
// Current: map(speed, 0, 9, 200, 20) for Main LEDs
// Slower: map(speed, 0, 9, 400, 40)
// Faster: map(speed, 0, 9, 100, 10)
```

### Adding New Colors

Extend color palette in `BaseLeds.h`:

```cpp
const CRGB colorMap[] = {
  // Existing colors (0-9)
  // Add new colors:
  CRGB(255, 128, 0),  // 10: Amber
  CRGB(0, 255, 128),  // 11: Spring Green
  // Update command parser to support 10+
};
```

### Custom Effects

Add new effects to LED classes (MainLeds.cpp, TopLeds.cpp, etc.):

```cpp
void MainLeds::yourCustomEffect() {
  // Your effect code here
  // Access: this->leds, this->numleds, this->currentColor
}
```

Then add case to `update()` switch statement.

## 📦 Project File Structure

```
PD-Periscope/
├── R2D2_Periscope_C3_v2.1_Enhanced.ino  # Main sketch
├── BaseLeds.h                            # Base LED class + color palette
├── MainLeds.h / .cpp                     # Main LED ring (9 LEDs)
├── TopLeds.h / .cpp                      # Top LEDs (7 LEDs)
├── BottomLeds.h / .cpp                   # Bottom LEDs (8 LEDs)
├── SideLeds.h / .cpp                     # Left/Right LEDs (9 each)
├── BackLeds.h / .cpp                     # Back LEDs (3 LEDs)
└── README.md                             # This file
```

## 📝 Version History

### Version 2.1 (2025/06)
- Enhanced command format with variable-length effect numbers
- Improved command parser for robust multi-digit effect handling
- Added comprehensive sequence library (21 sequences)
- Uppity Spinner mode integration
- Auto-demo mode (Sequence 20)
- Status LED with 1Hz heartbeat
- Fire effect optimization
- Better documentation

### Version 2.0
- Multi-LED group support (6 independent strips)
- Serial command interface
- Pre-programmed sequences
- Effect auto-change mode

### Version 1.0
- Initial release
- Basic LED control

## 📞 Support

### Getting Help

1. **Check this README** for common solutions
2. **Verify FastLED version** (must be 3.9.0 - 3.9.10)
3. **Test with simple commands** (A388 = all white)
4. **Check power connections** (5V + adequate current)

### Common Issues Quick Reference

| Problem | Quick Fix |
|---------|-----------|
| No LEDs lighting | Check FastLED version (must be 3.9.0 - 3.9.10) |
| Random flickering | Downgrade FastLED to 3.9.0 |
| Wrong colors | Try different COLOR_ORDER (GRB/RGB/BGR) |
| Serial not working | Check baud rate (9600), enable USB CDC |
| Uppity mode not working | Verify #define uncommented, check pins |
| Dim LEDs | Increase BRIGHTNESS value |
| Status LED not blinking | Check GPIO1 connection |

### Diagnostic Commands (Serial Mode)

```
?       # Show status and command reference
ON      # Enable all LEDs
OFF     # Disable all LEDs
A388    # Test all LEDs (white, fast)
Q0      # Test sequence 0 (R2-D2 startup)
Q20     # Test auto-demo mode
```

## 📜 License & Credits

### Project Credits
- **Hardware Design**: Printed-Droid.com
- **Software Development**: Printed-Droid.com
- **Version**: 2.1
- **Date**: 2025/06

### Open Source Libraries
- **FastLED**: High-performance LED control library

### Disclaimer

⚠️ **IMPORTANT SAFETY NOTICE** ⚠️

This project involves electrical components and LED strips. Users are responsible for:

- Proper electrical safety and insulation
- Adequate power supply sizing and protection
- Safe wiring and assembly practices
- Compliance with local electrical codes
- Testing all functions before final installation

**BUILD AT YOUR OWN RISK.** Ensure proper knowledge of electronics and safety practices. The authors assume no responsibility for damage, injury, or malfunction resulting from use of this design.

---

**May the Force be with your build!** 🌟

*For the latest updates and community support, visit: www.printed-droid.com*
