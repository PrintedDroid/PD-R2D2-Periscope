# R2-D2 Periscope LED Controller v2.2
**Advanced ESP32-C3 based LED controller for Star Wars R2-D2 Periscope builds**

## 🤖 Project Overview

This LED controller brings your R2-D2 Periscope to life with stunning light effects, multiple animation modes, and flexible control options. Designed for builders who demand professional results with authentic R2-D2 behavior.

### Key Features

- **✨ 6 Independent LED Groups** - Main (9), Top (7), Bottom (12), Left/Right Sides (9 each), Back (3)
- **🎨 20+ Animation Effects** - Per group with effects like pulse, chase, fire, rainbow, strobe, and more
- **🌈 20 Customizable Colors** - 10 default colors + 10 user-definable custom colors (RGB & HSV)
- **⚡ Variable Speed Control** - 0-9 speed settings for all effects
- **📡 Dual Control Modes** - Serial commands OR Uppity Spinner hardware interface
- **🎬 32 Pre-programmed Sequences** - R2-D2 startup, police lights, alarm, Knight Rider, fire, thematic sequences, and more
- **⏱️ Timed Sequences** - Run any sequence for a specified time (e.g., Q15T20 = 20 seconds)
- **🔁 Q31 Continuous Loop** - White flash alert that loops forever until stopped
- **🎯 10 Custom Sequences** - Create and save your own light shows with the CLI
- **💾 Flash Storage** - All settings, colors, and sequences persist across reboots
- **🔧 CLI Configuration System** - Runtime configuration without recompiling
- **🔄 Auto-Demo Mode** - Cycles through all sequences automatically
- **💡 Status LED** - Visual feedback with 1-second heartbeat
- **🎯 Effect Auto-Change** - Dynamic animation switching (Effect 99)
- **🔌 I2C Slave Mode** - Remote control capability (address 0x20)

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
  - Bottom: 12 LEDs (6 pairs - LEDs controlled in pairs for uniform lighting)
  - Left Side: 9 LEDs
  - Right Side: 9 LEDs
  - Back: 3 LEDs
  - **Total: 49 LEDs**
- **5V Power Supply** - Adequate for all LEDs (minimum 3A recommended)

### Optional Components
- **Uppity Spinner Interface** - Hardware control without serial connection

### Recommended Source
- **Printed-Droid.com Periscope Kit** - Complete periscope with LED integration

## 📋 Pin Configuration

ESP32-C3 (Lolin C3 Mini) Pin Assignments:

```
GPIO1  → Status LED (internal)
GPIO3  → Bottom LED Strip (12 LEDs)
GPIO4  → Left Side LED Strip (9 LEDs)
GPIO5  → Main LED Strip (9 LEDs)
GPIO6  → Top LED Strip (7 LEDs)
GPIO7  → Right Side LED Strip (9 LEDs)
GPIO8  → SDA (I2C / Uppity Spinner Pin B)
GPIO9  → SCL (I2C / Uppity Spinner Pin C)
GPIO10 → Back LED Strip (3 LEDs)
GPIO20 → RX (Uppity Spinner Pin A / Serial RX)
```

### Mode-Specific Pins

**Serial Mode (Default):**
- GPIO20: RX for serial commands
- GPIO8/9: I2C (SDA/SCL) for remote control at address 0x20

**Uppity Spinner Mode:**
- GPIO20 (RX): Uppity Pin A
- GPIO8 (SDA): Uppity Pin B
- GPIO9 (SCL): Uppity Pin C
- **Note:** Serial communication is DISABLED in Uppity Spinner mode

## 🔌 Power Requirements

- **Main Supply**: 5V/3A minimum
- **LEDs**: ~2940mA at full brightness (49 LEDs × 60mA)
- **ESP32-C3**: 3.3V internal regulation (~200mA)
- **Status LED**: Minimal current (<1mA)

