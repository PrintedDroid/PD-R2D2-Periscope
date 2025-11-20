#ifndef SIDELEDS_H
#define SIDELEDS_H

#include "BaseLeds.h"

class SideLeds: public BaseLeds {
  public:
    SideLeds(CRGB *leds, int numleds);
    void update(unsigned long currentTime) override;
    void setEffect(int effect) override;

  private:
    void pulseEffect();  // Renamed from pulse() to avoid conflict with BaseLeds::pulse variable
    void cw_run(int pt);
    void breathe();

    // Fire effect heat array - fixed size with MAX_LEDS_PER_STRIP for safety
    byte heat[LedConstants::MAX_LEDS_PER_STRIP];
};

#endif
