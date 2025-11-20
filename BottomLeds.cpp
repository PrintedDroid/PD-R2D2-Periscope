#include "BottomLeds.h"

BottomLeds::BottomLeds(CRGB *leds, int numleds)
{
  // Initialize base class members
  this->leds = leds;
  this->numleds = numleds;
  this->lastUpdate = millis();
  this->effectChangeTime = millis();
  this->speed = LedConstants::DEFAULT_SPEED_BOTTOM;
  this->idx = 0;
  this->pulse_speed = LedConstants::PULSE_SPEED_DEFAULT;
  this->pulse = LedConstants::PULSE_VALUE_DEFAULT;
  this->pulse_offset = LedConstants::PULSE_OFFSET_DEFAULT;
  this->strobe_ind = false;
  this->currentEffect = 0;
  this->currentColor = 0; // Red
  this->autoChange = false;
  this->alternateRowsPhase = true;

  // Validation
  if (!validatePointers()) {
    #ifdef DEBUG_MODE
      Serial.println(F("ERROR: BottomLeds - Invalid LED pointer"));
    #endif
  }

  if (!validateNumLeds()) {
    #ifdef DEBUG_MODE
      Serial.print(F("WARNING: BottomLeds - NumLEDs out of range: "));
      Serial.println(numleds);
    #endif
  }
}

void BottomLeds::setEffect(int effect) {
  this->currentEffect = effect;
  this->idx = 0;
  this->pulse = LedConstants::PULSE_VALUE_DEFAULT;
  this->pulse_offset = LedConstants::PULSE_OFFSET_DEFAULT;
  this->strobe_ind = false;
  this->alternateRowsPhase = true;

  if (effect == 99) {
    this->autoChange = true;
    this->currentEffect = 0;
  } else {
    this->autoChange = false;
  }
}

void BottomLeds::update(unsigned long currentTime)
{
  if (!validatePointers()) return;

  if ((currentTime - this->lastUpdate) < this->speed) return;

  // Auto-change mode
  if (this->autoChange && (currentTime - this->effectChangeTime) > LedConstants::AUTO_CHANGE_INTERVAL_MAIN) {
    this->currentEffect++;
    this->idx = 0;
    this->alternateRowsPhase = true;
    this->effectChangeTime = currentTime;

    if (this->currentEffect >= LedConstants::MAX_EFFECT_BOTTOM) {
      this->currentEffect = 1; // Skip 0 (off) in auto mode
    }
  }

  switch (this->currentEffect) {
    case 0: // Off
      safeFillSolid(CRGB::Black);
      break;
    case 1: // Superscan
      this->superscan();
      break;
    case 2: // Scan
      this->scan();
      break;
    case 3: // Simple
      this->simple();
      break;
    case 4: // Random
      this->randomLight();
      break;
    case 5: // Chase
      this->chase();
      break;
    case 6: // Comet
      this->comet();
      break;
    case 7: // Wave
      this->wave();
      break;
    case 8: // Alternate rows
      this->alternateRows();
      break;
    case 9: // Snake
      this->snake();
      break;
  }

  this->lastUpdate = currentTime;
}

void BottomLeds::simple()
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

void BottomLeds::scan()
{
  if (!validatePointers()) return;

  safeFillSolid(CRGB::Black);

  if (isValidIndex(this->idx) && isValidIndex(this->idx + 1)) {
    this->leds[this->idx] = getCurrentColor();
    this->leds[this->idx + 1] = getCurrentColor();
  }

  this->idx += 2;

  if (this->idx >= this->numleds) {
    this->idx = 0;
  }
}

void BottomLeds::superscan()
{
  if (!validatePointers()) return;

  safeFillSolid(CRGB::Black);
  CRGB color = getCurrentColor();

  switch (this->idx) {
    case 0:
      if (isValidIndex(0)) this->leds[0] = color;
      if (isValidIndex(1)) this->leds[1] = color;
      if (isValidIndex(LedConstants::INDEX_SIX)) this->leds[LedConstants::INDEX_SIX] = color;
      if (isValidIndex(7)) this->leds[7] = color;
      break;
    case 1:
      if (isValidIndex(LedConstants::INDEX_TWO)) this->leds[LedConstants::INDEX_TWO] = color;
      if (isValidIndex(LedConstants::INDEX_THREE)) this->leds[LedConstants::INDEX_THREE] = color;
      if (isValidIndex(LedConstants::INDEX_FOUR)) this->leds[LedConstants::INDEX_FOUR] = color;
      if (isValidIndex(5)) this->leds[5] = color;
      break;
    case 2:
      if (isValidIndex(LedConstants::INDEX_THREE)) this->leds[LedConstants::INDEX_THREE] = color;
      if (isValidIndex(5)) this->leds[5] = color;
      break;
    case 3:
      if (isValidIndex(LedConstants::INDEX_TWO)) this->leds[LedConstants::INDEX_TWO] = color;
      if (isValidIndex(LedConstants::INDEX_FOUR)) this->leds[LedConstants::INDEX_FOUR] = color;
      break;
    case 4:
      if (isValidIndex(LedConstants::INDEX_THREE)) this->leds[LedConstants::INDEX_THREE] = color;
      if (isValidIndex(5)) this->leds[5] = color;
      break;
    case 5:
      if (isValidIndex(LedConstants::INDEX_TWO)) this->leds[LedConstants::INDEX_TWO] = color;
      if (isValidIndex(LedConstants::INDEX_THREE)) this->leds[LedConstants::INDEX_THREE] = color;
      if (isValidIndex(LedConstants::INDEX_FOUR)) this->leds[LedConstants::INDEX_FOUR] = color;
      if (isValidIndex(5)) this->leds[5] = color;
      break;
  }

  this->idx++;
  if (this->idx >= LedConstants::INDEX_SIX) {
    this->idx = 0;
  }
}

