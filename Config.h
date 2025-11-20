#ifndef CONFIG_H
#define CONFIG_H

#include <Preferences.h>
#include <FastLED.h>

// Configuration structure for persistent storage
struct PeriscopeConfig {
  uint8_t bottomLedCount;           // 8 or 12
  int8_t startupSequence;           // -1 = none, 0-30 = sequence number
  uint8_t brightness;               // 0-255
  bool autoStart;                   // Start with saved sequence on boot
  char version[4];                  // "2.2"
};

// Custom sequence storage
#define MAX_CUSTOM_SEQUENCES 10
#define MAX_SEQUENCE_COMMANDS 10
#define MAX_COMMAND_LENGTH 20

struct CustomSequence {
  char name[20];                    // Sequence name
  uint8_t commandCount;             // Number of commands
  char commands[MAX_SEQUENCE_COMMANDS][MAX_COMMAND_LENGTH];  // Command strings
  uint16_t delays[MAX_SEQUENCE_COMMANDS];  // Delay after each command (ms)
};

// Custom color storage
#define MAX_COLOR_SLOTS 20           // 0-9 default colors, 10-19 custom colors

struct CustomColor {
  char name[20];                    // Color name
  uint8_t r, g, b;                  // RGB values
  bool isCustom;                    // true if user-defined
};

class ConfigManager {
  private:
    Preferences preferences;
    PeriscopeConfig config;
    CustomSequence customSequences[MAX_CUSTOM_SEQUENCES];
    CustomSequence tempSequence;  // For building new sequences
    CustomColor customColors[MAX_COLOR_SLOTS];

  public:
    ConfigManager();

    // Initialize and load config
    void begin();

    // Config getters
    uint8_t getBottomLedCount();
    int8_t getStartupSequence();
    uint8_t getBrightness();
    bool getAutoStart();

    // Config setters
    void setBottomLedCount(uint8_t count);
    void setStartupSequence(int8_t seq);
    void setBrightness(uint8_t bright);
    void setAutoStart(bool enabled);

    // Save/Load config
    void saveConfig();
    void loadConfig();
    void resetToDefaults();

    // Custom sequence management
    bool createSequence(uint8_t slot, const char* name);
    bool addCommandToTemp(const char* command, uint16_t delayMs = 0);
    bool saveTempSequence(uint8_t slot);
    bool deleteSequence(uint8_t slot);
    CustomSequence* getSequence(uint8_t slot);
    void listSequences();
    void clearTempSequence();

    // Custom color management
    bool setColorRGB(uint8_t slot, uint8_t r, uint8_t g, uint8_t b, const char* name = "");
    bool setColorHSV(uint8_t slot, uint8_t h, uint8_t s, uint8_t v, const char* name = "");
    CRGB getColor(uint8_t slot);
    void listColors();
    void resetColor(uint8_t slot);
    void resetAllColors();

    // CLI command processor
    void processConfigCommand(String cmd);

    // Print current config
    void printConfig();

  private:
    void saveCustomSequences();
    void loadCustomSequences();
    void saveCustomColors();
    void loadCustomColors();
    void initializeDefaultColors();
    bool isValidSlot(uint8_t slot);
    bool isValidColorSlot(uint8_t slot);
};

// Global color map access
extern CRGB colorMap[MAX_COLOR_SLOTS];

#endif
