#include "Config.h"
#include <FastLED.h>

ConfigManager::ConfigManager() {
  // Initialize with defaults
  config.bottomLedCount = 12;
  config.startupSequence = 0;  // Q0 - Original R2D2 startup
  config.brightness = 80;
  config.autoStart = true;
  strcpy(config.version, "2.2");

  // Clear custom sequences
  for (int i = 0; i < MAX_CUSTOM_SEQUENCES; i++) {
    customSequences[i].name[0] = '\0';
    customSequences[i].commandCount = 0;
  }

  clearTempSequence();
}

void ConfigManager::begin() {
  preferences.begin("periscope", false);  // RW mode
  loadConfig();
  loadCustomSequences();
}

uint8_t ConfigManager::getBottomLedCount() {
  return config.bottomLedCount;
}

int8_t ConfigManager::getStartupSequence() {
  return config.startupSequence;
}

uint8_t ConfigManager::getBrightness() {
  return config.brightness;
}

bool ConfigManager::getAutoStart() {
  return config.autoStart;
}

void ConfigManager::setBottomLedCount(uint8_t count) {
  if (count == 8 || count == 12) {
    config.bottomLedCount = count;
    Serial.print("Bottom LEDs set to: ");
    Serial.println(count);
    Serial.println("WARNING: Requires reboot to take effect!");
  } else {
    Serial.println("ERROR: Bottom LED count must be 8 or 12");
  }
}

void ConfigManager::setStartupSequence(int8_t seq) {
  if (seq >= -1 && seq <= 20) {
    config.startupSequence = seq;
    Serial.print("Startup sequence set to: ");
    if (seq == -1) {
      Serial.println("None (LEDs off)");
    } else {
      Serial.print("Q");
      Serial.println(seq);
    }
  } else {
    Serial.println("ERROR: Sequence must be -1 (none) or 0-20");
  }
}

void ConfigManager::setBrightness(uint8_t bright) {
  config.brightness = bright;
  FastLED.setBrightness(bright);  // Apply immediately
  Serial.print("Brightness set to: ");
  Serial.println(bright);
  Serial.println("Applied! Use 'SAVE' to persist.");
}

void ConfigManager::setAutoStart(bool enabled) {
  config.autoStart = enabled;
  Serial.print("Auto-start: ");
  Serial.println(enabled ? "Enabled" : "Disabled");
}

void ConfigManager::saveConfig() {
  preferences.putUChar("bottomLeds", config.bottomLedCount);
  preferences.putChar("startup", config.startupSequence);
  preferences.putUChar("brightness", config.brightness);
  preferences.putBool("autoStart", config.autoStart);
  preferences.putString("version", config.version);

  Serial.println("Configuration saved to flash!");
}

void ConfigManager::loadConfig() {
  // Check if config exists
  if (preferences.isKey("version")) {
    config.bottomLedCount = preferences.getUChar("bottomLeds", 12);
    config.startupSequence = preferences.getChar("startup", 0);
    config.brightness = preferences.getUChar("brightness", 80);
    config.autoStart = preferences.getBool("autoStart", true);
    preferences.getString("version", config.version, 4);

    Serial.println("Configuration loaded from flash");
  } else {
    Serial.println("No saved config found, using defaults");
    saveConfig();  // Save defaults
  }
}

void ConfigManager::resetToDefaults() {
  preferences.clear();
  config.bottomLedCount = 12;
  config.startupSequence = 0;
  config.brightness = 80;
  config.autoStart = true;
  strcpy(config.version, "2.2");

  // Clear custom sequences
  for (int i = 0; i < MAX_CUSTOM_SEQUENCES; i++) {
    customSequences[i].name[0] = '\0';
    customSequences[i].commandCount = 0;
  }

  saveConfig();
  saveCustomSequences();

  Serial.println("Reset to factory defaults!");
}

bool ConfigManager::createSequence(uint8_t slot, const char* name) {
  if (!isValidSlot(slot)) {
    Serial.println("ERROR: Slot must be 1-10");
    return false;
  }

  clearTempSequence();
  strncpy(tempSequence.name, name, 19);
  tempSequence.name[19] = '\0';

  Serial.print("Creating sequence S");
  Serial.print(slot);
  Serial.print(": ");
  Serial.println(name);
  Serial.println("Use 'SEQ ADD S<n> <command> [delay]' to add commands");

  return true;
}

