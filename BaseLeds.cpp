#include "BaseLeds.h"

// ============================================
// Common Method Implementations
// ============================================

void BaseLeds::setColor(int color) {
  if (color >= 0 && color <= LedConstants::MAX_COLOR_INDEX) {
    this->currentColor = color;
  }
}

void BaseLeds::setSpeed(int speed) {
  if (speed > 0) {
    this->speed = speed;
    this->pulse_speed = speed / 5;
    if (this->pulse_speed < LedConstants::PULSE_SPEED_MINIMUM) {
      this->pulse_speed = LedConstants::PULSE_SPEED_MINIMUM;
    }
  }
}

// ============================================
// Common Effects (Eliminates Code Duplication)
// ============================================

void BaseLeds::commonFire(byte* heat, int heatSize) {
  if (!validatePointers() || !heat) return;

  // Bounds check
  int effectiveSize = min(heatSize, this->numleds);

  // Cool down every cell a little
  for(int i = 0; i < effectiveSize; i++) {
    heat[i] = qsub8(heat[i], random8(0, ((LedConstants::FIRE_COOLING_BASE * LedConstants::FIRE_COOLING_MULTIPLIER) / effectiveSize) + 2));
  }

  // Heat from each cell drifts up and diffuses slightly
  for(int k = effectiveSize - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Randomly ignite new sparks near bottom
  if(random8() < LedConstants::FIRE_IGNITION_THRESHOLD) {
    int y = random8(min(LedConstants::FIRE_SPARK_RANGE, effectiveSize - 1));
    heat[y] = qadd8(heat[y], random8(LedConstants::FIRE_HEAT_MIN, LedConstants::FIRE_HEAT_MAX));
  }

  // Map from heat cells to LED colors
  for(int j = 0; j < effectiveSize; j++) {
    CRGB color = HeatColor(heat[j]);
    this->leds[j] = color;
  }
}

void BaseLeds::commonRainbow() {
  if (!validatePointers()) return;

  static uint8_t hue = 0;
  fill_rainbow(this->leds, this->numleds, hue, 255 / this->numleds);

  // Speed determines how fast the rainbow rotates
  uint8_t hueShift = map(this->speed, LedConstants::FADE_AMOUNT_STANDARD, 200,
                          LedConstants::RAINBOW_HUE_SHIFT_FAST, LedConstants::RAINBOW_HUE_SHIFT_SLOW);
  hue += hueShift;
}

void BaseLeds::commonSparkle(uint8_t threshold) {
  if (!validatePointers()) return;

  fadeToBlackBy(this->leds, this->numleds, LedConstants::FADE_AMOUNT_LIGHT);

  if (random8() < threshold) {
    int pos = random8(this->numleds);
    if (isValidIndex(pos)) {
      this->leds[pos] = getCurrentColor();
    }
  }
}

void BaseLeds::commonStrobe() {
  if (!validatePointers()) return;

  if (this->strobe_ind) {
    safeFillSolid(getCurrentColor());
  } else {
    safeFillSolid(CRGB::Black);
  }

  this->strobe_ind = !this->strobe_ind;
}

void BaseLeds::commonPulseAll() {
  if (!validatePointers()) return;

  CHSV hsv = rgb2hsv_approximate(getCurrentColor());
  hsv.v = this->pulse;

  for(int i = 0; i < this->numleds; i++) {
    this->leds[i] = hsv;
  }

  this->pulse += this->pulse_offset;
  if (this->pulse >= LedConstants::PULSE_VALUE_MAX || this->pulse <= LedConstants::PULSE_VALUE_MIN) {
    this->pulse_offset = -(this->pulse_offset);
  }
}

// ============================================
// Utility Methods
// ============================================

bool BaseLeds::isValidIndex(int index) const {
  return (index >= 0 && index < this->numleds);
}

void BaseLeds::safeFillSolid(CRGB color) {
  if (validatePointers()) {
    fill_solid(this->leds, this->numleds, color);
  }
}

CRGB BaseLeds::getCurrentColor() const {
  if (this->currentColor >= 0 && this->currentColor <= LedConstants::MAX_COLOR_INDEX) {
    return colorMap[this->currentColor];
  }
  return CRGB::White; // Fallback
}

// ============================================
// Validation Methods
// ============================================

bool BaseLeds::validatePointers() const {
  return (this->leds != nullptr && this->numleds > 0);
}

bool BaseLeds::validateNumLeds() const {
  return (this->numleds > 0 && this->numleds <= LedConstants::MAX_LEDS_PER_STRIP);
}
