#include "TopLeds.h"

TopLeds::TopLeds(CRGB *leds, int numleds)
{
  this->lastUpdate = millis();
  this->effectChangeTime = millis();
  this->leds = leds;
  this->numleds = numleds;
  this->speed = 200;
  this->idx = 0;
  this->currentEffect = 0;
  this->currentColor = 4; // Blue
  this->autoChange = false;
}

void TopLeds::setEffect(int effect) {
  this->currentEffect = effect;
  this->idx = 0;
  
  if (effect == 99) {
    this->autoChange = true;
    this->currentEffect = 0;
  } else {
    this->autoChange = false;
  }
}

void TopLeds::setColor(int color) {
  if (color >= 0 && color <= 9) {
    this->currentColor = color;
  }
}

void TopLeds::setSpeed(int speed) {
  this->speed = speed;
}

void TopLeds::update(unsigned long currentTime)
{
  if ((currentTime - this->lastUpdate) < this->speed) return;

  // Auto-change mode
  if (this->autoChange && (currentTime - this->effectChangeTime) > 5000) {
    this->currentEffect++;
    this->idx = 0;
    this->effectChangeTime = currentTime;
    
    if (this->currentEffect >= 12) {
      this->currentEffect = 1; // Skip 0 (off) in auto mode
    }
  }

  switch (this->currentEffect) {
    case 0: // Off
      fill_solid(this->leds, this->numleds, CRGB::Black);
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
      this->sparkle();
      break;
    case 5: // Comet
      this->comet();
      break;
    case 6: // Chase
      this->chase();
      break;
    case 7: // Rainbow
      this->rainbow();
      break;
    case 8: // Pulse
      this->pulse();
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
  }
    
  this->lastUpdate = currentTime;
}

void TopLeds::leftrun()
{
  fill_solid(this->leds, this->numleds, CRGB::Black);

  if (this->idx < this->numleds) {
    this->leds[this->idx] = colorMap[this->currentColor];
  }
  
  this->idx++;
  if (this->idx >= this->numleds) {
    this->idx = 0;
  }
}

void TopLeds::leftright()
{
  fill_solid(this->leds, this->numleds, CRGB::Black);

  int left = this->idx;
  int right = (this->numleds - 1) - this->idx;

  if (left < this->numleds) {
    this->leds[left] = colorMap[this->currentColor];
  }
  if (right >= 0 && right < this->numleds) {
    this->leds[right] = colorMap[this->currentColor];
  }
  
  this->idx++;
  if (this->idx >= 6) {
    this->idx = 0;
  }
}

void TopLeds::tocenter()
{
  fill_solid(this->leds, this->numleds, CRGB::Black);

  int left = this->idx;
  int right = (this->numleds - 1) - this->idx;

  if (left < this->numleds) {
    this->leds[left] = colorMap[this->currentColor];
  }
  if (right >= 0 && right < this->numleds) {
    this->leds[right] = colorMap[this->currentColor];
  }
  
  this->idx++;
  if (this->idx >= 4) {
    this->idx = 0;
  }
}

void TopLeds::sparkle()
{
  fadeToBlackBy(this->leds, this->numleds, 10);
  
  if (random8() < 120) {
    int pos = random(this->numleds);
    this->leds[pos] = colorMap[this->currentColor];
  }
}

void TopLeds::comet()
{
  fadeToBlackBy(this->leds, this->numleds, 20);
  
  int pos = beatsin16(13, 0, this->numleds - 1);
  this->leds[pos] = colorMap[this->currentColor];
  
  // Add trail
  if (pos > 0) {
    this->leds[pos - 1] = colorMap[this->currentColor];
    this->leds[pos - 1].fadeToBlackBy(128);
  }
  if (pos < this->numleds - 1) {
    this->leds[pos + 1] = colorMap[this->currentColor];
    this->leds[pos + 1].fadeToBlackBy(128);
  }
}

void TopLeds::chase()
{
  fadeToBlackBy(this->leds, this->numleds, 20);
  
  this->leds[this->idx] = colorMap[this->currentColor];
  
  this->idx++;
  if (this->idx >= this->numleds) {
    this->idx = 0;
  }
}

void TopLeds::rainbow()
{
  static uint8_t hue = 0;
  fill_rainbow(this->leds, this->numleds, hue, 255 / this->numleds);
  hue += map(this->speed, 50, 400, 10, 1);
}

void TopLeds::pulse()
{
  uint8_t brightness = beatsin8(30, 50, 255);
  CRGB color = colorMap[this->currentColor];
  
  for(int i = 0; i < this->numleds; i++) {
    this->leds[i] = color;
    this->leds[i].nscale8(brightness);
  }
}

void TopLeds::bounce()
{
  fill_solid(this->leds, this->numleds, CRGB::Black);
  
  // Ball bouncing left to right and back
  static int direction = 1;
  
  this->leds[this->idx] = colorMap[this->currentColor];
  
  // Add slight trail
  if (this->idx > 0 && this->idx < this->numleds - 1) {
    if (direction > 0 && this->idx > 0) {
      this->leds[this->idx - 1] = colorMap[this->currentColor];
      this->leds[this->idx - 1].fadeToBlackBy(180);
    } else if (direction < 0 && this->idx < this->numleds - 1) {
      this->leds[this->idx + 1] = colorMap[this->currentColor];
      this->leds[this->idx + 1].fadeToBlackBy(180);
    }
  }
  
  this->idx += direction;
  
  if (this->idx >= this->numleds - 1 || this->idx <= 0) {
    direction = -direction;
  }
}

void TopLeds::fillFromCenter()
{
  fill_solid(this->leds, this->numleds, CRGB::Black);
  
  int center = this->numleds / 2;
  
  // Fill from center outward
  for(int i = 0; i <= this->idx; i++) {
    if (center + i < this->numleds) {
      this->leds[center + i] = colorMap[this->currentColor];
    }
    if (center - i >= 0) {
      this->leds[center - i] = colorMap[this->currentColor];
    }
  }
  
  this->idx++;
  if (this->idx > center) {
    this->idx = 0;
  }
}

void TopLeds::knightRider()
{
  fadeToBlackBy(this->leds, this->numleds, 60);
  
  static int direction = 1;
  int width = 3; // Width of the scanner
  
  // Draw the scanner
  for(int i = 0; i < width; i++) {
    int pos = this->idx + i - (width / 2);
    if (pos >= 0 && pos < this->numleds) {
      this->leds[pos] = colorMap[this->currentColor];
      
      // Center is brightest
      if (i != width / 2) {
        this->leds[pos].fadeToBlackBy(80);
      }
    }
  }
  
  this->idx += direction;
  
  if (this->idx >= this->numleds - 1 || this->idx <= 0) {
    direction = -direction;
  }
}