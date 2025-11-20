#ifndef BACKLEDS_H
#define BACKLEDS_H

#include "BaseLeds.h"

class BackLeds: public BaseLeds {
  public:
    BackLeds(CRGB *leds, int numleds);
    void update(unsigned long currentTime);
    void setEffect(int effect);
    void setColor(int color);
    void setSpeed(int speed);

  private:
    void randomB();
    void randomColored();
    void alternateColors();
    void sparkle();
    void allOn();
    
    unsigned long lastUpdate;
    CRGB *leds;
    int numleds;
    int speed;
    int idx;
    int currentEffect;
    int currentColor;
};
  
#endif