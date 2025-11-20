#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// LED Effect Constants
namespace LedConstants {
    // Timing
    constexpr uint16_t AUTO_CHANGE_INTERVAL_MAIN = 10000;    // 10 seconds for main effects
    constexpr uint16_t AUTO_CHANGE_INTERVAL_SIDE = 5000;     // 5 seconds for side effects
    constexpr uint16_t AUTO_CHANGE_INTERVAL_BACK = 5000;     // 5 seconds for back effects

    // Speed defaults
    constexpr uint16_t DEFAULT_SPEED_MAIN = 100;
    constexpr uint16_t DEFAULT_SPEED_SIDE = 200;
    constexpr uint16_t DEFAULT_SPEED_TOP = 200;
    constexpr uint16_t DEFAULT_SPEED_BOTTOM = 200;
    constexpr uint16_t DEFAULT_SPEED_BACK = 500;

    // Pulse parameters
    constexpr uint16_t PULSE_SPEED_DEFAULT = 20;
    constexpr uint16_t PULSE_SPEED_MINIMUM = 10;
    constexpr uint8_t PULSE_VALUE_DEFAULT = 50;
    constexpr int8_t PULSE_OFFSET_DEFAULT = 1;
    constexpr uint8_t PULSE_VALUE_MIN = 50;
    constexpr uint8_t PULSE_VALUE_MAX = 255;

    // Fire effect parameters
    constexpr uint8_t FIRE_COOLING_BASE = 55;
    constexpr uint8_t FIRE_COOLING_MULTIPLIER = 10;
    constexpr uint8_t FIRE_IGNITION_THRESHOLD = 120;
    constexpr uint8_t FIRE_HEAT_MIN = 160;
    constexpr uint8_t FIRE_HEAT_MAX = 255;
    constexpr uint8_t FIRE_SPARK_RANGE = 7;

    // Fade amounts
    constexpr uint8_t FADE_AMOUNT_STANDARD = 20;
    constexpr uint8_t FADE_AMOUNT_LIGHT = 10;
    constexpr uint8_t FADE_AMOUNT_MEDIUM = 40;
    constexpr uint8_t FADE_AMOUNT_HEAVY = 50;
    constexpr uint8_t FADE_AMOUNT_VERY_HEAVY = 80;
    constexpr uint8_t FADE_MULTIPLIER_THEATER = 128;
    constexpr uint8_t FADE_MULTIPLIER_WAVE = 180;

    // Effect parameters
    constexpr uint8_t STROBE_THRESHOLD_SPARKLE = 120;
    constexpr uint8_t BEATSIN_FREQUENCY_BREATHE = 13;
    constexpr uint8_t BEATSIN_FREQUENCY_PULSE = 30;
    constexpr uint8_t RAINBOW_HUE_SHIFT_FAST = 10;
    constexpr uint8_t RAINBOW_HUE_SHIFT_SLOW = 1;

    // LED arrangement
    constexpr uint8_t MAIN_LED_CIRCLE_SIZE = 8;     // Outer ring (excluding center)
    constexpr uint8_t MAX_LEDS_PER_STRIP = 16;      // Maximum LEDs per strip for buffer allocation

    // Color defaults
    constexpr uint8_t DEFAULT_COLOR_WHITE = 8;
    constexpr uint8_t DEFAULT_COLOR_BLUE = 4;
    constexpr uint8_t MAX_COLOR_INDEX = 9;

    // Effect limits
    constexpr uint8_t MAX_EFFECT_MAIN = 17;
    constexpr uint8_t MAX_EFFECT_SIDE = 8;
    constexpr uint8_t MAX_EFFECT_TOP = 12;
    constexpr uint8_t MAX_EFFECT_BOTTOM = 10;
    constexpr uint8_t MAX_EFFECT_BACK = 6;

    // Random thresholds
    constexpr uint8_t RANDOM_THRESHOLD_LOW = 80;
    constexpr uint8_t RANDOM_THRESHOLD_MEDIUM = 120;

    // Modulo divisors
    constexpr uint8_t COLOR_WRAP_MODULO = 10;

    // Array indices
    constexpr uint8_t INDEX_ZERO = 0;
    constexpr uint8_t INDEX_TWO = 2;
    constexpr uint8_t INDEX_THREE = 3;
    constexpr uint8_t INDEX_FOUR = 4;
    constexpr uint8_t INDEX_SIX = 6;

    // Snake effect
    constexpr uint8_t SNAKE_LENGTH = 3;

    // Wave effect
    constexpr uint8_t WAVE_MAX_INDEX = 4;

    // Knight Rider scanner
    constexpr uint8_t SCANNER_WIDTH = 3;
    constexpr uint8_t SCANNER_FADE_AMOUNT = 60;
}

#endif
