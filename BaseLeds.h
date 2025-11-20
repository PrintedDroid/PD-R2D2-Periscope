#ifndef BASELED_H
#define BASELED_H

#include <FastLED.h>

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
    virtual void update(unsigned long currentTime) = 0;
    virtual void setEffect(int effect) = 0;
    virtual void setColor(int color) = 0;
    virtual void setSpeed(int speed) = 0;
};

#endif