### Power Calculation:
```
49 LEDs × 60mA (max per LED) = 2940mA
ESP32-C3 overhead: 200mA
Total maximum: ~3.2A at 5V
Typical operation (brightness 80): ~1.6-2.2A
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

#### Basic Command Examples:
```
M185    = Main LEDs, Effect 1 (pulse), White (8), Speed 5
T249    = Top LEDs, Effect 2 (run), Blue (4), Speed 9 (fastest)
A643    = All LEDs, Effect 6 (split), Blue (4), Speed 3
M12     = Main LEDs, Effect 12 (rainbow), auto color
Q4      = Sequence 4 (Police lights)
Q31     = Sequence 31 (White flash - loops forever)
Q15T20  = Sequence 15 for 20 seconds, then auto-OFF
Q31T10  = Q31 loops for 10 seconds, then stops
S1      = Custom sequence 1 (user-defined)
ON      = Enable all LEDs
OFF     = Disable all LEDs
?       = Show status and help
HELP    = Show comprehensive command reference
```

#### Targets:
- **M**: Main LEDs (9 LEDs)
- **T**: Top LEDs (7 LEDs)
- **B**: Bottom LEDs (12 LEDs)
- **S**: Both Side LEDs (18 LEDs)
- **L**: Left LEDs only (9 LEDs)
- **R**: Right LEDs only (9 LEDs)
- **K**: Back LEDs (3 LEDs)
- **A**: All LEDs (49 LEDs)
- **X**: All OFF
- **Q[0-31]**: Built-in sequences (predefined light shows)
- **Q[0-31]T[seconds]**: Timed sequences (runs for specified seconds, then auto-OFF)
- **S[1-10]**: Custom sequences (user-created sequences)

#### Effects (Per LED Group):

**Main LEDs (M) - 20 Effects:**
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
11. Twinkle
12. Theater chase
13. Rainbow
14. Fire effect
15. Circle chase
16. Center expand
17. Twinkle (variation)
18. Theater chase (variation)
19. Bounce trail
20. Color gradient

**Top LEDs (T) - 15 Effects:**
1. Pulse
2. Left run
3. To center
4. Random sparkle
5. Strobe
6. Breathe
7. Wave
8. Alternate rows
9. Bounce
10. Fill from center
11. Knight Rider
12. Twinkle
13. Theater chase
14. Bounce trail
15. Color gradient

**Bottom LEDs (B) - 13 Effects:**
1. Pulse
2. Simple scan (1 pair)
3. Scan (2 pairs)
4. Random
5. Strobe
6. Comet (with tail)
7. Wave (phase offset)
8. Alternate rows (3+3 pairs)
9. Snake (3 pairs)
10. Superscan (expand/contract)
11. Twinkle
12. Theater chase
13. Bounce trail

**Side LEDs (S/L/R) - 13 Effects:**
1. Pulse
2. Run
3. Random sparkle
4. Strobe
5. Breathe
6. Fire
7. Rainbow
8. Sparkle
9. Twinkle
10. Smooth pulse
11. Theater chase
12. Bounce trail
13. Color gradient

**Back LEDs (K) - 12 Effects:**
1. Simple on
2. Random colors
3. All on
4. Sparkle
5. Chase
6. Breathe
7. Twinkle
8. Smooth pulse
9. Theater chase
10. Bounce trail
11. Color gradient
12. Rainbow

**Special:**
- **0**: OFF
- **99**: Auto-change (cycles through all effects every 10 seconds)

#### Default Colors (0-9):
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

#### Custom Colors (10-19):
- **10-19**: User-definable colors via CLI (see Custom Color Management section)

#### Speed (0-9):
- **0**: Slowest
- **9**: Fastest

---

## ⏱️ Timed Sequences (QxxTyy)

**NEW in v2.2!** Run any sequence for a specified time, then automatically turn off.

### Syntax:
```
Q<sequence>T<seconds>
```

### How It Works:
- Runs the specified sequence for the given number of seconds
- Automatically turns the system **OFF** when time expires
- Works with **ALL sequences** (Q0-Q31)
- **Q31 special behavior**: Loops continuously during the time window
- Can be stopped early by sending any other command

### Examples:

**Basic timed sequences:**
```
Q15T20    Run sequence 15 for 20 seconds, then OFF
Q4T30     Police lights for 30 seconds
Q11T60    Calm blue for 60 seconds (1 minute)
Q0T10     R2-D2 startup for 10 seconds
```

**Q31 with timer:**
```
Q31T10    White flash loops for 10 seconds, then stops
Q31T5     Quick 5-second alert flash
Q31       White flash loops FOREVER (no timer)
```

### Stopping Timed Sequences:

Any command will stop the timer and current sequence:
```
OFF       Stop sequence and turn off
Q5        Stop current and run Q5 instead
M185      Stop sequence and run effect
X         All LEDs off
```

### Use Cases:

**Attention Signal:**
```
Q31T10    Alert flash for 10 seconds
```

**Timed Party Mode:**
```
Q1T300    Party mode for 5 minutes (300 seconds)
```

**Quick Test:**
```
Q4T5      Test police lights for 5 seconds
```

**Sleep Timer:**
```
Q28T1800  Sleep mode for 30 minutes, then auto-off
```

---

## 🎨 Custom Color Management

**NEW in v2.2!** Define and save your own custom colors that persist across reboots.

### Color Slots:
- **Slots 0-9**: Default colors (can be edited)
- **Slots 10-19**: Custom colors (fully user-definable)
- **Total**: 20 color slots available

### Color Commands:

#### List All Colors:
```
COLOR LIST
COLORS        (shortcut)
```

Shows all 20 colors with RGB values and status (Default/Custom).

Example output:
```
=== Color Palette (20 slots) ===
Slot | Name          | RGB Values      | Status
-----|---------------|-----------------|--------
  0  | Red           | 255,   0,   0  | Default
  1  | Yellow        | 255, 255,   0  | Default
  ...
 10  | My Blue       |   0,  50, 200  | Custom
 11  | Warm Orange   | 255, 140,   0  | Custom
