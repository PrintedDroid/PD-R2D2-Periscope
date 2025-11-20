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

  // Additional check for LED pair configuration (V2 only)
  #ifdef BOTTOM_LED_V2
    if (this->numleds % 2 != 0) {
      #ifdef DEBUG_MODE
        Serial.println(F("WARNING: BottomLeds - NumLEDs should be even (pairs) for V2 hardware"));
      #endif
    }
    // Debug: Print configuration
    Serial.print(F("BottomLeds V2: numleds="));
    Serial.print(numleds);
    Serial.print(F(", logical positions="));
    Serial.println(getLogicalCount());
  #else
    Serial.print(F("BottomLeds V1: numleds="));
    Serial.println(numleds);
  #endif
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

// ============================================
// Hardware Abstraction Helper Functions
// ============================================

void BottomLeds::setPosition(int position, CRGB color) {
  if (!validatePointers()) return;

  #ifdef BOTTOM_LED_V2
    // V2: 12 LEDs in pairs - set both LEDs of the pair
    int led1 = position * 2;
    int led2 = position * 2 + 1;

    if (isValidIndex(led1)) {
      this->leds[led1] = color;
    }
    if (isValidIndex(led2)) {
      this->leds[led2] = color;
    }
    // Debug output (can be removed later)
    /*
    Serial.print("setPos ");
    Serial.print(position);
    Serial.print(" -> LEDs ");
    Serial.print(led1);
    Serial.print(",");
    Serial.println(led2);
    */
  #else
    // V1: 8 individual LEDs - set single LED
    if (isValidIndex(position)) {
      this->leds[position] = color;
    }
  #endif
}

int BottomLeds::getLogicalCount() const {
  #ifdef BOTTOM_LED_V2
    return this->numleds / 2;  // V2: 12 LEDs = 6 logical positions
  #else
    return this->numleds;      // V1: 8 LEDs = 8 logical positions
  #endif
}

// ============================================
// Effect Implementations
// ============================================

