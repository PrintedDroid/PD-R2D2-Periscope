#include "BackLeds.h"

BackLeds::BackLeds(CRGB *leds, int numleds)
{
  this->lastUpdate = millis();
  this->leds = leds;
  this->numleds = numleds;
  this->speed = 500;
  this->idx = 0;
  this->currentEffect = 0;
  this->currentColor = 0; // Red
}

void BackLeds::setEffect(int effect) {
  this->currentEffect = effect;
}

void BackLeds::setColor(int color) {
  if (color >= 0 && color <= 9) {
    this->currentColor = color;
  }
}

void BackLeds::setSpeed(int speed) {
  this->speed = speed;
}

void BackLeds::update(unsigned long currentTime)
{
  if ((currentTime - this->lastUpdate) < this->speed) return;

  switch(this->currentEffect) {
    case 0: // Off
      fill_solid(this->leds, this->numleds, CRGB::Black);
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
      this->sparkle();
      break;
  }

  this->lastUpdate = currentTime;
}

void BackLeds::randomB() {
  fill_solid(this->leds, this->numleds, CRGB::Black);
  
  for(int i = 0; i < this->numleds; i++) {
    int nr = random(0, 2);
    if (nr) {
      this->leds[i] = CRGB::Red;  
    }
    else {
      this->leds[i] = CRGB::Blue;
    }
  }
}

void BackLeds::randomColored() {
  for(int i = 0; i < this->numleds; i++) {
    if (random(0, 2)) {
      this->leds[i] = colorMap[this->currentColor];
    } else {
      this->leds[i] = CRGB::Black;
    }
  }
}

void BackLeds::allOn() {
  fill_solid(this->leds, this->numleds, colorMap[this->currentColor]);
}

void BackLeds::alternateColors() {
  static bool phase = false;
  for(int i = 0; i < this->numleds; i++) {
    if ((i % 2) == phase) {
      this->leds[i] = colorMap[this->currentColor];
    } else {
      this->leds[i] = colorMap[(this->currentColor + 1) % 10];
    }
  }
  phase = !phase;
}

void BackLeds::sparkle() {
  fadeToBlackBy(this->leds, this->numleds, 50);
  
  if (random8() < 120) {
    int pos = random(this->numleds);
    this->leds[pos] = colorMap[this->currentColor];
  }
}