```

#### Define Color by RGB:
```
COLOR RGB <slot> <r> <g> <b> [name]
```

**Parameters:**
- `slot`: 0-19 (color slot number)
- `r`: 0-255 (red component)
- `g`: 0-255 (green component)
- `b`: 0-255 (blue component)
- `name`: Optional color name (max 19 characters)

**Examples:**
```
COLOR RGB 10 0 50 200 Deep Blue
COLOR RGB 11 255 140 0 Warm Orange
COLOR RGB 0 200 0 0 Dark Red
```

#### Define Color by HSV:
```
COLOR HSV <slot> <h> <s> <v> [name]
```

**Parameters:**
- `slot`: 0-19 (color slot number)
- `h`: 0-255 (hue - color wheel position)
- `s`: 0-255 (saturation - color intensity, 0=white, 255=pure)
- `v`: 0-255 (value - brightness, 0=black, 255=bright)
- `name`: Optional color name

**HSV Guide:**
- **Hue (H)**: 0=Red, 43=Yellow, 85=Green, 128=Cyan, 170=Blue, 213=Magenta
- **Saturation (S)**: 0=Grayscale/White, 255=Fully saturated color
- **Value (V)**: 0=Black, 128=Medium brightness, 255=Full brightness

**Examples:**
```
COLOR HSV 12 170 255 200 Sky Blue
COLOR HSV 13 85 200 255 Lime Green
COLOR HSV 14 0 0 128 Medium Gray
```

#### Reset Colors:
```
COLOR RESET <slot>     Reset one color to default
COLOR RESET ALL        Reset all colors to factory defaults
```

**Examples:**
```
COLOR RESET 10         (reset slot 10 to default)
COLOR RESET ALL        (reset all 20 colors)
```

#### Save Colors to Flash:
```
SAVE
```

Saves all custom colors to flash memory. They will persist across power cycles.

### Using Custom Colors:

Once defined, use custom colors like any other color in commands:

```
M1105      Main pulse, color slot 10, speed 5
T3129      Top to-center, color slot 12, speed 9
S6147      Sides breathe, color slot 14, speed 7
```

### Custom Color Workflow Example:

```
# 1. List current colors
COLOR LIST

# 2. Define a custom deep blue
COLOR RGB 10 0 50 200 Deep Blue

# 3. Define a warm orange
COLOR HSV 11 20 255 255 Warm Orange

# 4. Test the colors
M1105      (Main pulse with Deep Blue)
T1119      (Top pulse with Warm Orange)

# 5. Save to flash
SAVE

