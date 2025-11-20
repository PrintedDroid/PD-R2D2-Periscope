#ifndef BOTTOM_H
#define BOTTOM_H

// Hardware Configuration
// Choose ONE of the following to match your hardware:
#define BOTTOM_LED_V2  // New board: 12 LEDs in pairs (1&2, 3&4, 5&6, 7&8, 9&10, 11&12) = 6 logical positions
// #define BOTTOM_LED_V1  // Old board: 8 individual LEDs

#include "BaseLeds.h"

class BottomLeds: public BaseLeds {
  public:
    BottomLeds(CRGB *leds, int numleds);
    void update(unsigned long currentTime) override;
    void setEffect(int effect) override;

    // Override common effects to use hardware abstraction
    void commonPulseAll();
    void commonStrobe();
    void commonSparkle(uint8_t threshold = LedConstants::STROBE_THRESHOLD_SPARKLE);
    void safeFillSolid(CRGB color);

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

    // Helper functions for hardware abstraction
    void setPosition(int position, CRGB color);  // Hardware-agnostic position setter
    int getLogicalCount() const;                 // Returns logical position count

    // Member variable for alternateRows
    bool alternateRowsPhase;
};

#endif