bool ConfigManager::addCommandToTemp(const char* command, uint16_t delayMs) {
  if (tempSequence.commandCount >= MAX_SEQUENCE_COMMANDS) {
    Serial.println("ERROR: Sequence full (max 10 commands)");
    return false;
  }

  strncpy(tempSequence.commands[tempSequence.commandCount], command, MAX_COMMAND_LENGTH - 1);
  tempSequence.commands[tempSequence.commandCount][MAX_COMMAND_LENGTH - 1] = '\0';
  tempSequence.delays[tempSequence.commandCount] = delayMs;
  tempSequence.commandCount++;

  Serial.print("Added: ");
  Serial.print(command);
  if (delayMs > 0) {
    Serial.print(" (delay ");
    Serial.print(delayMs);
    Serial.print("ms)");
  }
  Serial.print(" [");
  Serial.print(tempSequence.commandCount);
  Serial.println("/10]");

  return true;
}

bool ConfigManager::saveTempSequence(uint8_t slot) {
  if (!isValidSlot(slot)) {
    Serial.println("ERROR: Slot must be 1-10");
    return false;
  }

  if (tempSequence.commandCount == 0) {
    Serial.println("ERROR: No commands to save");
    return false;
  }

  // Save to slot
  memcpy(&customSequences[slot - 1], &tempSequence, sizeof(CustomSequence));

  // Persist to flash
  saveCustomSequences();

  Serial.print("Saved sequence S");
  Serial.print(slot);
  Serial.print(" with ");
  Serial.print(tempSequence.commandCount);
  Serial.println(" commands");

  clearTempSequence();
  return true;
}

bool ConfigManager::deleteSequence(uint8_t slot) {
  if (!isValidSlot(slot)) {
    Serial.println("ERROR: Slot must be 1-10");
    return false;
  }

  customSequences[slot - 1].name[0] = '\0';
  customSequences[slot - 1].commandCount = 0;

  saveCustomSequences();

  Serial.print("Deleted sequence S");
  Serial.println(slot);
  return true;
}

CustomSequence* ConfigManager::getSequence(uint8_t slot) {
  if (!isValidSlot(slot)) {
    return nullptr;
  }

  if (customSequences[slot - 1].commandCount > 0) {
    return &customSequences[slot - 1];
  }

  return nullptr;
}

void ConfigManager::listSequences() {
  Serial.println(F("\n=== Custom Sequences ==="));

  bool found = false;
  for (int i = 0; i < MAX_CUSTOM_SEQUENCES; i++) {
    if (customSequences[i].commandCount > 0) {
      found = true;
      Serial.print("S");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(customSequences[i].name);
      Serial.print(" (");
      Serial.print(customSequences[i].commandCount);
      Serial.println(" commands)");

      for (int j = 0; j < customSequences[i].commandCount; j++) {
        Serial.print("  ");
        Serial.print(j + 1);
        Serial.print(". ");
        Serial.print(customSequences[i].commands[j]);
        if (customSequences[i].delays[j] > 0) {
          Serial.print(" [delay ");
          Serial.print(customSequences[i].delays[j]);
          Serial.print("ms]");
        }
        Serial.println();
      }
    }
  }

  if (!found) {
    Serial.println("No custom sequences saved");
  }

  Serial.println(F("========================\n"));
}

void ConfigManager::clearTempSequence() {
  tempSequence.name[0] = '\0';
  tempSequence.commandCount = 0;
}

void ConfigManager::saveCustomSequences() {
  for (int i = 0; i < MAX_CUSTOM_SEQUENCES; i++) {
    char key[20];
    snprintf(key, 20, "seq%d", i);

    if (customSequences[i].commandCount > 0) {
      // Save as JSON-like string: name|count|cmd1,delay1|cmd2,delay2|...
      String data = String(customSequences[i].name) + "|" + String(customSequences[i].commandCount);

      for (int j = 0; j < customSequences[i].commandCount; j++) {
        data += "|";
        data += customSequences[i].commands[j];
        data += ",";
        data += String(customSequences[i].delays[j]);
      }

      preferences.putString(key, data);
    } else {
      preferences.remove(key);
    }
  }
}

void ConfigManager::loadCustomSequences() {
  for (int i = 0; i < MAX_CUSTOM_SEQUENCES; i++) {
    char key[20];
    snprintf(key, 20, "seq%d", i);

    if (preferences.isKey(key)) {
      String data = preferences.getString(key, "");

      if (data.length() > 0) {
        // Parse: name|count|cmd1,delay1|cmd2,delay2|...
        int firstPipe = data.indexOf('|');
        int secondPipe = data.indexOf('|', firstPipe + 1);

        String name = data.substring(0, firstPipe);
        int count = data.substring(firstPipe + 1, secondPipe).toInt();

        strncpy(customSequences[i].name, name.c_str(), 19);
        customSequences[i].name[19] = '\0';
        customSequences[i].commandCount = count;

        int pos = secondPipe + 1;
        for (int j = 0; j < count && j < MAX_SEQUENCE_COMMANDS; j++) {
          int nextPipe = data.indexOf('|', pos);
          if (nextPipe == -1) nextPipe = data.length();

          String cmdPart = data.substring(pos, nextPipe);
          int comma = cmdPart.indexOf(',');

          String cmd = cmdPart.substring(0, comma);
          int delayMs = cmdPart.substring(comma + 1).toInt();

          strncpy(customSequences[i].commands[j], cmd.c_str(), MAX_COMMAND_LENGTH - 1);
          customSequences[i].commands[j][MAX_COMMAND_LENGTH - 1] = '\0';
          customSequences[i].delays[j] = delayMs;

          pos = nextPipe + 1;
        }
      }
    }
  }
}