# 6. Verify colors are saved
COLOR LIST
```

---

## 🎬 Custom Sequence Creation

**NEW in v2.2!** Create and save your own light show sequences with the CLI.

### What are Custom Sequences?

Custom sequences (S1-S10) allow you to chain multiple LED commands together with delays to create complex light shows. They persist across reboots and can be triggered with a single command.

### Custom Sequence Workflow:

#### 1. Create a New Sequence:
```
SEQ NEW S<n> <name>
```

**Parameters:**
- `n`: 1-10 (sequence slot)
- `name`: Sequence name (max 30 characters)

**Example:**
```
SEQ NEW S1 Police Chase
```

#### 2. Add Commands to Sequence:
```
SEQ ADD S<n> <command> [DELAY <ms>]
```

**Parameters:**
- `n`: 1-10 (sequence slot)
- `command`: Any valid LED command
- `ms`: Optional delay in milliseconds after this command

**Examples:**
```
SEQ ADD S1 L509 DELAY 500        Left strobe red, wait 500ms
SEQ ADD S1 R549 DELAY 500        Right strobe blue, wait 500ms
SEQ ADD S1 M989 DELAY 200        Main strobe white, wait 200ms
SEQ ADD S1 T249                  Top chase blue (no delay)
```

#### 3. Save Sequence to Flash:
```
SEQ SAVE S<n>
```

**Example:**
```
SEQ SAVE S1
```

#### 4. Run Your Sequence:
```
S<n>
```

**Example:**
```
S1          (runs your Police Chase sequence)
```

#### 5. List All Custom Sequences:
```
SEQ LIST
```

Shows all saved sequences with their names and command counts.

Example output:
```
=== Custom Sequences ===
S1: Police Chase (4 commands)
S2: Rainbow Party (6 commands)
S3: Calm Mode (3 commands)
---
S4-S10: Empty
```

#### 6. Delete a Sequence:
```
SEQ DEL S<n>
```

**Example:**
```
SEQ DEL S1      (deletes Police Chase sequence)
```

### Complete Custom Sequence Example:

**Creating a "Fire Alarm" sequence:**

```
# 1. Create the sequence
SEQ NEW S2 Fire Alarm

# 2. Add commands with timing
SEQ ADD S2 X DELAY 200                # All off
SEQ ADD S2 M905 DELAY 300             # Main strobe red
SEQ ADD S2 T905 DELAY 300             # Top strobe red
SEQ ADD S2 S505 DELAY 300             # Sides strobe red
SEQ ADD S2 B805 DELAY 300             # Bottom alternate red
SEQ ADD S2 K105 DELAY 500             # Back random red
SEQ ADD S2 A905                       # All strobe red (final)

# 3. Save to flash
SEQ SAVE S2

# 4. Test it
S2