void BottomLeds::update(unsigned long currentTime)
{
  if (!validatePointers()) return;

  if ((currentTime - this->lastUpdate) < this->speed) return;

  // Auto-change mode - cycles through effects
  if (this->autoChange && (currentTime - this->effectChangeTime) > LedConstants::AUTO_CHANGE_INTERVAL_MAIN) {
    this->currentEffect++;
    this->speed = LedConstants::DEFAULT_SPEED_BOTTOM;
    this->idx = 0;
    this->pulse_speed = LedConstants::PULSE_SPEED_DEFAULT;
    this->pulse = LedConstants::PULSE_VALUE_DEFAULT;
    this->pulse_offset = LedConstants::PULSE_OFFSET_DEFAULT;
    this->strobe_ind = false;
    this->alternateRowsPhase = true;
    this->effectChangeTime = currentTime;

    if (this->currentEffect >= LedConstants::MAX_EFFECT_BOTTOM) {
      this->currentEffect = 1; // Skip 0 (off) in auto mode
    }
  }

  switch(this->currentEffect) {
    case 0: // Off
      safeFillSolid(CRGB::Black);
      break;
    case 1: // Superscan (ORIGINAL Effect 1)
      this->superscan();
      break;
    case 2: // Scan (ORIGINAL Effect 2)
      this->scan();
      break;
    case 3: // Simple (ORIGINAL Effect 3)
      this->simple();
      break;
    case 4: // Random
      this->randomLight();
      break;
    case 5: // Chase (ORIGINAL Effect 5)
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

  // Clear all LEDs
  fill_solid(this->leds, this->numleds, CRGB::Black);

  // Light one pair at a time
  int logicalPos = this->idx % getLogicalCount();
  setPosition(logicalPos, getCurrentColor());

  this->idx++;
  if (this->idx >= getLogicalCount()) {
    this->idx = 0;
  }
}

void BottomLeds::scan()
{
  if (!validatePointers()) return;

  // Clear all LEDs
  fill_solid(this->leds, this->numleds, CRGB::Black);

  // Show 2 adjacent pairs (like original showed 2 adjacent LEDs)
  int logicalCount = getLogicalCount();
  if (this->idx < logicalCount - 1) {
    setPosition(this->idx, getCurrentColor());
    setPosition(this->idx + 1, getCurrentColor());
  }

  this->idx += 2;
  if (this->idx >= logicalCount) {
    this->idx = 0;
  }
}

void BottomLeds::superscan()
{
  if (!validatePointers()) return;

  // Clear all LEDs
  fill_solid(this->leds, this->numleds, CRGB::Black);
  CRGB color = getCurrentColor();

  // Original pattern adapted for 6 logical positions (pairs)
  // Original for 8 LEDs: 0,1,6,7 then 2,3,4,5 then variations
  // For 6 pairs: outer pairs, inner pairs, variations
  switch (this->idx) {
    case 0: // Outer pairs (0 & 5)
      setPosition(0, color);
      setPosition(5, color);
      break;
    case 1: // Inner 4 pairs (1,2,3,4)
      setPosition(1, color);
      setPosition(2, color);
      setPosition(3, color);
      setPosition(4, color);
      break;
    case 2: // Middle pairs (2 & 3)
      setPosition(2, color);
      setPosition(3, color);
      break;
    case 3: // Next to middle (1 & 4)
      setPosition(1, color);
      setPosition(4, color);
      break;
    case 4: // Middle again
      setPosition(2, color);
      setPosition(3, color);
      break;
    case 5: // Inner 4 again
      setPosition(1, color);
      setPosition(2, color);
      setPosition(3, color);
      setPosition(4, color);
      break;
  }

  this->idx++;
  if (this->idx >= 6) {
    this->idx = 0;
  }
}

void BottomLeds::randomLight()
{
  if (!validatePointers()) return;

  fadeToBlackBy(this->leds, this->numleds, LedConstants::FADE_AMOUNT_STANDARD);

  if (random8() < LedConstants::RANDOM_THRESHOLD_MEDIUM) {
    int randomPos = random8(getLogicalCount());
    setPosition(randomPos, getCurrentColor());
  }
}

void BottomLeds::chase()
{
  if (!validatePointers()) return;

  fadeToBlackBy(this->leds, this->numleds, LedConstants::FADE_AMOUNT_STANDARD);

  uint8_t beatsinFreq = LedConstants::BEATSIN_FREQUENCY_BREATHE;
  uint8_t brightness = beatsin8(beatsinFreq, LedConstants::PULSE_VALUE_MIN, LedConstants::PULSE_VALUE_MAX);

  CRGB color = getCurrentColor();
  color.nscale8(brightness);

  int logicalPos = this->idx % getLogicalCount();
  setPosition(logicalPos, color);

  this->idx++;
  if (this->idx >= getLogicalCount()) {
    this->idx = 0;
  }
}

void BottomLeds::comet()
{
  if (!validatePointers()) return;

  // Fade all LEDs
  fadeToBlackBy(this->leds, this->numleds, LedConstants::FADE_MULTIPLIER_THEATER);

  // Bright head
  int logicalPos = this->idx % getLogicalCount();
  setPosition(logicalPos, getCurrentColor());

  // Dimmer tail
  int tailPos = (logicalPos - 1 + getLogicalCount()) % getLogicalCount();
  CRGB dimColor = getCurrentColor();
  dimColor.nscale8(LedConstants::FADE_MULTIPLIER_THEATER);
  setPosition(tailPos, dimColor);

  this->idx++;
  if (this->idx >= getLogicalCount()) {
    this->idx = 0;
  }
}

void BottomLeds::wave()
{
  if (!validatePointers()) return;

  // Create wave pattern across all 6 logical positions
  int logicalCount = getLogicalCount();

  for (int i = 0; i < logicalCount; i++) {
    uint8_t brightness = beatsin8(
      LedConstants::BEATSIN_FREQUENCY_BREATHE,
      LedConstants::PULSE_VALUE_MIN,
      LedConstants::PULSE_VALUE_MAX,
      0,
      (i * 255) / logicalCount  // Phase offset for wave effect
    );

    CRGB color = getCurrentColor();
    color.nscale8(brightness);
    setPosition(i, color);
  }
}

void BottomLeds::alternateRows()
{
  if (!validatePointers()) return;

  fill_solid(this->leds, this->numleds, CRGB::Black);
  CRGB color = getCurrentColor();

  // Alternate between two groups of 3 pairs each
  if (this->alternateRowsPhase) {
    // Positions 0, 1, 2
    setPosition(0, color);
    setPosition(1, color);
    setPosition(2, color);
  } else {
    // Positions 3, 4, 5
    setPosition(3, color);
    setPosition(4, color);
    setPosition(5, color);
  }

  this->alternateRowsPhase = !this->alternateRowsPhase;
}

void BottomLeds::snake()
{
  if (!validatePointers()) return;

  // Fade trail
  fadeToBlackBy(this->leds, this->numleds, LedConstants::FADE_AMOUNT_VERY_HEAVY);

  // Snake of 3 pairs
  CRGB color = getCurrentColor();
  int logicalCount = getLogicalCount();

  for (int i = 0; i < LedConstants::SNAKE_LENGTH; i++) {
    int pos = (this->idx - i + logicalCount) % logicalCount;
    CRGB fadeColor = color;
    fadeColor.nscale8(255 - (i * 85)); // Fade tail: 255, 170, 85
    setPosition(pos, fadeColor);
  }

  this->idx++;
  if (this->idx >= logicalCount) {
    this->idx = 0;
  }
}

// ============================================
// Override Common Effects for Hardware Abstraction
// ============================================

void BottomLeds::commonPulseAll() {
  if (!validatePointers()) return;

  CHSV hsv = rgb2hsv_approximate(getCurrentColor());
  hsv.v = this->pulse;

  // Use hardware abstraction to set all logical positions
  int logicalCount = getLogicalCount();
  for(int i = 0; i < logicalCount; i++) {
    CRGB color = hsv;
    setPosition(i, color);
  }

  this->pulse += this->pulse_offset;
  if (this->pulse >= LedConstants::PULSE_VALUE_MAX || this->pulse <= LedConstants::PULSE_VALUE_MIN) {
    this->pulse_offset = -(this->pulse_offset);
  }
}

void BottomLeds::commonStrobe() {
  if (!validatePointers()) return;

  CRGB color = this->strobe_ind ? getCurrentColor() : CRGB::Black;

  // Use hardware abstraction to set all logical positions
  int logicalCount = getLogicalCount();
  for(int i = 0; i < logicalCount; i++) {
    setPosition(i, color);
  }

  this->strobe_ind = !this->strobe_ind;
}

void BottomLeds::commonSparkle(uint8_t threshold) {
  if (!validatePointers()) return;

  fadeToBlackBy(this->leds, this->numleds, LedConstants::FADE_AMOUNT_LIGHT);

  if (random8() < threshold) {
    int logicalPos = random8(getLogicalCount());
    setPosition(logicalPos, getCurrentColor());
  }
}

void BottomLeds::safeFillSolid(CRGB color) {
  if (!validatePointers()) return;

  // Use hardware abstraction to fill all logical positions
  int logicalCount = getLogicalCount();
  for(int i = 0; i < logicalCount; i++) {
    setPosition(i, color);
  }
}
