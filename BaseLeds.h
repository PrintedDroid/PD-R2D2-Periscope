#ifndef BASELEDS_H
#define BASELEDS_H

#include <FastLED.h>
#include "Constants.h"

// Color palette for all LEDs
const CRGB colorMap[] = {
  CRGB::Red,      // 0
  CRGB::Yellow,   // 1
  CRGB::Green,    // 2
  CRGB::Cyan,     // 3
  CRGB::Blue,     // 4
  CRGB::Magenta,  // 5
  CRGB::Orange,   // 6
  CRGB::Purple,   // 7
  CRGB::White,    // 8
  CRGB::Pink      // 9
};

class BaseLeds {
  public:
    // Virtual destructor to prevent memory leaks
    virtual ~BaseLeds() = default;

    // Pure virtual methods - must be implemented by derived classes
    virtual void update(unsigned long currentTime) = 0;
    virtual void setEffect(int effect) = 0;

    // Common methods with default implementation
    virtual void setColor(int color);
    virtual void setSpeed(int speed);

  protected:
    // Common effects - eliminates code duplication
    void commonFire(byte* heat, int heatSize);
    void commonRainbow();
    void commonSparkle(uint8_t threshold = LedConstants::STROBE_THRESHOLD_SPARKLE);
    void commonStrobe();
    void commonPulseAll();

    // Utility methods
    bool isValidIndex(int index) const;
    void safeFillSolid(CRGB color);
    CRGB getCurrentColor() const;

    // Validation methods
    bool validatePointers() const;
    bool validateNumLeds() const;

    // Common member variables - shared by all LED classes
    CRGB* leds;
    int numleds;
    int speed;
    int idx;
    int currentEffect;
    int currentColor;
    bool autoChange;
    unsigned long lastUpdate;
    unsigned long effectChangeTime;

    // Pulse effect variables
    unsigned long pulse_speed;
    int pulse;
    int pulse_offset;

    // Strobe effect variable
    bool strobe_ind;
};

#endif