# 5. Verify it's saved
SEQ LIST
```

**Creating a "Startup" sequence:**

```
SEQ NEW S3 Boot Sequence
SEQ ADD S3 X DELAY 500                # All off, pause
SEQ ADD S3 M308 DELAY 300             # Main center white
SEQ ADD S3 T185 DELAY 200             # Top run white
SEQ ADD S3 S285 DELAY 200             # Sides run white
SEQ ADD S3 B205 DELAY 500             # Bottom scan red
SEQ ADD S3 K285                       # Back all on red
SEQ SAVE S3
```

**Creating a "Rainbow Party" sequence:**

```
SEQ NEW S4 Rainbow Party
SEQ ADD S4 M12 DELAY 100              # Main rainbow
SEQ ADD S4 T12 DELAY 100              # Top rainbow
SEQ ADD S4 S7 DELAY 100               # Sides rainbow
SEQ ADD S4 B12 DELAY 100              # Bottom rainbow
SEQ ADD S4 K12                        # Back rainbow
SEQ SAVE S4
```

### Tips for Creating Great Sequences:

1. **Start Simple**: Begin with 3-4 commands and expand from there
2. **Use Delays Strategically**:
   - Short delays (100-300ms): Quick transitions
   - Medium delays (500-1000ms): Visible pauses
   - Long delays (1500-3000ms): Dramatic timing
3. **Start with X (All OFF)**: Gives a clean slate for your sequence
4. **Test Incrementally**: Add commands one at a time and test
5. **Combine Effects**: Mix different effects for visual interest
6. **Use Custom Colors**: Define custom colors for unique looks
7. **Maximum Capacity**: Each sequence can hold up to 20 commands

### Sequence Slots:

- **Q0-Q31**: Built-in sequences (cannot be edited)
- **S1-S10**: Custom sequences (user-editable)

This separation ensures your custom sequences never overwrite the built-in ones!

---

## 🎬 Pre-Programmed Sequences (Q0-Q31)

### Basic Sequences (Q0-Q20)

#### Q0: Original R2-D2 Startup
Classic R2-D2 power-up sequence with white pulsing and scanning.

#### Q1: Party Mode
Rainbow effects with auto-cycling colors.

#### Q2: Bright Pulse
Maximum brightness white pulse - searchlight mode.

#### Q3: Communication Mode
Fast white strobes and chases simulating data transmission.

#### Q4: Police Lights
Red/blue alternating strobes with chase effects.

#### Q5: Alarm/Warning
Red strobes and bouncing effects for alert state.

#### Q6: Knight Rider
Classic KITT scanner effect with red split animations.

#### Q7: Searchlight Scanning
All LEDs white, maximum visibility mode.

#### Q8: Stealth Search Mode
Slow red circle chase and scanning - low visibility.

#### Q9: Dive
Blue spiral and breathe effects simulating underwater.

#### Q10: Surface
Cyan expanding and wave effects.

#### Q11: Calm Blue
Gentle blue breathing - idle/standby mode.

#### Q12: Boot-up/System Check
Sequential green activation of all LED groups.

#### Q13: Fire
Realistic fire effect with flickering orange/red.

#### Q14: Celebration/Victory
Auto-cycling rainbow effects for success.

#### Q15: Energy Charging
Blue waves and pulses simulating power-up.

#### Q16: Hyperdrive/Warp
Fast white spirals and chases - high energy mode.

#### Q17: Malfunction
Red/yellow alternating strobes indicating error state.

#### Q18: Scan Complete
Green expanding effects confirming successful scan.

#### Q19: Sonar Ping
Cyan center expanding pulses.

#### Q20: Auto Demo Mode
Cycles through ALL sequences (0-19) every 15 seconds. Perfect for demonstrations!

### Thematic Sequences (Q21-Q31)

#### Q21: Happy
Bright, energetic white twinkles and theater chase effects. R2 is excited!

#### Q22: Angry
Aggressive red strobing across all LED groups. R2 is mad!

#### Q23: Scared
Nervous, quick, erratic white sparkles and twinkles. R2 is frightened!

#### Q24: Boot Sequence
Realistic system startup - sequential green activation with delays.

#### Q25: Shutdown
Gradual power down - LEDs turn off one group at a time from back to front.

#### Q26: Radar Scan
Rotating cyan scan effect with circle chase pattern.

#### Q27: Celebration
Enhanced party mode with bounce trails and rainbow effects.

#### Q28: Sleep Mode
Gentle red breathing - low-power standby mode.

#### Q29: Color Gradient Demo
Showcases the new color gradient effect across all LED groups.

#### Q30: Theater Mode
All theater chase effects synchronized in white.

#### Q31: White Double-Flash (Continuous Loop) **NEW!**
**Special behavior: Loops continuously until stopped!**

Precise white strobe sequence that repeats forever:
- Sides double-flash (50% brightness)
- Pause 300ms
- Top & Bottom double-flash (50% brightness)
- Pause 300ms
- Main double-flash (100% brightness)
- Pause 300ms
- **Repeat continuously**

**Stopping Q31:**
- Send any other command (OFF, Q0, M185, X, etc.)
- Use timed version: `Q31T10` (runs for 10 seconds, then stops)

Perfect for:
- Continuous alert signals
- Attention-getting mode
- Communication signals
- Emergency indicators

**Examples:**
```
Q31       Runs forever (until stopped)
Q31T10    Runs for 10 seconds, then auto-stops
Q31T5     Quick 5-second alert
OFF       Stops Q31 immediately
```

---

## 🔧 Configuration System

**NEW in v2.2!** Comprehensive CLI-based configuration with flash storage.

### Configuration Commands:

#### Show Current Configuration:
```
CONFIG
```

Displays all current settings:
- Brightness level
- Bottom LED count (8 or 12)
- Auto-start enabled/disabled
- Startup sequence
- Custom sequences
- Custom colors

#### Set Brightness:
```
SET BRIGHTNESS <0-255>
```

**Examples:**
```
SET BRIGHTNESS 80      (default)
SET BRIGHTNESS 150     (bright)
SET BRIGHTNESS 40      (dim)
```

**Brightness Guide:**
- 0-50: Dim (indoor, low power)
- 80: Default (balanced)
- 150: Bright (outdoor)
- 255: Maximum (requires adequate power supply)

#### Set Bottom LED Count:
```
SET LEDS <8|12>
```

Configure for your hardware version.

**Examples:**
```
SET LEDS 12        (V2 hardware - 12 LEDs in 6 pairs)
SET LEDS 8         (V1 hardware - 8 individual LEDs)
```

#### Set Startup Sequence:
```
SET STARTUP Q<n>
SET STARTUP NONE
```

Choose which sequence runs automatically on boot.

**Examples:**
```
SET STARTUP Q0         (R2-D2 startup sequence)
SET STARTUP Q11        (Calm blue on boot)
SET STARTUP NONE       (no auto-start)
```

#### Enable/Disable Auto-Start:
```
SET AUTOSTART ON
SET AUTOSTART OFF
```

**Examples:**
```
SET AUTOSTART ON       (run startup sequence on boot)
SET AUTOSTART OFF      (stay off on boot)
```

#### Save All Settings:
```
SAVE
```

Saves all configuration, custom colors, and custom sequences to flash memory. Settings persist across reboots.

#### Reset to Factory Defaults:
```
RESET
```

**Warning**: This erases all custom sequences, colors, and settings!

### Configuration Workflow Example:

```
# 1. Check current config
CONFIG

