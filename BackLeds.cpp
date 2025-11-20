#include "BackLeds.h"

BackLeds::BackLeds(CRGB *leds, int numleds)
{
  // Initialize base class members
  this->leds = leds;
  this->numleds = numleds;
  this->lastUpdate = millis();
  this->effectChangeTime = millis();
  this->speed = LedConstants::DEFAULT_SPEED_BACK;
  this->idx = 0;
  this->pulse_speed = LedConstants::PULSE_SPEED_DEFAULT;
  this->pulse = LedConstants::PULSE_VALUE_DEFAULT;
  this->pulse_offset = LedConstants::PULSE_OFFSET_DEFAULT;
  this->strobe_ind = false;
  this->currentEffect = 0;
  this->currentColor = 0; // Red
  this->autoChange = false;
  this->alternatePhase = false;

  // Validation
  if (!validatePointers()) {
    #ifdef DEBUG_MODE
      Serial.println(F("ERROR: BackLeds - Invalid LED pointer"));
    #endif
  }

  if (!validateNumLeds()) {
    #ifdef DEBUG_MODE
      Serial.print(F("WARNING: BackLeds - NumLEDs out of range: "));
      Serial.println(numleds);
    #endif
  }
}

void BackLeds::setEffect(int effect) {
  this->currentEffect = effect;
  this->idx = 0;
  this->pulse = LedConstants::PULSE_VALUE_DEFAULT;
  this->pulse_offset = LedConstants::PULSE_OFFSET_DEFAULT;
  this->strobe_ind = false;
  this->alternatePhase = false;

  if (effect == 99) {
    this->autoChange = true;
    this->currentEffect = 0;
  } else {
    this->autoChange = false;
  }
}

void BackLeds::update(unsigned long currentTime)
{
  if (!validatePointers()) return;

  if ((currentTime - this->lastUpdate) < this->speed) return;

  // Auto-change mode
  if (this->autoChange && (currentTime - this->effectChangeTime) > LedConstants::AUTO_CHANGE_INTERVAL_BACK) {
    this->currentEffect++;
    this->idx = 0;
    this->alternatePhase = false;
    this->effectChangeTime = currentTime;

    if (this->currentEffect >= LedConstants::MAX_EFFECT_BACK) {
      this->currentEffect = 1; // Skip 0 (off) in auto mode
    }
  }

  switch(this->currentEffect) {
    case 0: // Off
      safeFillSolid(CRGB::Black);
      break;

    case 1: // Random Red/Blue (original)
      this->randomB();
      break;

    case 2: // Random with selected color
      this->randomColored();
      break;

    case 3: // All on with selected color
      this->allOn();
      break;

    case 4: // Alternate between two colors
      this->alternateColors();
      break;

    case 5: // Sparkle effect
      commonSparkle(LedConstants::RANDOM_THRESHOLD_MEDIUM);
      break;
  }

  this->lastUpdate = currentTime;
}

void BackLeds::randomB() {
  if (!validatePointers()) return;

  safeFillSolid(CRGB::Black);

  for(int i = 0; i < this->numleds; i++) {
    if (isValidIndex(i)) {
      int nr = random(0, 2);
      if (nr) {
        this->leds[i] = CRGB::Red;
      }
      else {
        this->leds[i] = CRGB::Blue;
      }
    }
  }
}

void BackLeds::randomColored() {
  if (!validatePointers()) return;

  for(int i = 0; i < this->numleds; i++) {
    if (isValidIndex(i)) {
      if (random(0, 2)) {
        this->leds[i] = getCurrentColor();
      } else {
        this->leds[i] = CRGB::Black;
      }
    }
  }
}

void BackLeds::allOn() {
  safeFillSolid(getCurrentColor());
}

void BackLeds::alternateColors() {
  if (!validatePointers()) return;

  for(int i = 0; i < this->numleds; i++) {
    if (isValidIndex(i)) {
      if ((i % 2) == this->alternatePhase) {
        this->leds[i] = getCurrentColor();
      } else {
        this->leds[i] = colorMap[(this->currentColor + 1) % LedConstants::COLOR_WRAP_MODULO];
      }
    }
  }
  this->alternatePhase = !this->alternatePhase;
}