bool ConfigManager::isValidSlot(uint8_t slot) {
  return (slot >= 1 && slot <= MAX_CUSTOM_SEQUENCES);
}

void ConfigManager::printConfig() {
  Serial.println(F("\n=== Periscope Configuration ==="));
  Serial.print(F("Version: "));
  Serial.println(config.version);
  Serial.print(F("Bottom LEDs: "));
  Serial.println(config.bottomLedCount);
  Serial.print(F("Startup Sequence: "));
  if (config.startupSequence == -1) {
    Serial.println("None");
  } else {
    Serial.print("Q");
    Serial.println(config.startupSequence);
  }
  Serial.print(F("Brightness: "));
  Serial.println(config.brightness);
  Serial.print(F("Auto-Start: "));
  Serial.println(config.autoStart ? "Yes" : "No");
  Serial.println(F("================================\n"));
}

void ConfigManager::processConfigCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();

  if (cmd == "CONFIG") {
    printConfig();
  }
  else if (cmd == "SAVE") {
    saveConfig();
  }
  else if (cmd == "RESET") {
    Serial.println("Are you sure? This will reset ALL settings!");
    Serial.println("Send 'RESET CONFIRM' to proceed");
  }
  else if (cmd == "RESET CONFIRM") {
    resetToDefaults();
  }
  else if (cmd.startsWith("SET LEDS ")) {
    int count = cmd.substring(9).toInt();
    setBottomLedCount(count);
  }
  else if (cmd.startsWith("SET STARTUP Q")) {
    int seq = cmd.substring(13).toInt();
    setStartupSequence(seq);
  }
  else if (cmd.startsWith("SET STARTUP NONE")) {
    setStartupSequence(-1);
  }
  else if (cmd.startsWith("SET BRIGHTNESS ")) {
    int bright = cmd.substring(15).toInt();
    setBrightness(bright);
  }
  else if (cmd.startsWith("SET AUTOSTART ")) {
    String val = cmd.substring(14);
    setAutoStart(val == "ON" || val == "YES" || val == "1");
  }
  else if (cmd.startsWith("SEQ NEW S")) {
    int slot = cmd.substring(9, 10).toInt();
    String name = cmd.substring(11);
    if (name.length() == 0) name = "Unnamed";
    createSequence(slot, name.c_str());
  }
  else if (cmd.startsWith("SEQ ADD S")) {
    int slot = cmd.substring(9, 10).toInt();
    int cmdStart = cmd.indexOf(' ', 11);
    if (cmdStart > 0) {
      String cmdPart = cmd.substring(cmdStart + 1);

      // Check for delay parameter
      int delayStart = cmdPart.indexOf(" DELAY ");
      uint16_t delayMs = 0;
      String command = cmdPart;

      if (delayStart > 0) {
        command = cmdPart.substring(0, delayStart);
        delayMs = cmdPart.substring(delayStart + 7).toInt();
      }

      addCommandToTemp(command.c_str(), delayMs);
    }
  }
  else if (cmd.startsWith("SEQ SAVE S")) {
    int slot = cmd.substring(10).toInt();
    saveTempSequence(slot);
  }
  else if (cmd.startsWith("SEQ DEL S")) {
    int slot = cmd.substring(9).toInt();
    deleteSequence(slot);
  }
  else if (cmd == "SEQ LIST") {
    listSequences();
  }
  else {
    Serial.println("Unknown config command");
    Serial.println("Available commands:");
    Serial.println("  CONFIG - Show configuration");
    Serial.println("  SET LEDS <8|12> - Set bottom LED count");
    Serial.println("  SET STARTUP Q<n> - Set startup sequence");
    Serial.println("  SET STARTUP NONE - Disable startup sequence");
    Serial.println("  SET BRIGHTNESS <0-255> - Set brightness");
    Serial.println("  SET AUTOSTART <ON|OFF> - Enable/disable auto-start");
    Serial.println("  SEQ NEW S<1-10> <name> - Create new sequence");
    Serial.println("  SEQ ADD S<n> <command> [DELAY <ms>] - Add command");
    Serial.println("  SEQ SAVE S<n> - Save sequence");
    Serial.println("  SEQ DEL S<n> - Delete sequence");
    Serial.println("  SEQ LIST - List all sequences");
    Serial.println("  SAVE - Save config to flash");
    Serial.println("  RESET - Reset to defaults");
  }
}