# 2. Set brightness
SET BRIGHTNESS 120

# 3. Configure hardware
SET LEDS 12

# 4. Set startup behavior
SET STARTUP Q11
SET AUTOSTART ON

# 5. Define custom colors
COLOR RGB 10 0 100 255 Ocean Blue
COLOR RGB 11 255 50 0 Sunset Orange

# 6. Create custom sequence
SEQ NEW S1 Morning Startup
SEQ ADD S1 X DELAY 300
SEQ ADD S1 M1105 DELAY 500
SEQ ADD S1 A1105
SEQ SAVE S1

# 7. Save everything to flash
SAVE

# 8. Verify configuration
CONFIG
COLOR LIST
SEQ LIST
```

---

## 🛠️ Advanced Configuration

### Hardware Version Selection

The sketch supports both old and new bottom LED hardware. Edit in sketch:

```cpp
// Uncomment ONE of the following to match your hardware:
#define BOTTOM_LED_V2  // New board: 12 LEDs in pairs (default)
// #define BOTTOM_LED_V1  // Old board: 8 individual LEDs
```

**Bottom LED V2 (New - Default):**
- 12 physical LEDs controlled in 6 pairs
- LEDs 1&2, 3&4, 5&6, 7&8, 9&10, 11&12
- Provides uniform lighting across pairs
- Total: 49 LEDs in periscope

**Bottom LED V1 (Old):**
- 8 individual LEDs
- Independent control
- Total: 45 LEDs in periscope

### Default Brightness

Edit in sketch:
```cpp
#define BRIGHTNESS 80   // 0-255 (default: 80)
```

Note: This can be overridden at runtime via `SET BRIGHTNESS` command.

### Color Order

If colors appear wrong, adjust:
```cpp
#define COLOR_ORDER GRB  // Try RGB or BGR if colors incorrect
```

### Status LED Brightness

```cpp
#define STATUS_LED_BRIGHTNESS 64  // 0-255
```

### I2C Address

For remote control via I2C:
```cpp
#define I2C_ADDRESS 0x20  // I2C slave address (32 decimal)
```

---

## 🎮 Uppity Spinner Mode

Control via 3-pin hardware interface (8 states). Alternative to serial control.

### Enabling Uppity Spinner Mode:

Edit the sketch and uncomment:
```cpp
#define UPPITY_SPINNER_MODE
```

### Pin Connections:
- **Pin A**: GPIO20 (RX)
- **Pin B**: GPIO8 (SDA)
- **Pin C**: GPIO9 (SCL)

### Input States (3-bit = 8 states):

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

### Customizing Sequences:

Edit these defines in the sketch:
```cpp
#define UPPITY_STATE_0_SEQUENCE -1  // -1 = off, 0-31 = sequence
#define UPPITY_STATE_2_SEQUENCE 4   // Police lights
#define UPPITY_STATE_3_SEQUENCE 1   // Party mode
#define UPPITY_STATE_4_SEQUENCE 13  // Fire mode
#define UPPITY_STATE_5_SEQUENCE 11  // Calm blue
#define UPPITY_STATE_6_SEQUENCE 20  // Auto demo
#define UPPITY_STATE_7_SEQUENCE 6   // Knight Rider
```

**Note:** Serial communication is DISABLED in Uppity Spinner mode (RX pin is used for input).

---

## 🎨 Effect Showcase

### Fire Effect (Effect 13)
Realistic fire simulation using heat diffusion algorithm. Best with orange/red colors.
```
M13     (Main fire with auto colors)
Q13     (Full fire sequence)
S7      (Side fire effect)
```

### Rainbow Effect (Effect 12/13)
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

### Theater Chase (Effect 11/12/13)
Classic theater marquee chase pattern. Available on all LED groups.
```
M1185   (Main theater chase white medium)
Q30     (Theater mode - all synchronized)
```

### Twinkle Effect (Effect 10/11/12)
Random LED twinkling with smooth fade. Creates starfield effect.
```
M1085   (Main twinkle white medium)
T1289   (Top twinkle white fast)
```

### Color Gradient (Effect 20/15/13)
Smooth color transitions across LED strip. Showcases new gradient capability.
```
M2085   (Main color gradient medium)
Q29     (Color gradient demo sequence)
```

### Bounce Trail (Effect 19/14/12/13)
Bouncing LED with trailing fade effect. Available on most LED groups.
```
M1989   (Main bounce trail white fast)
Q27     (Celebration with bounce trails)
```

---

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

### Custom Sequences Not Saving

✅ **Save after creating:** Use `SEQ SAVE S<n>` after adding commands
✅ **Save to flash:** Use `SAVE` command to persist to flash
✅ **Verify with:** `SEQ LIST` to confirm sequence is saved
✅ **Check flash space:** ESP32-C3 has limited NVS space

### Custom Colors Not Working

✅ **Valid range:** Slots 0-19 only
✅ **Save after defining:** Use `SAVE` command
✅ **Verify with:** `COLOR LIST`
✅ **RGB/HSV ranges:** All values 0-255

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

---

## 💡 Status LED Indicators

The status LED (GPIO1) provides system feedback:

**Normal Operation:**
- **1 Hz blink** (1 sec on, 1 sec off): System running normally

**Boot Sequence:**
- **Solid ON** briefly: System initializing
- **Switches to 1 Hz blink**: Initialization complete

**Note:** In Uppity Spinner mode, a white flash on all LEDs indicates mode activation.

---

## 📊 Performance Specifications

| Specification | Value |
|---------------|-------|
| Total LEDs | 49 (12 bottom in 6 pairs) |
| Refresh Rate | ~60 FPS |
| Effect Count | 60+ (across all groups) |
| Built-in Sequences | 32 (Q0-Q31) |
| Custom Sequences | 10 (S1-S10) |
| Color Slots | 20 (10 default + 10 custom) |
| Speed Levels | 10 (0-9) |
| Control Modes | 3 (Serial + I2C + Uppity) |
| Power Consumption | 1.6-2.2A typical @ 5V |
| Baud Rate | 9600 (Serial mode) |
| I2C Address | 0x20 (32 decimal) |
| Boot Time | <2 seconds |
| Flash Storage | Custom sequences, colors, config |

---

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

---

## 🚀 Advanced Customization

### Creating Custom Built-in Sequences

Edit `processSequence()` function in the sketch to add new Q sequences:

```cpp
case 32: // Your custom sequence
  processCommand("M689");   // Main chase white fast
  delay(500);               // Pause
  processCommand("T189");   // Top pulse white medium
  delay(500);
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

