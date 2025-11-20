#ifndef TOPLEDS_H
#define TOPLEDS_H

#include "BaseLeds.h"

class TopLeds: public BaseLeds {
  public:
    TopLeds(CRGB *leds, int numleds);
    void update(unsigned long currentTime);
    void setEffect(int effect);
    void setColor(int color);
    void setSpeed(int speed);

  private:
    void leftrun();
    void leftright();
    void tocenter();
    void sparkle();
    void comet();
    void chase();
    void rainbow();
    void pulse();
    void bounce();
    void fillFromCenter();
    void knightRider();
    
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