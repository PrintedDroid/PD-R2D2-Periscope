#include "MainLeds.h"

MainLeds::MainLeds(CRGB *leds, int numleds)
{
  // Initialize base class members
  this->leds = leds;
  this->numleds = numleds;
  this->lastUpdate = millis();
  this->effectChangeTime = millis();
  this->speed = LedConstants::DEFAULT_SPEED_MAIN;
  this->idx = 0;
  this->pulse_speed = LedConstants::PULSE_SPEED_DEFAULT;
  this->pulse = LedConstants::PULSE_VALUE_DEFAULT;
  this->pulse_offset = LedConstants::PULSE_OFFSET_DEFAULT;
  this->strobe_ind = false;
  this->theater_chase_q = 0;
  this->currentEffect = 0;
  this->currentColor = LedConstants::DEFAULT_COLOR_WHITE;
  this->autoChange = false;

  // Initialize fire heat array
  for(int i = 0; i < LedConstants::MAX_LEDS_PER_STRIP; i++) {
    this->heat[i] = 0;
  }

  // Validation
  if (!validatePointers()) {
    #ifdef DEBUG_MODE
      Serial.println(F("ERROR: MainLeds - Invalid LED pointer"));
    #endif
  }

  if (!validateNumLeds()) {
    #ifdef DEBUG_MODE
      Serial.print(F("WARNING: MainLeds - NumLEDs out of range: "));
      Serial.println(numleds);
    #endif
  }
}

