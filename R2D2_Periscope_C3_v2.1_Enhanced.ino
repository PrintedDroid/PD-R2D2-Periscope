//
// R2D2 Periscope LED Controller for ESP32-C3 - Enhanced Version v2.2
// ====================================================================
// Sketch for the Printed-Droid.com Periscope
// Board: Lolin C3 Mini (ESP32-C3)
//
// VERSION 2.2 IMPROVEMENTS (2025/11):
// - CRITICAL: Fixed buffer overflow in MainLeds/SideLeds
// - CRITICAL: Added virtual destructor to BaseLeds
// - CRITICAL: Fixed static variable issues
// - Created Constants.h with 90+ named constants
// - Created BaseLeds.cpp with common effects
// - Eliminated 100+ magic numbers and ~176 lines of duplication
// - Added 74+ safety checks and validation
// - 100% backward compatible
//
// UPPITY SPINNER MODE:
// ====================
// To enable Uppity Spinner Mode, uncomment the following line:
// #define UPPITY_SPINNER_MODE
//
// When enabled:
// - Connect Uppity Spinner pins to: A=RX(GPIO20), B=SDA(GPIO8), C=SCL(GPIO9)
// - Serial commands are DISABLED (RX pin is used for Spinner)
// - LED state controlled by 3-bit input (0-7)
// - Input state 1 = OFF, all others = ON
//
// IMPORTANT - FastLED Library Version Requirements:
// ================================================
// COMPATIBLE: FastLED 3.9.0 ONLY
// WARNING:    Versions > 3.9.0 cause timing and display issues!
// 
// Known Issues with FastLED > 3.9.0:
// - Timing problems with multiple LED strips
// - Random flickering and color issues
// - RMT channel conflicts on ESP32
// - Unstable behavior with WS2812B LEDs
// - Complete failure to light LEDs
//
// To install correct version in Arduino IDE:
// 1. Go to: Sketch → Include Library → Manage Libraries
// 2. Search for "FastLED"
// 3. Click "Select version" dropdown
// 4. Choose version between 3.9.0 and 3.9.10
// 5. Click "Install"
//
// Enhanced Command Format (Serial Mode Only):
// ===========================================
// Format: [Target][Effect][Color][Speed]
// Example: M185 = Main LEDs, Effect 1, White, Speed 5
//
// Targets:
// - M: Main LEDs
// - T: Top LEDs
// - B: Bottom LEDs
// - S: Both Side LEDs
// - L: Left LEDs only
// - R: Right LEDs only
// - K: Back LEDs
// - A: All LEDs
// - X: All OFF
// - Q[0-9]: Sequences
//
// Effects (0-99):
// - 0: Off
// - 1-16+: Various effects per LED group
// - 99: Auto-change between effects
//
// Colors (0-9):
// - 0: Red, 1: Yellow, 2: Green, 3: Cyan, 4: Blue
// - 5: Magenta, 6: Orange, 7: Purple, 8: White, 9: Pink
//
// Speed (0-9):
// - 0: Slowest, 9: Fastest
//
// Legacy Commands:
// - "ON": Enable all LEDs
// - "OFF": Disable all LEDs
//
// Version: 2.2
// Date: 2025/11
// Author: Printed-Droid.com
//

// ========== CONFIGURATION ==========
// Uncomment the following line to enable Uppity Spinner Mode
// #define UPPITY_SPINNER_MODE

// Bottom LED Hardware Configuration
// Uncomment ONE of the following to match your hardware:
#define BOTTOM_LED_V2  // New board: 12 LEDs in pairs (default)
// #define BOTTOM_LED_V1  // Old board: 8 individual LEDs

// Uppity Spinner Sequence Selection (only used in UPPITY_SPINNER_MODE)
#define UPPITY_STATE_0_SEQUENCE -1  // -1 means all off, 0-20 means sequence Q0-Q20
#define UPPITY_STATE_2_SEQUENCE 4   // Police lights (Sequence 4)
#define UPPITY_STATE_3_SEQUENCE 1   // Party mode
#define UPPITY_STATE_4_SEQUENCE 13  // Fire mode (using Fire effect from MainLeds)
#define UPPITY_STATE_5_SEQUENCE 11  // Calm blue
#define UPPITY_STATE_6_SEQUENCE 20  // Auto demo
#define UPPITY_STATE_7_SEQUENCE 6   // Knight Rider

