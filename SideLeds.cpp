#include "SideLeds.h"

SideLeds::SideLeds(CRGB *leds, int numleds)
{
  this->lastUpdate = millis();
  this->leds = leds;
  this->numleds = numleds;
  this->speed = 200;
  this->idx = 0;

  this->pulse_offset = 10;
  this->strobe_ind = false;

  this->currentEffect = 0;
  this->currentColor = 0; // Red
  this->effect_time = millis();
  this->autoChange = false;
  
  // Initialize fire heat array
  for(int i = 0; i < this->numleds; i++) {
    this->heat[i] = 0;
  }
}

void SideLeds::setEffect(int effect) {
  this->currentEffect = effect;
  this->idx = 0;
  this->pulse_offset = 10;
  this->strobe_ind = false;
  
  if (effect == 99) {
    this->autoChange = true;
    this->currentEffect = 0;
  } else {
    this->autoChange = false;
  }
}

void SideLeds::setColor(int color) {
  if (color >= 0 && color <= 9) {
    this->currentColor = color;
  }
}

void SideLeds::setSpeed(int speed) {
  this->speed = speed;
}

void SideLeds::update(unsigned long currentTime)
{
  if ((currentTime - this->lastUpdate) < this->speed) return;

  // Auto-change mode
  if (this->autoChange && (currentTime - this->effect_time) > 5000) {
    this->currentEffect++;
    this->speed = 200;
    this->idx = 0;
    this->pulse_offset = 10;
    this->strobe_ind = false;
    this->effect_time = currentTime;
    
    if (this->currentEffect >= 8) {
      this->currentEffect = 1; // Skip 0 (off) in auto mode
    }
  }

  switch(this->currentEffect) {
    case 0: // Off
      fill_solid(this->leds, this->numleds, CRGB::Black);
      break;
    case 1: // Pulse
      this->pulse();
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
      this->strobe();
      break;
    case 6: // Breathe
      this->breathe();
      break;
    case 7: // Fire
      this->fire();
      break;
    case 8: // Sparkle
      this->sparkle();
      break;
    case 9: // Rainbow
      this->rainbow();
      break;
  }
  
  this->lastUpdate = currentTime;
}

void SideLeds::pulse()
{
  // Corrected to pulse the current color, not just white
  CHSV hsv = rgb2hsv_approximate(colorMap[this->currentColor]);
  hsv.v = this->idx; // Use idx to control the brightness (Value)

  for(int i = 0; i < this->numleds; i++) {
    this->leds[i] = hsv;
  }
  
  this->idx += this->pulse_offset;
  if (this->idx >= 255 || this->idx <= 0) {
    this->pulse_offset = -this->pulse_offset;
    this->idx += this->pulse_offset; // Ensure it doesn't get stuck at bounds
  }
}

void SideLeds::strobe()
{
  fill_solid(this->leds, this->numleds, CRGB::Black);
  
  if (this->strobe_ind) {
    fill_solid(this->leds, this->numleds, colorMap[this->currentColor]);
  }
  
  this->strobe_ind = !this->strobe_ind;
}

void SideLeds::cw_run(int pt)
{
  fill_solid(this->leds, this->numleds, CRGB::Black);
  
  // Always keep center LED on with current color
  this->leds[0] = colorMap[this->currentColor];
  
  for(int x = 0; x < pt; x++) {
    int y = (this->idx + x) % 8;
    // Add 1 to skip center LED (index 0)
    this->leds[y + 1] = colorMap[this->currentColor];
  }
  
  this->idx++;
  if (this->idx >= 8) {
    this->idx = 0;
  }
}

void SideLeds::breathe()
{
  uint8_t breath = beatsin8(12, 0, 255);
  CRGB color = colorMap[this->currentColor];
  
  for(int i = 0; i < this->numleds; i++) {
    this->leds[i] = color;
    this->leds[i].nscale8(breath);
  }
}

void SideLeds::fire()
{
  // Cool down every cell a little
  for(int i = 0; i < this->numleds; i++) {
    this->heat[i] = qsub8(this->heat[i], random8(0, ((55 * 10) / this->numleds) + 2));
  }

  // Heat from each cell drifts up and diffuses slightly
  for(int k = this->numleds - 1; k >= 2; k--) {
    this->heat[k] = (this->heat[k - 1] + this->heat[k - 2] + this->heat[k - 2]) / 3;
  }
  
  // Randomly ignite new sparks near bottom
  if(random8() < 120) {
    int y = random8(7);
    this->heat[y] = qadd8(this->heat[y], random8(160, 255));
  }

  // Map from heat cells to LED colors
  for(int j = 0; j < this->numleds; j++) {
    this->leds[j] = HeatColor(this->heat[j]);
  }
}

void SideLeds::sparkle()
{
  fadeToBlackBy(this->leds, this->numleds, 10);
  
  if (random8() < 120) {
    int pos = random(this->numleds);
    this->leds[pos] = colorMap[this->currentColor];
  }
}

void SideLeds::rainbow()
{
  static uint8_t hue = 0;
  fill_rainbow(this->leds, this->numleds, hue, 255 / this->numleds);
  hue += map(this->speed, 50, 400, 10, 1);
}