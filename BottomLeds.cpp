#include "BottomLeds.h"

BottomLeds::BottomLeds(CRGB *leds, int numleds)
{
  this->lastUpdate = millis();
  this->effectChangeTime = millis();
  this->leds = leds;
  this->numleds = numleds;
  this->speed = 200;
  this->idx = 0;
  this->currentEffect = 0;
  this->currentColor = 0; // Red
  this->autoChange = false; // Manual mode by default
}

void BottomLeds::setEffect(int effect) {
  this->currentEffect = effect;
  this->idx = 0; // Reset index when changing effects
  if (effect == 99) {
    this->autoChange = true;
    this->currentEffect = 0;
  } else {
    this->autoChange = false;
  }
}

void BottomLeds::setColor(int color) {
  if (color >= 0 && color <= 9) {
    this->currentColor = color;
  }
}

void BottomLeds::setSpeed(int speed) {
  this->speed = speed;
}

void BottomLeds::update(unsigned long currentTime)
{
  if ((currentTime - this->lastUpdate) < this->speed) return;

  // Auto-change mode
  if (this->autoChange && (currentTime - this->effectChangeTime) > 10000) {
    this->currentEffect++;
    this->idx = 0;
    this->effectChangeTime = currentTime;
    
    if (this->currentEffect >= 10) {
      this->currentEffect = 1; // Skip 0 (off) in auto mode
    }
  }

  switch (this->currentEffect) {
    case 0: // Off
      fill_solid(this->leds, this->numleds, CRGB::Black);
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
  fill_solid(this->leds, this->numleds, CRGB::Black);
  
  if (this->idx < this->numleds) {
    this->leds[this->idx] = colorMap[this->currentColor];
  }
  
  this->idx++;
  if (this->idx >= this->numleds) {
    this->idx = 0;
  }
}

void BottomLeds::scan()
{
  fill_solid(this->leds, this->numleds, CRGB::Black);

  if (this->idx < this->numleds - 1) {
    this->leds[this->idx] = colorMap[this->currentColor];
    this->leds[this->idx + 1] = colorMap[this->currentColor];
  }

  this->idx += 2;
  
  if (this->idx >= this->numleds) {
    this->idx = 0;
  }
}

void BottomLeds::superscan()
{
  fill_solid(this->leds, this->numleds, CRGB::Black);
  CRGB color = colorMap[this->currentColor];

  switch (this->idx) {
    case 0:
      this->leds[0] = color;
      this->leds[1] = color;
      this->leds[6] = color;
      this->leds[7] = color;
      break;
    case 1:
      this->leds[2] = color;
      this->leds[3] = color;
      this->leds[4] = color;
      this->leds[5] = color;
      break;
    case 2:
      this->leds[3] = color;
      this->leds[5] = color;
      break;
    case 3:
      this->leds[2] = color;
      this->leds[4] = color;
      break;
    case 4:
      this->leds[3] = color;
      this->leds[5] = color;
      break;
    case 5:
      this->leds[2] = color;
      this->leds[3] = color;
      this->leds[4] = color;
      this->leds[5] = color;
      break;
  }

  this->idx++;
  if (this->idx >= 6) {
    this->idx = 0;
  }
}

void BottomLeds::randomLight() {
  fill_solid(this->leds, this->numleds, CRGB::Black);
  int ledIndex = random(this->numleds);
  this->leds[ledIndex] = colorMap[this->currentColor];
}

void BottomLeds::chase() {
  fadeToBlackBy(this->leds, this->numleds, 20);
  
  int pos = beatsin16(13, 0, this->numleds - 1);
  this->leds[pos] = colorMap[this->currentColor];
}

void BottomLeds::comet() {
  fadeToBlackBy(this->leds, this->numleds, 20);
  
  this->leds[this->idx] = colorMap[this->currentColor];
  
  // Add trail
  if (this->idx > 0) {
    this->leds[this->idx - 1] = colorMap[this->currentColor];
    this->leds[this->idx - 1].fadeToBlackBy(128);
  }
  
  this->idx++;
  if (this->idx >= this->numleds) {
    this->idx = 0;
  }
}

void BottomLeds::wave() {
  fill_solid(this->leds, this->numleds, CRGB::Black);
  
  // Wave effect for 2x4 arrangement
  switch(this->idx) {
    case 0: // Left column
      this->leds[0] = colorMap[this->currentColor];
      this->leds[7] = colorMap[this->currentColor];
      break;
    case 1:
      this->leds[1] = colorMap[this->currentColor];
      this->leds[6] = colorMap[this->currentColor];
      break;
    case 2:
      this->leds[2] = colorMap[this->currentColor];
      this->leds[5] = colorMap[this->currentColor];
      break;
    case 3: // Right column
      this->leds[3] = colorMap[this->currentColor];
      this->leds[4] = colorMap[this->currentColor];
      break;
  }
  
  this->idx++;
  if (this->idx >= 4) {
    this->idx = 0;
  }
}

void BottomLeds::alternateRows() {
  static bool topRow = true;
  
  fill_solid(this->leds, this->numleds, CRGB::Black);
  
  if (topRow) {
    // Top row on
    for(int i = 0; i < 4; i++) {
      this->leds[i] = colorMap[this->currentColor];
    }
  } else {
    // Bottom row on
    for(int i = 4; i < 8; i++) {
      this->leds[i] = colorMap[this->currentColor];
    }
  }
  
  topRow = !topRow;
}

void BottomLeds::snake() {
  fill_solid(this->leds, this->numleds, CRGB::Black);
  
  // Snake pattern through 2x4 grid
  // Path: 0->1->2->3->4->5->6->7->0
  int snakeLength = 3;
  
  for(int i = 0; i < snakeLength; i++) {
    int pos = (this->idx - i + this->numleds) % this->numleds;
    this->leds[pos] = colorMap[this->currentColor];
    
    // Fade tail
    if (i > 0) {
      this->leds[pos].fadeToBlackBy(i * 80);
    }
  }
  
  this->idx++;
  if (this->idx >= this->numleds) {
    this->idx = 0;
  }
}