#include <FastLED.h>
#include "BottomLeds.h"
#include "MainLeds.h"
#include "SideLeds.h"
#include "BackLeds.h"
#include "TopLeds.h"

#define BRIGHTNESS 80   // 0-255, higher number is brighter. 
#define COLOR_ORDER GRB

// Pin definitions for Lolin C3 Mini
#define STATUS_LED_PIN 1 // GPIO1 for status LED
#define STATUS_LED_BRIGHTNESS 64 // 0-255, where 255 is full brightness

#define RX_PIN 20       // GPIO20 = RX
#define SDA_PIN 8       // GPIO8 = SDA  
#define SCL_PIN 9       // GPIO9 = SCL

// LED Strip pins
#define MAIN_PIN 5
#define RIGHT_PIN 7
#define LEFT_PIN 4
#define BOTTOM_PIN 3
#define TOP_PIN 6
#define BACK_PIN 10

// Uppity Spinner pin mapping
#define UPPITY_PIN_A RX_PIN
#define UPPITY_PIN_B SDA_PIN
#define UPPITY_PIN_C SCL_PIN

// LED configurations
#define MAIN_NUMLEDS 9
#define RIGHT_NUMLEDS 9
#define LEFT_NUMLEDS 9

// Bottom LED count based on hardware version
#ifdef BOTTOM_LED_V2
  #define BOTTOM_NUMLEDS 12  // New: 12 LEDs in pairs (1&2, 3&4, 5&6, 7&8, 9&10, 11&12) = 6 logical positions
#else
  #define BOTTOM_NUMLEDS 8   // Old: 8 individual LEDs
#endif

#define TOP_NUMLEDS 7
#define BACK_NUMLEDS 3

// LED arrays
CRGB main_leds[MAIN_NUMLEDS];
CRGB right_leds[RIGHT_NUMLEDS];
CRGB left_leds[LEFT_NUMLEDS];
CRGB bottom_leds[BOTTOM_NUMLEDS];
CRGB top_leds[TOP_NUMLEDS];
CRGB back_leds[BACK_NUMLEDS];

// LED objects
TopLeds    topLeds(top_leds, TOP_NUMLEDS);
BottomLeds bottomLeds(bottom_leds, BOTTOM_NUMLEDS);
MainLeds   mainLeds(main_leds, MAIN_NUMLEDS);
SideLeds   leftLeds(left_leds, LEFT_NUMLEDS);
SideLeds   rightLeds(right_leds, RIGHT_NUMLEDS);
BackLeds   backLeds(back_leds, BACK_NUMLEDS);

// Global state
bool activated = true;

// Status LED variables
unsigned long statusLedLastToggle = 0;
bool statusLedState = false;

#ifndef UPPITY_SPINNER_MODE
// Command handling for Serial mode
#define MAX_COMMAND_LENGTH 10
char commandBuffer[MAX_COMMAND_LENGTH];
String commandString = "";
volatile boolean commandComplete = false;
#else
// Uppity Spinner mode variables
int currentUppityState = -1;
int lastUppityState = -1;
#endif

// Global variables for demo mode
int currentDemoSequence = 0;
unsigned long lastDemoSequenceChange = 0;
bool inDemoMode = false;

// Forward declarations
void processCommand(String cmd);
void processSequence(int seq);
void clearLEDs();

