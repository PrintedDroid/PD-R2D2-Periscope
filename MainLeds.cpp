#include "MainLeds.h"

MainLeds::MainLeds(CRGB *leds, int numleds)
{
  this->lastUpdate = millis();
  this->effectChangeTime = millis();
  this->leds = leds;
  this->numleds = numleds;
  this->speed = 100;
  
  this->idx = 0;

  this->pulse_speed = 20;
  this->pulse = 50;
  this->pulse_offset = 1;

  this->strobe_ind = false;

  this->currentEffect = 0;
  this->currentColor = 8; // White
  this->autoChange = false;
  
  // Initialize fire heat array
  for(int i = 0; i < this->numleds; i++) {
    this->heat[i] = 0;
  }
}

void MainLeds::setEffect(int effect) {
  this->currentEffect = effect;
  this->idx = 0;
  this->pulse = 50;
  this->pulse_offset = 1;
  this->strobe_ind = false;
  
  if (effect == 99) {
    this->autoChange = true;
    this->currentEffect = 0;
  } else {
    this->autoChange = false;
  }
}

void MainLeds::setColor(int color) {
  if (color >= 0 && color <= 9) {
    this->currentColor = color;
  }
}

void MainLeds::setSpeed(int speed) {
  this->speed = speed;
  this->pulse_speed = speed / 5;
  if (this->pulse_speed < 10) this->pulse_speed = 10;
}

void MainLeds::update(unsigned long currentTime)
{
  // Handle center LED pulsing only if not in OFF state
  if (this->currentEffect != 0 && (currentTime - this->lastUpdate) > this->pulse_speed) {
    this->pulseCenter();
  }
  
  if ((currentTime - this->lastUpdate) < this->speed) return;

  // Auto-change mode
  if (this->autoChange && (currentTime - this->effectChangeTime) > 10000) {
    this->currentEffect++;
    this->speed = 100;
    this->idx = 0;
    this->pulse_speed = 20;
    this->pulse = 50;
    this->pulse_offset = 1;  
    this->strobe_ind = false;
    this->effectChangeTime = currentTime;
    
    if (this->currentEffect >= 17) {
      this->currentEffect = 1; // Skip 0 (off) in auto mode
    }
  }

  switch(this->currentEffect) {
    case 0: // Off
      fill_solid(this->leds, this->numleds, CRGB::Black);
      break;
    case 1: // Pulse all
      this->pulseAll();
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
      this->strobe();
      break;
    case 10: // Smooth pulse
      this->smoothPulse();
      break;
    case 11: // Theater chase
      this->theaterChase();
      break;
    case 12: // Rainbow
      this->rainbow();
      break;
    case 13: // Fire
      this->fire();
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
  }
  
  this->lastUpdate = currentTime;
}

void MainLeds::pulseCenter()
{
  CHSV hsv = rgb2hsv_approximate(colorMap[this->currentColor]);
  hsv.v = this->pulse;
  *(this->leds) = hsv;
  
  this->pulse += this->pulse_offset;  
  if (this->pulse >= 255 || this->pulse <= 50) {
    this->pulse_offset = -(this->pulse_offset);
  }
}

void MainLeds::pulseAll()
{
  CHSV hsv = rgb2hsv_approximate(colorMap[this->currentColor]);
  hsv.v = this->pulse;
  
  for(int i = 0; i < this->numleds; i++) {
    this->leds[i] = hsv;
  }
  
  this->pulse += this->pulse_offset;  
  if (this->pulse >= 255 || this->pulse <= 50) {
    this->pulse_offset = -(this->pulse_offset);
  }
}

void MainLeds::cw_run(int pt)
{
  fill_solid(this->leds + 1, this->numleds - 1, CRGB::Black);

  for(int x = 0; x < pt; x++) {
    int y = (this->idx + x) % 8;
    if (y < this->numleds - 1) {
      this->leds[y + 1] = colorMap[this->currentColor];
    }
  }
  
  this->idx++;
  if (this->idx >= 8) {
    this->idx = 0;
  }
}

void MainLeds::cw_split2()
{
  fill_solid(this->leds + 1, this->numleds - 1, CRGB::Black);

  int x = (this->idx + 4) % 8;
  
  if (this->idx < this->numleds - 1) {
    this->leds[this->idx + 1] = colorMap[this->currentColor];
  }
  if (x < this->numleds - 1) {
    this->leds[x + 1] = colorMap[this->currentColor];
  }
  
  this->idx++;
  if (this->idx >= 8) {
    this->idx = 0;
  }
}

