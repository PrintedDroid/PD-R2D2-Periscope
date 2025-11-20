#ifndef BOTTOM_H
#define BOTTOM_H

#include "BaseLeds.h"

class BottomLeds: public BaseLeds {
  public:
    BottomLeds(CRGB *leds, int numleds);
    void update(unsigned long currentTime) override;
    void setEffect(int effect) override;

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

    // Member variable for alternateRows
    bool alternateRowsPhase;
};

#endif
