#include "TopLeds.h"

TopLeds::TopLeds(CRGB *leds, int numleds)
{
  // Initialize base class members
  this->leds = leds;
  this->numleds = numleds;
  this->lastUpdate = millis();
  this->effectChangeTime = millis();
  this->speed = LedConstants::DEFAULT_SPEED_TOP;
  this->idx = 0;
  this->pulse_speed = LedConstants::PULSE_SPEED_DEFAULT;
  this->pulse = LedConstants::PULSE_VALUE_DEFAULT;
  this->pulse_offset = LedConstants::PULSE_OFFSET_DEFAULT;
  this->strobe_ind = false;
  this->theater_chase_q = 0;
  this->currentEffect = 0;
  this->currentColor = LedConstants::DEFAULT_COLOR_BLUE;
  this->autoChange = false;
  this->bounceDirection = 1;

  // Validation
  if (!validatePointers()) {
    #ifdef DEBUG_MODE
      Serial.println(F("ERROR: TopLeds - Invalid LED pointer"));
    #endif
  }

  if (!validateNumLeds()) {
    #ifdef DEBUG_MODE
      Serial.print(F("WARNING: TopLeds - NumLEDs out of range: "));
      Serial.println(numleds);
    #endif
  }
}

void TopLeds::setEffect(int effect) {
  this->currentEffect = effect;
  this->idx = 0;
  this->pulse = LedConstants::PULSE_VALUE_DEFAULT;
  this->pulse_offset = LedConstants::PULSE_OFFSET_DEFAULT;
  this->strobe_ind = false;
  this->bounceDirection = 1;

  if (effect == 99) {
    this->autoChange = true;
    this->currentEffect = 0;
  } else {
    this->autoChange = false;
  }
}

void TopLeds::update(unsigned long currentTime)
{
  if (!validatePointers()) return;

  if ((currentTime - this->lastUpdate) < this->speed) return;

  // Auto-change mode
  if (this->autoChange && (currentTime - this->effectChangeTime) > LedConstants::AUTO_CHANGE_INTERVAL_SIDE) {
    this->currentEffect++;
    this->idx = 0;
    this->bounceDirection = 1;
    this->effectChangeTime = currentTime;

    if (this->currentEffect >= LedConstants::MAX_EFFECT_TOP) {
      this->currentEffect = 1; // Skip 0 (off) in auto mode
    }
  }

  switch (this->currentEffect) {
    case 0: // Off
      safeFillSolid(CRGB::Black);
      break;
    case 1: // Left run
      this->leftrun();
      break;
    case 2: // Left-right
      this->leftright();
      break;
    case 3: // To center
      this->tocenter();
      break;
    case 4: // Sparkle
      commonSparkle();
      break;
    case 5: // Comet
      this->comet();
      break;
    case 6: // Chase
      this->chase();
      break;
    case 7: // Rainbow
      commonRainbow();
      break;
    case 8: // Pulse
      this->pulseEffect();
      break;
    case 9: // Bounce
      this->bounce();
      break;
    case 10: // Fill from center
      this->fillFromCenter();
      break;
    case 11: // Knight Rider
      this->knightRider();
      break;
    case 12: // Twinkle
      commonTwinkle();
      break;
    case 13: // Theater Chase (common)
      commonTheaterChase();
      break;
    case 14: // Bounce with Trail
      commonBounceWithTrail();
      break;
    case 15: // Color Gradient
      commonColorGradient();
      break;
  }

  this->lastUpdate = currentTime;
}

void TopLeds::leftrun()
{
  if (!validatePointers()) return;

  safeFillSolid(CRGB::Black);

  if (isValidIndex(this->idx)) {
    this->leds[this->idx] = getCurrentColor();
  }

  this->idx++;
  if (this->idx >= this->numleds) {
    this->idx = 0;
  }
}

void TopLeds::leftright()
{
  if (!validatePointers()) return;

  safeFillSolid(CRGB::Black);

  int left = this->idx;
  int right = (this->numleds - 1) - this->idx;

  if (isValidIndex(left)) {
    this->leds[left] = getCurrentColor();
  }
  if (isValidIndex(right)) {
    this->leds[right] = getCurrentColor();
  }

  this->idx++;
  if (this->idx >= LedConstants::INDEX_SIX) {
    this->idx = 0;
  }
}