void MainLeds::cw_split3()
{
  fill_solid(this->leds + 1, this->numleds - 1, CRGB::Black);

  // Corrected for more even spacing on an 8-LED circle
  int pos1 = this->idx % 8;
  int pos2 = (this->idx + 3) % 8; // Approx 120 degrees
  int pos3 = (this->idx + 6) % 8; // Approx 240 degrees

  this->leds[pos1 + 1] = colorMap[this->currentColor];
  this->leds[pos2 + 1] = colorMap[this->currentColor];
  this->leds[pos3 + 1] = colorMap[this->currentColor];
  
  this->idx++;
  if (this->idx >= 8) {
    this->idx = 0;
  }
}

void MainLeds::cw_split4()
{
  fill_solid(this->leds + 1, this->numleds - 1, CRGB::Black);

  int offset = 8 / 4;
  int x = (this->idx + offset) % 8;
  int y = (this->idx + (offset * 2)) % 8;
  int z = (this->idx + (offset * 3)) % 8;
  
  if (this->idx < this->numleds - 1) {
    this->leds[this->idx + 1] = colorMap[this->currentColor];
  }
  if (x < this->numleds - 1) {
    this->leds[x + 1] = colorMap[this->currentColor];
  }
  if (y < this->numleds - 1) {
    this->leds[y + 1] = colorMap[this->currentColor];
  }
  if (z < this->numleds - 1) {
    this->leds[z + 1] = colorMap[this->currentColor];
  }
  
  this->idx++;
  if (this->idx >= 8) {
    this->idx = 0;
  }
}

void MainLeds::strobe()
{
  fill_solid(this->leds, this->numleds, CRGB::Black);
  
  if (this->strobe_ind) {
    fill_solid(this->leds, this->numleds, colorMap[this->currentColor]);
  }
  
  this->strobe_ind = !this->strobe_ind;
}

void MainLeds::smoothPulse()
{
  uint8_t brightness = beatsin8(60 / (this->speed / 20), 50, 255);
  CRGB color = colorMap[this->currentColor];
  
  for(int i = 0; i < this->numleds; i++) {
    this->leds[i] = color;
    this->leds[i].nscale8(brightness);
  }
}

void MainLeds::theaterChase()
{
  fadeToBlackBy(this->leds + 1, this->numleds - 1, 20);
  
  for(int i = 0; i < 3; i++) {
    int pos = (this->idx + i * 3) % (this->numleds - 1);
    if (pos < this->numleds - 1) {
      this->leds[pos + 1] = colorMap[this->currentColor];
    }
  }
  
  this->idx++;
  if (this->idx >= (this->numleds - 1)) {
    this->idx = 0;
  }
}

void MainLeds::rainbow()
{
  static uint8_t hue = 0;
  fill_rainbow(this->leds, this->numleds, hue, 255 / this->numleds);
  hue += map(this->speed, 20, 200, 10, 1);
}

void MainLeds::fire()
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
    CRGB color = HeatColor(this->heat[j]);
    this->leds[j] = color;
  }
}

void MainLeds::circleChase()
{
  fadeToBlackBy(this->leds, this->numleds, 50);
  
  // Keep center pulsing
  CHSV hsv = rgb2hsv_approximate(colorMap[this->currentColor]);
  hsv.v = this->pulse;
  this->leds[0] = hsv;
  
  // Chase around the outer ring
  if (this->idx < 8) {
    this->leds[this->idx + 1] = colorMap[this->currentColor];
  }
  
  this->idx++;
  if (this->idx >= 8) {
    this->idx = 0;
  }
}

void MainLeds::centerExpand()
{
  fill_solid(this->leds, this->numleds, CRGB::Black);
  
  switch(this->idx) {
    case 0: // Center only
      this->leds[0] = colorMap[this->currentColor];
      break;
    case 1: // Center + cross
      this->leds[0] = colorMap[this->currentColor];
      this->leds[2] = colorMap[this->currentColor];
      this->leds[4] = colorMap[this->currentColor];
      this->leds[6] = colorMap[this->currentColor];
      this->leds[8] = colorMap[this->currentColor];
      break;
    case 2: // Center + corners
      this->leds[0] = colorMap[this->currentColor];
      this->leds[1] = colorMap[this->currentColor];
      this->leds[3] = colorMap[this->currentColor];
      this->leds[5] = colorMap[this->currentColor];
      this->leds[7] = colorMap[this->currentColor];
      break;
    case 3: // All on
      fill_solid(this->leds, this->numleds, colorMap[this->currentColor]);
      break;
  }
  
  this->idx++;
  if (this->idx >= 4) {
    this->idx = 0;
  }
}

void MainLeds::spiralOut()
{
  fadeToBlackBy(this->leds, this->numleds, 40);
  
  // Spiral pattern from center outward
  int sequence[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  
  if (this->idx < 9) {
    this->leds[sequence[this->idx]] = colorMap[this->currentColor];
  }
  
  this->idx++;
  if (this->idx >= 12) { // Pause at end
    this->idx = 0;
  }
}