### Custom Effects

Add new effects to LED classes (MainLeds.cpp, TopLeds.cpp, etc.):

```cpp
void MainLeds::yourCustomEffect() {
  // Your effect code here
  // Access: this->leds, this->numleds, this->currentColor

  // Example: Center pulse outward
  uint8_t center = numleds / 2;
  uint8_t wave = beatsin8(60, 0, center);

  fill_solid(leds, numleds, CRGB::Black);
  leds[center + wave] = currentColor;
  leds[center - wave] = currentColor;
}
```

Then add case to `update()` switch statement and increment effect count.

---

## 📦 Project File Structure

```
PD-Periscope/
├── R2D2_Periscope_C3_v2.1_Enhanced.ino  # Main sketch (v2.2)
├── Config.h / .cpp                       # Configuration system (NEW v2.2)
├── Constants.h                           # Centralized constants (v2.2)
├── BaseLeds.h / .cpp                     # Base class + common effects (v2.2)
├── MainLeds.h / .cpp                     # Main LED ring (9 LEDs)
├── TopLeds.h / .cpp                      # Top LEDs (7 LEDs)
├── BottomLeds.h / .cpp                   # Bottom LEDs (12 LEDs)
├── SideLeds.h / .cpp                     # Left/Right LEDs (9 each)
├── BackLeds.h / .cpp                     # Back LEDs (3 LEDs)
└── README.md                             # This file
```