void setup() {
  // Setup status LED
  pinMode(STATUS_LED_PIN, OUTPUT);
  
#ifndef UPPITY_SPINNER_MODE
  Serial.begin(9600);
  Serial.println("Starting Enhanced R2D2 Periscope - Serial Mode (Final Correction)");
  Serial.println("Commands: [Target][Effect][Color][Speed]");
  Serial.println("Example: M1285 = Main LEDs, Effect 12, White, Speed 5");

  // Debug: Show hardware configuration
  Serial.print("Bottom LEDs: ");
  Serial.print(BOTTOM_NUMLEDS);
  #ifdef BOTTOM_LED_V2
    Serial.println(" (V2 - 12 LEDs in 6 pairs)");
  #else
    Serial.println(" (V1 - 8 individual LEDs)");
  #endif
#else
  // Configure Uppity Spinner pins
  pinMode(UPPITY_PIN_A, INPUT_PULLUP);
  pinMode(UPPITY_PIN_B, INPUT_PULLUP);
  pinMode(UPPITY_PIN_C, INPUT_PULLUP);
#endif
    
  delay(1000); // power-up safety delay
  
  // Initialize all LED strips
  FastLED.addLeds<WS2812, MAIN_PIN, COLOR_ORDER>(main_leds, MAIN_NUMLEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812, RIGHT_PIN, COLOR_ORDER>(right_leds, RIGHT_NUMLEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812, LEFT_PIN, COLOR_ORDER>(left_leds, LEFT_NUMLEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812, BOTTOM_PIN, COLOR_ORDER>(bottom_leds, BOTTOM_NUMLEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812, TOP_PIN, COLOR_ORDER>(top_leds, TOP_NUMLEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812, BACK_PIN, COLOR_ORDER>(back_leds, BACK_NUMLEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  
#ifdef UPPITY_SPINNER_MODE
  // Visual indication of Uppity Spinner mode - quick white flash
  fill_solid(main_leds, MAIN_NUMLEDS, CRGB::White);
  fill_solid(top_leds, TOP_NUMLEDS, CRGB::White);
  FastLED.show();
  delay(200);
  clearLEDs();
  FastLED.show();
  delay(200);
#else
  // Start with original R2D2 sequence in Serial mode
  processSequence(0);
#endif
}

void loop() {
  unsigned long currentTime = millis();

  // Update status LED - 1 second on, 1 second off
  if (currentTime - statusLedLastToggle >= 1000) {
    statusLedState = !statusLedState;
    digitalWrite(STATUS_LED_PIN, statusLedState);
    statusLedLastToggle = currentTime;
  }
  
#ifndef UPPITY_SPINNER_MODE
  // Serial mode operation
  checkSerial();

  // Process commands
  if(commandComplete) {
    commandString.trim();
    commandString.toUpperCase();

    Serial.print(F("Received: "));
    Serial.println(commandString);

    if (commandString == "ON") {
      activated = true;
      Serial.println("System: ON");
    }
    else if (commandString == "OFF") {
      activated = false;
      Serial.println("System: OFF");
    }
    else if (commandString.startsWith("Q")) {
      int seqNum = commandString.substring(1).toInt();
      processSequence(seqNum);
    }
    else if (commandString == "?") {
      printStatus();
    }
    else {
      processCommand(commandString);
    }
    
    commandString = "";
    commandComplete = false;
  }
#else
  // Uppity Spinner mode operation
  int inputState = ((uint8_t(!digitalRead(UPPITY_PIN_A)) << 2) |
                    (uint8_t(!digitalRead(UPPITY_PIN_B)) << 1) |
                    (uint8_t(!digitalRead(UPPITY_PIN_C)) << 0));

  currentUppityState = inputState;
  
  // Check if state has changed
  if (currentUppityState != lastUppityState) {
    lastUppityState = currentUppityState;

    // Process state change
    switch(currentUppityState) {
      case 0:
        if (UPPITY_STATE_0_SEQUENCE >= 0) {
          activated = true;
          processSequence(UPPITY_STATE_0_SEQUENCE);
        } else {
          activated = false; // All off
        }
        break;
      case 1:
        activated = false; // Always OFF for state 1
        break;
      case 2: activated = true; processSequence(UPPITY_STATE_2_SEQUENCE); break;
      case 3: activated = true; processSequence(UPPITY_STATE_3_SEQUENCE); break;
      case 4: activated = true; processSequence(UPPITY_STATE_4_SEQUENCE); break;
      case 5: activated = true; processSequence(UPPITY_STATE_5_SEQUENCE); break;
      case 6: activated = true; processSequence(UPPITY_STATE_6_SEQUENCE); break;
      case 7: activated = true; processSequence(UPPITY_STATE_7_SEQUENCE); break;
    }
  }
#endif

  // Update LEDs if activated
  if (activated) {
    // Handle demo mode sequence cycling
    if (inDemoMode) {
      if (millis() - lastDemoSequenceChange > 15000) { // 15 seconds per sequence
        currentDemoSequence++;
        if (currentDemoSequence >= 20) {
          currentDemoSequence = 0; // Loop back to start
        }
        processSequence(currentDemoSequence);
        lastDemoSequenceChange = millis();
      }
    }
    
    mainLeds.update(currentTime);
    topLeds.update(currentTime);
    leftLeds.update(currentTime);
    rightLeds.update(currentTime);
    bottomLeds.update(currentTime);
    backLeds.update(currentTime);
  }
  else {
    clearLEDs();
  }
  
  FastLED.show();
}

void clearLEDs() {
  fill_solid(main_leds, MAIN_NUMLEDS, CRGB::Black);
  fill_solid(left_leds, LEFT_NUMLEDS, CRGB::Black);
  fill_solid(right_leds, RIGHT_NUMLEDS, CRGB::Black);
  fill_solid(top_leds, TOP_NUMLEDS, CRGB::Black);
  fill_solid(bottom_leds, BOTTOM_NUMLEDS, CRGB::Black);
  fill_solid(back_leds, BACK_NUMLEDS, CRGB::Black);
}

#ifndef UPPITY_SPINNER_MODE
void checkSerial() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\r' || inChar == '\n') {
      if (commandString.length() > 0) {
        commandComplete = true;
      }
    } else {
      if (commandString.length() < MAX_COMMAND_LENGTH - 1) {
        commandString += inChar;
      }
    }
  }
}

void printStatus() {
  Serial.println(F("\n=== LED Status ==="));
  Serial.print(F("System: "));
  Serial.println(activated ? "ON" : "OFF");
  Serial.println(F("Use '?' for status, 'ON'/'OFF' to control."));
  Serial.println(F("Commands: [Target][Effect][Color][Speed]"));
  Serial.println(F("Targets: M, T, B, S, L, R, K, A, X"));
  Serial.println(F("Effects: 0-16+ (e.g., 99 for Auto)"));
  Serial.println(F("Colors: 0-9"));
  Serial.println(F("Speed: 0-9"));
  Serial.println(F("Sequences: Q0-Q20 (e.g., Q4=Police, Q6=Knight Rider)"));
  Serial.println(F("==================\n"));
}
#endif

// Corrected, robust command processing function
void processCommand(String cmd) {
  if (cmd.length() < 2) return;

  char target = cmd.charAt(0);
  if (target == 'X') {
    mainLeds.setEffect(0);
    topLeds.setEffect(0);
    bottomLeds.setEffect(0);
    leftLeds.setEffect(0);
    rightLeds.setEffect(0);
    backLeds.setEffect(0);
    clearLEDs();
    FastLED.show();
    Serial.println("Set All to OFF");
    return;
  }

  String params = cmd.substring(1);
  if (params.length() == 0) return;

  int effect = -1, color = -1, speed = -1;

  // This parser reads from right to left to correctly handle variable-length effect numbers.
  // Example: "1285" -> speed=5, color=8, effect=12
  // Example: "185"  -> speed=5, color=8, effect=1
  // Example: "99"   -> speed=-1, color=-1, effect=99
  
  long value = params.toInt();

  if (params.length() > 2) { // Assumes format Effect-Color-Speed (e.g. M185 or M1285)
      speed = value % 10;
      color = (value / 10) % 10;
      effect = value / 100;
  } else { // Assumes format is only an Effect number (e.g. M99, or M1, or M7)
      effect = value;
  }


  // Lambda function to apply settings to any LED object
  auto apply = [&](BaseLeds &leds, int speed_map_low, int speed_map_high) {
    if (effect >= 0) leds.setEffect(effect);
    if (color >= 0) leds.setColor(color);
    if (speed >= 0) leds.setSpeed(map(speed, 0, 9, speed_map_low, speed_map_high));
  };

  switch (target) {
    case 'M': apply(mainLeds, 200, 20); break;
    case 'T': apply(topLeds, 400, 50); break;
    case 'B': apply(bottomLeds, 400, 50); break;
    case 'S': apply(leftLeds, 400, 50); apply(rightLeds, 400, 50); break;
    case 'L': apply(leftLeds, 400, 50); break;
    case 'R': apply(rightLeds, 400, 50); break;
    case 'K': apply(backLeds, 2000, 200); break;
    case 'A':
      apply(mainLeds, 200, 20);
      apply(topLeds, 400, 50);
      apply(bottomLeds, 400, 50);
      apply(leftLeds, 400, 50);
      apply(rightLeds, 400, 50);
      apply(backLeds, 2000, 200);
      break;
  }

#ifndef UPPITY_SPINNER_MODE
  Serial.print("Set ");
  Serial.print(target);
  Serial.print(": E=");
  Serial.print(effect);
  if (color >= 0) {
    Serial.print(" C=");
    Serial.print(color);
  }
  if (speed >= 0) {
    Serial.print(" S=");
    Serial.print(speed);
  }
  Serial.println();
#endif
}

void processSequence(int seq) {
  // Disable demo mode if any other sequence is selected
  if (seq != 20) {
    inDemoMode = false;
  }
  
#ifndef UPPITY_SPINNER_MODE
  Serial.print("Sequence ");
  Serial.println(seq);
#endif
  
  switch(seq) {
    case 0: // Original R2D2 startup
      processCommand("M185");   // Main pulse white medium
      processCommand("T285");   // Top left-right white medium
      processCommand("S385");   // Sides CW run 2 white medium
      processCommand("B105");   // Bottom superscan red medium
      processCommand("K105");   // Back random red/blue medium
      break;
    case 1: // Party mode
      processCommand("A99");    // All rainbow/auto effects
      processCommand("M12");    // Main rainbow specifically
      break;
    case 2: // Bright Pulse - Maximum brightness white pulse
      processCommand("M188");   // Main pulse white fast
      processCommand("T888");   // Top pulse white fast
      processCommand("S188");   // Sides pulse white fast
      processCommand("B188");   // Bottom simple white fast (ORIGINAL Effect 3)
      processCommand("K388");   // Back all on white fast
      break;
    case 3: // Communication Mode
      processCommand("S589");   // Sides strobe white very fast
      processCommand("T689");   // Top chase white very fast
      processCommand("M949");   // Main strobe blue very fast
      processCommand("B489");   // Bottom random white very fast
      processCommand("K589");   // Back sparkle white very fast
      break;
    case 4: // Police lights
      processCommand("L509");   // Left strobe red fast
      processCommand("R549");   // Right strobe blue fast
      processCommand("T249");   // Top chase blue fast
      processCommand("B409");   // Bottom random red fast
      processCommand("M989");   // Main strobe white fast
      break;
    case 5: // Alarm/Warning
      processCommand("M903");   // Main strobe red medium
      processCommand("T905");   // Top bounce red medium
      processCommand("S505");   // Sides strobe red medium
      processCommand("B805");   // Bottom alternate rows red medium
      processCommand("K105");   // Back random red medium
      break;
    case 6: // Knight Rider
      processCommand("M609");   // Main split red fast
      processCommand("S209");   // Sides run red fast
      processCommand("T1109");  // Top Knight Rider red fast
      processCommand("B109");   // Bottom superscan red fast
      break;
    case 7: // Searchlight scanning - ALL WHITE
      processCommand("M388");   // Main all on white fast
      processCommand("T188");   // Top all on white fast
      processCommand("S188");   // Sides all on white fast
      processCommand("B288");   // Bottom all on white fast
      processCommand("K388");   // Back all on white fast
      break;
    case 8: // Stealth search mode
      processCommand("M1404");  // Main circle chase red very slow
      processCommand("T900");   // Top bounce red very slow
      processCommand("S204");   // Sides run single red slow
      processCommand("B300");   // Bottom simple red slow
      processCommand("K0");     // Back off
      break;
    case 9: // Dive
      processCommand("M1643");  // Main spiral out blue slow
      processCommand("T643");   // Top chase blue slow
      processCommand("S643");   // Sides breathe blue slow
      processCommand("B943");   // Bottom snake blue slow
      processCommand("K243");   // Back random blue slow
      break;
    case 10: // Surface
      processCommand("M1533");  // Main center expand cyan medium
      processCommand("T333");   // Top to center cyan medium
      processCommand("S635");   // Sides breathe cyan medium
      processCommand("B735");   // Bottom wave cyan medium
      processCommand("K535");   // Back sparkle cyan medium
      break;
    case 11: // Calm blue
      processCommand("A643");   // All breathe blue slow
      processCommand("M1040");  // Main smooth pulse blue very slow
      break;
    case 12: // Boot-up/System Check
      processCommand("X");      // All off first
      delay(500);
      processCommand("T122");   // Top left run green slow
      delay(500);
      processCommand("M122");   // Main pulse green slow
      delay(500);
      processCommand("S122");   // Sides pulse green slow
      delay(500);
      processCommand("B122");   // Bottom simple green slow
      processCommand("K322");   // Back all on green slow
      break;
    case 13: // Fire
      processCommand("M13");    // Main fire
      processCommand("S7");     // Sides fire
      processCommand("B462");   // Bottom random orange slow
      processCommand("K462");   // Back alternate orange slow
      break;

    case 14: // Celebration/Victory
      processCommand("A99");    // All auto-change
      break;
    case 15: // Energy Charging
      processCommand("B747");   // Bottom wave blue fast
      processCommand("S647");   // Sides breathe blue fast
      processCommand("T1047");  // Top fill from center blue fast
      processCommand("M1047");  // Main smooth pulse blue fast
      processCommand("K347");   // Back all on blue fast
      break;
    case 16: // Hyperdrive/Warp
      processCommand("M1689");  // Main spiral out white very fast
      processCommand("T689");   // Top chase white very fast
      processCommand("S489");   // Sides cw run 4 white very fast
      processCommand("B689");   // Bottom comet white very fast
      processCommand("K589");   // Back sparkle white very fast
      break;
    case 17: // Malfunction - RED/YELLOW alternating
      processCommand("M905");   // Main strobe red medium
      processCommand("T415");   // Top sparkle yellow medium
      processCommand("S505");   // Sides strobe red medium
      processCommand("B415");   // Bottom random yellow medium
      processCommand("K205");   // Back random red medium
      break;
    case 18: // Scan Complete
      processCommand("B222");   // Bottom scan green medium
      processCommand("S625");   // Sides breathe green medium
      processCommand("T325");   // Top to center green medium
      processCommand("M1525");  // Main center expand green medium
      delay(500);
      processCommand("A122");   // All pulse green slow
      break;
    case 19: // Sonar Ping
      processCommand("M1534");  // Main center expand cyan slow
      processCommand("T1034");  // Top fill from center cyan slow
      processCommand("S634");   // Sides breathe cyan slow
      processCommand("B834");   // Bottom alternate rows cyan slow
      processCommand("K334");   // Back all on cyan slow
      break;
    case 20: // Auto demo - Cycle through ALL sequences
      inDemoMode = true;
      currentDemoSequence = 0;
      lastDemoSequenceChange = millis();
      processSequence(0); // Start with first sequence
      
#ifndef UPPITY_SPINNER_MODE
      Serial.println("Demo mode: Cycling through all sequences");
      Serial.println("15 seconds per sequence");
#endif
      break;
  }
}