#ifndef MAINLEDS_H
#define MAINLEDS_H

#include "BaseLeds.h"

class MainLeds: public BaseLeds {
  public:
    MainLeds(CRGB *leds, int numleds);
    void update(unsigned long currentTime) override;
    void setEffect(int effect) override;

  private:
    void pulseCenter();
    void cw_run(int pt);
    void cw_split2();
    void cw_split3();
    void cw_split4();
    void smoothPulse();
    void theaterChase();
    void circleChase();
    void centerExpand();
    void spiralOut();

    // Fire effect heat array - fixed size with MAX_LEDS_PER_STRIP for safety
    byte heat[LedConstants::MAX_LEDS_PER_STRIP];
};

#endif
