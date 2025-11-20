#ifndef BOTTOM_H
#define BOTTOM_H

#include "BaseLeds.h"

class BottomLeds: public BaseLeds {
  public:
    BottomLeds(CRGB *leds, int numleds);
    void update(unsigned long currentTime);
    void setEffect(int effect);
    void setColor(int color);
    void setSpeed(int speed);
  
  private:
    void simple();
    void scan();
    void superscan();
    void randomLight();
    void chase();
    void comet();
    void wave();
    void alternateRows();
    void snake();
    
    unsigned long lastUpdate;
    unsigned long effectChangeTime;
    CRGB *leds;
    int numleds;
    int speed;
    int idx;
    int currentEffect;
    int currentColor;
    bool autoChange;
};

#endif