void BottomLeds::randomLight() {
  if (!validatePointers()) return;

  safeFillSolid(CRGB::Black);
  int ledIndex = random(this->numleds);
  if (isValidIndex(ledIndex)) {
    this->leds[ledIndex] = getCurrentColor();
  }
}

void BottomLeds::chase() {
  if (!validatePointers()) return;

  fadeToBlackBy(this->leds, this->numleds, LedConstants::FADE_AMOUNT_STANDARD);

  int pos = beatsin16(LedConstants::BEATSIN_FREQUENCY_BREATHE, 0, this->numleds - 1);
  if (isValidIndex(pos)) {
    this->leds[pos] = getCurrentColor();
  }
}

void BottomLeds::comet() {
  if (!validatePointers()) return;

  fadeToBlackBy(this->leds, this->numleds, LedConstants::FADE_AMOUNT_STANDARD);

  if (isValidIndex(this->idx)) {
    this->leds[this->idx] = getCurrentColor();

    // Add trail
    if (isValidIndex(this->idx - 1)) {
      this->leds[this->idx - 1] = getCurrentColor();
      this->leds[this->idx - 1].fadeToBlackBy(LedConstants::FADE_MULTIPLIER_THEATER);
    }
  }

  this->idx++;
  if (this->idx >= this->numleds) {
    this->idx = 0;
  }
}

void BottomLeds::wave() {
  if (!validatePointers()) return;

  safeFillSolid(CRGB::Black);

  // Wave effect for 2x4 arrangement
  switch(this->idx) {
    case 0: // Left column
      if (isValidIndex(0)) this->leds[0] = getCurrentColor();
      if (isValidIndex(7)) this->leds[7] = getCurrentColor();
      break;
    case 1:
      if (isValidIndex(1)) this->leds[1] = getCurrentColor();
      if (isValidIndex(LedConstants::INDEX_SIX)) this->leds[LedConstants::INDEX_SIX] = getCurrentColor();
      break;
    case 2:
      if (isValidIndex(LedConstants::INDEX_TWO)) this->leds[LedConstants::INDEX_TWO] = getCurrentColor();
      if (isValidIndex(5)) this->leds[5] = getCurrentColor();
      break;
    case 3: // Right column
      if (isValidIndex(LedConstants::INDEX_THREE)) this->leds[LedConstants::INDEX_THREE] = getCurrentColor();
      if (isValidIndex(LedConstants::INDEX_FOUR)) this->leds[LedConstants::INDEX_FOUR] = getCurrentColor();
      break;
  }

  this->idx++;
  if (this->idx >= LedConstants::WAVE_MAX_INDEX) {
    this->idx = 0;
  }
}

void BottomLeds::alternateRows() {
  if (!validatePointers()) return;

  safeFillSolid(CRGB::Black);

  if (this->alternateRowsPhase) {
    // Top row on
    for(int i = 0; i < 4; i++) {
      if (isValidIndex(i)) {
        this->leds[i] = getCurrentColor();
      }
    }
  } else {
    // Bottom row on
    for(int i = 4; i < 8; i++) {
      if (isValidIndex(i)) {
        this->leds[i] = getCurrentColor();
      }
    }
  }

  this->alternateRowsPhase = !this->alternateRowsPhase;
}

void BottomLeds::snake() {
  if (!validatePointers()) return;

  safeFillSolid(CRGB::Black);

  // Snake pattern through 2x4 grid
  // Path: 0->1->2->3->4->5->6->7->0
  for(int i = 0; i < LedConstants::SNAKE_LENGTH; i++) {
    int pos = (this->idx - i + this->numleds) % this->numleds;
    if (isValidIndex(pos)) {
      this->leds[pos] = getCurrentColor();

      // Fade tail
      if (i > 0) {
        this->leds[pos].fadeToBlackBy(i * LedConstants::FADE_AMOUNT_VERY_HEAVY);
      }
    }
  }

  this->idx++;
  if (this->idx >= this->numleds) {
    this->idx = 0;
  }
}
