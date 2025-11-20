#include "SideLeds.h"

SideLeds::SideLeds(CRGB *leds, int numleds)
{
  // Initialize base class members
  this->leds = leds;
  this->numleds = numleds;
  this->lastUpdate = millis();
  this->effectChangeTime = millis();
  this->speed = LedConstants::DEFAULT_SPEED_SIDE;
  this->idx = 0;
  this->pulse_speed = LedConstants::PULSE_SPEED_DEFAULT;
  this->pulse = LedConstants::PULSE_VALUE_DEFAULT;
  this->pulse_offset = LedConstants::PULSE_OFFSET_DEFAULT;
  this->strobe_ind = false;
  this->currentEffect = 0;
  this->currentColor = 0; // Red
  this->autoChange = false;

  // Initialize fire heat array
  for(int i = 0; i < LedConstants::MAX_LEDS_PER_STRIP; i++) {
    this->heat[i] = 0;
  }

  // Validation
  if (!validatePointers()) {
    #ifdef DEBUG_MODE
      Serial.println(F("ERROR: SideLeds - Invalid LED pointer"));
    #endif
  }

  if (!validateNumLeds()) {
    #ifdef DEBUG_MODE
      Serial.print(F("WARNING: SideLeds - NumLEDs out of range: "));
      Serial.println(numleds);
    #endif
  }
}

void SideLeds::setEffect(int effect) {
  this->currentEffect = effect;
  this->idx = 0;
  this->pulse = LedConstants::PULSE_VALUE_DEFAULT;
  this->pulse_offset = LedConstants::PULSE_OFFSET_DEFAULT;
  this->strobe_ind = false;

  if (effect == 99) {
    this->autoChange = true;
    this->currentEffect = 0;
  } else {
    this->autoChange = false;
  }
}

void SideLeds::update(unsigned long currentTime)
{
  if (!validatePointers()) return;

  if ((currentTime - this->lastUpdate) < this->speed) return;

  // Auto-change mode
  if (this->autoChange && (currentTime - this->effectChangeTime) > LedConstants::AUTO_CHANGE_INTERVAL_SIDE) {
    this->currentEffect++;
    this->speed = LedConstants::DEFAULT_SPEED_SIDE;
    this->idx = 0;
    this->pulse = LedConstants::PULSE_VALUE_DEFAULT;
    this->pulse_offset = LedConstants::PULSE_OFFSET_DEFAULT;
    this->strobe_ind = false;
    this->effectChangeTime = currentTime;

    if (this->currentEffect >= LedConstants::MAX_EFFECT_SIDE) {
      this->currentEffect = 1; // Skip 0 (off) in auto mode
    }
  }

  switch(this->currentEffect) {
    case 0: // Off
      safeFillSolid(CRGB::Black);
      break;
    case 1: // Pulse
      this->pulseEffect();
      break;
    case 2: // CW run 1
      this->cw_run(1);
      break;
    case 3: // CW run 2
      this->cw_run(2);
      break;
    case 4: // CW run 3
      this->cw_run(3);
      break;
    case 5: // Strobe
      commonStrobe();
      break;
    case 6: // Breathe
      this->breathe();
      break;
    case 7: // Fire
      commonFire(this->heat, LedConstants::MAX_LEDS_PER_STRIP);
      break;
    case 8: // Sparkle
      commonSparkle();
      break;
    case 9: // Rainbow
      commonRainbow();
      break;
  }

  this->lastUpdate = currentTime;
}

void SideLeds::pulseEffect()
{
  if (!validatePointers()) return;

  CHSV hsv = rgb2hsv_approximate(getCurrentColor());
  hsv.v = this->idx;

  for(int i = 0; i < this->numleds; i++) {
    this->leds[i] = hsv;
  }

  this->idx += this->pulse_offset;
  if (this->idx >= LedConstants::PULSE_VALUE_MAX || this->idx <= LedConstants::PULSE_VALUE_MIN) {
    this->pulse_offset = -this->pulse_offset;
    this->idx += this->pulse_offset;
  }
}

void SideLeds::cw_run(int pt)
{
  if (!validatePointers()) return;

  safeFillSolid(CRGB::Black);

  // Always keep center LED on with current color (last LED = center)
  int centerLed = this->numleds - 1;
  if (isValidIndex(centerLed)) {
    this->leds[centerLed] = getCurrentColor();
  }

  // Running LEDs in circle (LEDs 0 to numleds-2)
  for(int x = 0; x < pt; x++) {
    int y = (this->idx + x) % LedConstants::MAIN_LED_CIRCLE_SIZE;
    if (isValidIndex(y)) {
      this->leds[y] = getCurrentColor();
    }
  }

  this->idx++;
  if (this->idx >= LedConstants::MAIN_LED_CIRCLE_SIZE) {
    this->idx = 0;
  }
}

void SideLeds::breathe()
{
  if (!validatePointers()) return;

  uint8_t breath = beatsin8(LedConstants::BEATSIN_FREQUENCY_BREATHE,
                             LedConstants::PULSE_VALUE_MIN,
                             LedConstants::PULSE_VALUE_MAX);
  CRGB color = getCurrentColor();

  for(int i = 0; i < this->numleds; i++) {
    this->leds[i] = color;
    this->leds[i].nscale8(breath);
  }
}
