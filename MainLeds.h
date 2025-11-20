#ifndef MAINLEDS_H
#define MAINLEDS_H

#include "BaseLeds.h"

class MainLeds: public BaseLeds {
  public:
    MainLeds(CRGB *leds, int numleds);
    void update(unsigned long currentTime);
    void setEffect(int effect);
    void setColor(int color);
    void setSpeed(int speed);
  
  private:
    void pulseCenter();
    void pulseAll();
    void cw_run(int pt);
    void cw_split2();
    void cw_split3();
    void cw_split4();
    void strobe();
    void smoothPulse();
    void theaterChase();
    void rainbow();
    void fire();
    void circleChase();
    void centerExpand();
    void spiralOut();

    unsigned long lastUpdate;
    unsigned long effectChangeTime;
    CRGB *leds;
    int numleds;
    int speed;
    int idx;

    unsigned long pulse_speed;
    int pulse;
    int pulse_offset;

    bool strobe_ind;
    int currentEffect;
    int currentColor;
    bool autoChange;
    
    // Fire effect variables
    byte heat[9];
};
  
#endif