---

## 📝 Version History

### Version 2.2 (2025/11)
**Major Code Refactoring - Quality & Safety Improvements**
- **CRITICAL:** Fixed buffer overflow vulnerability in MainLeds and SideLeds
- **CRITICAL:** Added virtual destructor to BaseLeds (prevents memory leaks)
- **CRITICAL:** Fixed static variable issues in multiple LED classes
- **HARDWARE:** Updated bottom LEDs from 8 to 12 (6 pairs for uniform lighting)
- **NEW:** Custom color management system (20 color slots, RGB/HSV, flash storage)
- **NEW:** Custom sequence creation system (S1-S10 user sequences)
- **NEW:** Comprehensive CLI configuration system
- **NEW:** Timed sequences (QxxTyy format: Q15T20 = run for 20 seconds, then auto-OFF)
- **NEW:** Q31 white double-flash sequence with continuous looping
- **NEW:** Q31 special behavior: loops forever until stopped by another command
- **NEW:** 10 new LED effects across all classes (twinkle, theater, bounce, gradient)
- **NEW:** 10 thematic sequences (Q21-Q30: Happy, Angry, Scared, Boot, etc.)
- Created Constants.h - eliminated 100+ magic numbers
- Created Config.h/.cpp - flash storage for all settings
- Created BaseLeds.cpp - common effect implementations
- Eliminated ~176 lines of code duplication
- Added 74+ pointer validation and bounds checks
- Consolidated 12 member variables into base class
- Improved code maintainability and extensibility
- Added setPair() function for LED pair control in BottomLeds
- 100% backward compatible API

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

---

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
| Dim LEDs | Increase BRIGHTNESS value or use SET BRIGHTNESS |
| Status LED not blinking | Check GPIO1 connection |
| Sequences not saving | Use SEQ SAVE and SAVE commands |
| Colors not persisting | Use SAVE command after COLOR RGB/HSV |

### Diagnostic Commands (Serial Mode)

```
HELP    # Comprehensive command reference
?       # Quick status
CONFIG  # Show all settings
ON      # Enable all LEDs
OFF     # Disable all LEDs
A388    # Test all LEDs (white, fast)
Q0      # Test sequence 0 (R2-D2 startup)
Q20     # Test auto-demo mode
COLOR LIST    # Show all colors
SEQ LIST      # Show custom sequences
```

---

## 📜 License & Credits

### Project Credits
- **Hardware Design**: Printed-Droid.com
- **Software Development**: Printed-Droid.com
- **Version**: 2.2
- **Date**: 2025/11

### Open Source Libraries
- **FastLED**: High-performance LED control library (3.9.0)
- **ESP32 Arduino Core**: ESP32-C3 support

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