void TopLeds::tocenter()
{
  if (!validatePointers()) return;

  safeFillSolid(CRGB::Black);

  int left = this->idx;
  int right = (this->numleds - 1) - this->idx;

  if (isValidIndex(left)) {
    this->leds[left] = getCurrentColor();
  }
  if (isValidIndex(right)) {
    this->leds[right] = getCurrentColor();
  }

  this->idx++;
  if (this->idx >= LedConstants::INDEX_FOUR) {
    this->idx = 0;
  }
}

void TopLeds::comet()
{
  if (!validatePointers()) return;

  fadeToBlackBy(this->leds, this->numleds, LedConstants::FADE_AMOUNT_STANDARD);

  int pos = beatsin16(LedConstants::BEATSIN_FREQUENCY_BREATHE, 0, this->numleds - 1);
  if (isValidIndex(pos)) {
    this->leds[pos] = getCurrentColor();

    // Add trail
    if (isValidIndex(pos - 1)) {
      this->leds[pos - 1] = getCurrentColor();
      this->leds[pos - 1].fadeToBlackBy(LedConstants::FADE_MULTIPLIER_THEATER);
    }
    if (isValidIndex(pos + 1)) {
      this->leds[pos + 1] = getCurrentColor();
      this->leds[pos + 1].fadeToBlackBy(LedConstants::FADE_MULTIPLIER_THEATER);
    }
  }
}

void TopLeds::chase()
{
  if (!validatePointers()) return;

  fadeToBlackBy(this->leds, this->numleds, LedConstants::FADE_AMOUNT_STANDARD);

  if (isValidIndex(this->idx)) {
    this->leds[this->idx] = getCurrentColor();
  }

  this->idx++;
  if (this->idx >= this->numleds) {
    this->idx = 0;
  }
}

void TopLeds::pulseEffect()
{
  if (!validatePointers()) return;

  uint8_t brightness = beatsin8(LedConstants::BEATSIN_FREQUENCY_PULSE,
                                 LedConstants::PULSE_VALUE_MIN,
                                 LedConstants::PULSE_VALUE_MAX);
  CRGB color = getCurrentColor();

  for(int i = 0; i < this->numleds; i++) {
    this->leds[i] = color;
    this->leds[i].nscale8(brightness);
  }
}

void TopLeds::bounce()
{
  if (!validatePointers()) return;

  safeFillSolid(CRGB::Black);

  if (isValidIndex(this->idx)) {
    this->leds[this->idx] = getCurrentColor();

    // Add slight trail
    if (this->bounceDirection > 0 && isValidIndex(this->idx - 1)) {
      this->leds[this->idx - 1] = getCurrentColor();
      this->leds[this->idx - 1].fadeToBlackBy(LedConstants::FADE_MULTIPLIER_WAVE);
    } else if (this->bounceDirection < 0 && isValidIndex(this->idx + 1)) {
      this->leds[this->idx + 1] = getCurrentColor();
      this->leds[this->idx + 1].fadeToBlackBy(LedConstants::FADE_MULTIPLIER_WAVE);
    }
  }

  this->idx += this->bounceDirection;

  if (this->idx >= this->numleds - 1 || this->idx <= 0) {
    this->bounceDirection = -this->bounceDirection;
  }
}

void TopLeds::fillFromCenter()
{
  if (!validatePointers()) return;

  safeFillSolid(CRGB::Black);

  int center = this->numleds / 2;

  // Fill from center outward
  for(int i = 0; i <= this->idx; i++) {
    if (isValidIndex(center + i)) {
      this->leds[center + i] = getCurrentColor();
    }
    if (isValidIndex(center - i)) {
      this->leds[center - i] = getCurrentColor();
    }
  }

  this->idx++;
  if (this->idx > center) {
    this->idx = 0;
  }
}

void TopLeds::knightRider()
{
  if (!validatePointers()) return;

  fadeToBlackBy(this->leds, this->numleds, LedConstants::SCANNER_FADE_AMOUNT);

  // Draw the scanner
  for(int i = 0; i < LedConstants::SCANNER_WIDTH; i++) {
    int pos = this->idx + i - (LedConstants::SCANNER_WIDTH / 2);
    if (isValidIndex(pos)) {
      this->leds[pos] = getCurrentColor();

      // Center is brightest
      if (i != LedConstants::SCANNER_WIDTH / 2) {
        this->leds[pos].fadeToBlackBy(LedConstants::FADE_AMOUNT_VERY_HEAVY);
      }
    }
  }

  this->idx += this->bounceDirection;

  if (this->idx >= this->numleds - 1 || this->idx <= 0) {
    this->bounceDirection = -this->bounceDirection;
  }
}
