#ifndef TOPLEDS_H
#define TOPLEDS_H

#include "BaseLeds.h"

class TopLeds: public BaseLeds {
  public:
    TopLeds(CRGB *leds, int numleds);
    void update(unsigned long currentTime) override;
    void setEffect(int effect) override;

  private:
    void leftrun();
    void leftright();
    void tocenter();
    void comet();
    void chase();
    void pulse();
    void bounce();
    void fillFromCenter();
    void knightRider();

    // Member variable for bounce and knightRider direction
    int bounceDirection;
};

#endif
