#ifndef BASELEDS_H
#define BASELEDS_H

#include <FastLED.h>
#include "Constants.h"

// Color palette for all LEDs (defined in main .ino file, managed by Config)
#define MAX_COLOR_SLOTS 20
extern CRGB colorMap[MAX_COLOR_SLOTS];

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
    void commonTwinkle();
    void commonTheaterChase();
    void commonBounceWithTrail();
    void commonColorGradient();

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

    // Theater chase effect variable
    int theater_chase_q;
};

#endif
