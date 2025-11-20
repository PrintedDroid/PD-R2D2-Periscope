#ifndef BACKLEDS_H
#define BACKLEDS_H

#include "BaseLeds.h"

class BackLeds: public BaseLeds {
  public:
    BackLeds(CRGB *leds, int numleds);
    void update(unsigned long currentTime) override;
    void setEffect(int effect) override;

  private:
    void randomB();
    void randomColored();
    void alternateColors();
    void allOn();

    // Member variable for alternateColors phase
    bool alternatePhase;
};

#endif
