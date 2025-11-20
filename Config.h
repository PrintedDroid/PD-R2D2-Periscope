#ifndef CONFIG_H
#define CONFIG_H

#include <Preferences.h>

// Configuration structure for persistent storage
struct PeriscopeConfig {
  uint8_t bottomLedCount;           // 8 or 12
  int8_t startupSequence;           // -1 = none, 0-20 = sequence number
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

class ConfigManager {
  private:
    Preferences preferences;
    PeriscopeConfig config;
    CustomSequence customSequences[MAX_CUSTOM_SEQUENCES];
    CustomSequence tempSequence;  // For building new sequences

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

    // CLI command processor
    void processConfigCommand(String cmd);

    // Print current config
    void printConfig();

  private:
    void saveCustomSequences();
    void loadCustomSequences();
    bool isValidSlot(uint8_t slot);
};

#endif
