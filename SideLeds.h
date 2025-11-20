#ifndef SIDELEDS_H
#define SIDELEDS_H

#include "BaseLeds.h"

class SideLeds: public BaseLeds {
  public:
    SideLeds(CRGB *leds, int numleds);
    void update(unsigned long currentTime);
    void setEffect(int effect);
    void setColor(int color);
    void setSpeed(int speed);
  
  private:
    void pulse();
    void strobe();
    void cw_run(int pt);
    void breathe();
    void fire();
    void sparkle();
    void rainbow();
    
    unsigned long lastUpdate;
    unsigned long effect_time;
    CRGB *leds;
    int numleds;
    int speed;
    int idx;

    int pulse_offset;
    bool strobe_ind;

    int currentEffect;
    int currentColor;
    bool autoChange;
    
    // Fire effect variables
    byte heat[9];
};
  
#endif