void MainLeds::setEffect(int effect) {
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

void MainLeds::update(unsigned long currentTime)
{
  if (!validatePointers()) return;

  // Handle center LED pulsing only if not in OFF state
  if (this->currentEffect != 0 && (currentTime - this->lastUpdate) > this->pulse_speed) {
    this->pulseCenter();
  }

  if ((currentTime - this->lastUpdate) < this->speed) return;

  // Auto-change mode
  if (this->autoChange && (currentTime - this->effectChangeTime) > LedConstants::AUTO_CHANGE_INTERVAL_MAIN) {
    this->currentEffect++;
    this->speed = LedConstants::DEFAULT_SPEED_MAIN;
    this->idx = 0;
    this->pulse_speed = LedConstants::PULSE_SPEED_DEFAULT;
    this->pulse = LedConstants::PULSE_VALUE_DEFAULT;
    this->pulse_offset = LedConstants::PULSE_OFFSET_DEFAULT;
    this->strobe_ind = false;
    this->effectChangeTime = currentTime;

    if (this->currentEffect >= LedConstants::MAX_EFFECT_MAIN) {
      this->currentEffect = 1; // Skip 0 (off) in auto mode
    }
  }

  switch(this->currentEffect) {
    case 0: // Off
      safeFillSolid(CRGB::Black);
      break;
    case 1: // Pulse all
      commonPulseAll();
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
    case 5: // CW run 4
      this->cw_run(4);
      break;
    case 6: // CW split 2
      this->cw_split2();
      break;
    case 7: // CW split 3
      this->cw_split3();
      break;
    case 8: // CW split 4
      this->cw_split4();
      break;
    case 9: // Strobe
      commonStrobe();
      break;
    case 10: // Smooth pulse
      this->smoothPulse();
      break;
    case 11: // Theater chase
      this->theaterChase();
      break;
    case 12: // Rainbow
      commonRainbow();
      break;
    case 13: // Fire
      commonFire(this->heat, LedConstants::MAX_LEDS_PER_STRIP);
      break;
    case 14: // Circle Chase
      this->circleChase();
      break;
    case 15: // Center Expand
      this->centerExpand();
      break;
    case 16: // Spiral Out
      this->spiralOut();
      break;
    case 17: // Twinkle
      commonTwinkle();
      break;
    case 18: // Theater Chase (common)
      commonTheaterChase();
      break;
    case 19: // Bounce with Trail
      commonBounceWithTrail();
      break;
    case 20: // Color Gradient
      commonColorGradient();
      break;
  }

  this->lastUpdate = currentTime;
}

void MainLeds::pulseCenter()
{
  if (!isValidIndex(0)) return;

  CHSV hsv = rgb2hsv_approximate(getCurrentColor());
  hsv.v = this->pulse;
  this->leds[0] = hsv;

  this->pulse += this->pulse_offset;
  if (this->pulse >= LedConstants::PULSE_VALUE_MAX || this->pulse <= LedConstants::PULSE_VALUE_MIN) {
    this->pulse_offset = -(this->pulse_offset);
  }
}

void MainLeds::cw_run(int pt)
{
  fill_solid(this->leds + 1, this->numleds - 1, CRGB::Black);

  for(int x = 0; x < pt; x++) {
    int y = (this->idx + x) % LedConstants::MAIN_LED_CIRCLE_SIZE;
    if (y < this->numleds - 1) {
      this->leds[y + 1] = getCurrentColor();
    }
  }

  this->idx++;
  if (this->idx >= LedConstants::MAIN_LED_CIRCLE_SIZE) {
    this->idx = 0;
  }
}

void MainLeds::cw_split2()
{
  fill_solid(this->leds + 1, this->numleds - 1, CRGB::Black);

  int x = (this->idx + 4) % LedConstants::MAIN_LED_CIRCLE_SIZE;

  if (this->idx < this->numleds - 1) {
    this->leds[this->idx + 1] = getCurrentColor();
  }
  if (x < this->numleds - 1) {
    this->leds[x + 1] = getCurrentColor();
  }

  this->idx++;
  if (this->idx >= LedConstants::MAIN_LED_CIRCLE_SIZE) {
    this->idx = 0;
  }
}

void MainLeds::cw_split3()
{
  fill_solid(this->leds + 1, this->numleds - 1, CRGB::Black);

  // Corrected for more even spacing on an 8-LED circle
  int pos1 = this->idx % LedConstants::MAIN_LED_CIRCLE_SIZE;
  int pos2 = (this->idx + 3) % LedConstants::MAIN_LED_CIRCLE_SIZE; // Approx 120 degrees
  int pos3 = (this->idx + 6) % LedConstants::MAIN_LED_CIRCLE_SIZE; // Approx 240 degrees

  this->leds[pos1 + 1] = getCurrentColor();
  this->leds[pos2 + 1] = getCurrentColor();
  this->leds[pos3 + 1] = getCurrentColor();

  this->idx++;
  if (this->idx >= LedConstants::MAIN_LED_CIRCLE_SIZE) {
    this->idx = 0;
  }
}

void MainLeds::cw_split4()
{
  fill_solid(this->leds + 1, this->numleds - 1, CRGB::Black);

  int offset = LedConstants::MAIN_LED_CIRCLE_SIZE / 4;
  int x = (this->idx + offset) % LedConstants::MAIN_LED_CIRCLE_SIZE;
  int y = (this->idx + (offset * 2)) % LedConstants::MAIN_LED_CIRCLE_SIZE;
  int z = (this->idx + (offset * 3)) % LedConstants::MAIN_LED_CIRCLE_SIZE;

  if (this->idx < this->numleds - 1) {
    this->leds[this->idx + 1] = getCurrentColor();
  }
  if (x < this->numleds - 1) {
    this->leds[x + 1] = getCurrentColor();
  }
  if (y < this->numleds - 1) {
    this->leds[y + 1] = getCurrentColor();
  }
  if (z < this->numleds - 1) {
    this->leds[z + 1] = getCurrentColor();
  }

  this->idx++;
  if (this->idx >= LedConstants::MAIN_LED_CIRCLE_SIZE) {
    this->idx = 0;
  }
}

void MainLeds::smoothPulse()
{
  uint8_t beatsinDivisor = 60 / (this->speed / LedConstants::PULSE_SPEED_DEFAULT);
  uint8_t brightness = beatsin8(beatsinDivisor, LedConstants::PULSE_VALUE_MIN, LedConstants::PULSE_VALUE_MAX);
  CRGB color = getCurrentColor();

  for(int i = 0; i < this->numleds; i++) {
    this->leds[i] = color;
    this->leds[i].nscale8(brightness);
  }
}

void MainLeds::theaterChase()
{
  fadeToBlackBy(this->leds + 1, this->numleds - 1, LedConstants::FADE_AMOUNT_STANDARD);

  for(int i = 0; i < 3; i++) {
    int pos = (this->idx + i * 3) % (this->numleds - 1);
    if (pos < this->numleds - 1) {
      this->leds[pos + 1] = getCurrentColor();
    }
  }

  this->idx++;
  if (this->idx >= (this->numleds - 1)) {
    this->idx = 0;
  }
}

void MainLeds::circleChase()
{
  fadeToBlackBy(this->leds, this->numleds, LedConstants::FADE_AMOUNT_HEAVY);

  // Keep center pulsing
  CHSV hsv = rgb2hsv_approximate(getCurrentColor());
  hsv.v = this->pulse;
  if (isValidIndex(0)) {
    this->leds[0] = hsv;
  }

  // Chase around the outer ring
  if (this->idx < LedConstants::MAIN_LED_CIRCLE_SIZE) {
    int ledIndex = this->idx + 1;
    if (isValidIndex(ledIndex)) {
      this->leds[ledIndex] = getCurrentColor();
    }
  }

  this->idx++;
  if (this->idx >= LedConstants::MAIN_LED_CIRCLE_SIZE) {
    this->idx = 0;
  }
}

void MainLeds::centerExpand()
{
  safeFillSolid(CRGB::Black);

  switch(this->idx) {
    case 0: // Center only
      if (isValidIndex(0)) {
        this->leds[0] = getCurrentColor();
      }
      break;
    case 1: // Center + cross
      if (isValidIndex(0)) this->leds[0] = getCurrentColor();
      if (isValidIndex(LedConstants::INDEX_TWO)) this->leds[LedConstants::INDEX_TWO] = getCurrentColor();
      if (isValidIndex(LedConstants::INDEX_FOUR)) this->leds[LedConstants::INDEX_FOUR] = getCurrentColor();
      if (isValidIndex(LedConstants::INDEX_SIX)) this->leds[LedConstants::INDEX_SIX] = getCurrentColor();
      if (isValidIndex(8)) this->leds[8] = getCurrentColor();
      break;
    case 2: // Center + corners
      if (isValidIndex(0)) this->leds[0] = getCurrentColor();
      if (isValidIndex(1)) this->leds[1] = getCurrentColor();
      if (isValidIndex(LedConstants::INDEX_THREE)) this->leds[LedConstants::INDEX_THREE] = getCurrentColor();
      if (isValidIndex(5)) this->leds[5] = getCurrentColor();
      if (isValidIndex(7)) this->leds[7] = getCurrentColor();
      break;
    case 3: // All on
      safeFillSolid(getCurrentColor());
      break;
  }

  this->idx++;
  if (this->idx >= 4) {
    this->idx = 0;
  }
}

void MainLeds::spiralOut()
{
  fadeToBlackBy(this->leds, this->numleds, LedConstants::FADE_AMOUNT_MEDIUM);

  // Spiral pattern from center outward
  int sequence[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

  if (this->idx < 9 && isValidIndex(sequence[this->idx])) {
    this->leds[sequence[this->idx]] = getCurrentColor();
  }

  this->idx++;
  if (this->idx >= 12) { // Pause at end
    this->idx = 0;
  }
}
