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
  initializeDefaultColors();
}

void ConfigManager::begin() {
  preferences.begin("periscope", false);  // RW mode
  loadConfig();
  loadCustomSequences();
  loadCustomColors();
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
  if (seq >= -1 && seq <= 30) {
    config.startupSequence = seq;
    Serial.print("Startup sequence set to: ");
    if (seq == -1) {
      Serial.println("None (LEDs off)");
    } else {
      Serial.print("Q");
      Serial.println(seq);
    }
  } else {
    Serial.println("ERROR: Sequence must be -1 (none) or 0-30");
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

  saveCustomColors();

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

  // Reset colors
  resetAllColors();

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

bool ConfigManager::isValidColorSlot(uint8_t slot) {
  return (slot < MAX_COLOR_SLOTS);
}

void ConfigManager::initializeDefaultColors() {
  // Default colors (0-9) - matching original colorMap
  const char* defaultNames[] = {"Red", "Yellow", "Green", "Cyan", "Blue",
                                  "Magenta", "Orange", "Purple", "White", "Pink"};
  CRGB defaultColors[] = {CRGB::Red, CRGB::Yellow, CRGB::Green, CRGB::Cyan, CRGB::Blue,
                          CRGB::Magenta, CRGB::Orange, CRGB::Purple, CRGB::White, CRGB::Pink};

  for (int i = 0; i < 10; i++) {
    strncpy(customColors[i].name, defaultNames[i], 19);
    customColors[i].name[19] = '\0';
    customColors[i].r = defaultColors[i].r;
    customColors[i].g = defaultColors[i].g;
    customColors[i].b = defaultColors[i].b;
    customColors[i].isCustom = false;
  }

  // Custom slots (10-19) - empty by default
  for (int i = 10; i < MAX_COLOR_SLOTS; i++) {
    sprintf(customColors[i].name, "Custom%d", i - 9);
    customColors[i].r = 128;
    customColors[i].g = 128;
    customColors[i].b = 128;
    customColors[i].isCustom = false;
  }

  // Update global colorMap
  for (int i = 0; i < MAX_COLOR_SLOTS; i++) {
    colorMap[i] = CRGB(customColors[i].r, customColors[i].g, customColors[i].b);
  }
}

bool ConfigManager::setColorRGB(uint8_t slot, uint8_t r, uint8_t g, uint8_t b, const char* name) {
  if (!isValidColorSlot(slot)) {
    Serial.println("ERROR: Color slot must be 0-19");
    return false;
  }

  customColors[slot].r = r;
  customColors[slot].g = g;
  customColors[slot].b = b;
  customColors[slot].isCustom = true;

  if (name && strlen(name) > 0) {
    strncpy(customColors[slot].name, name, 19);
    customColors[slot].name[19] = '\0';
  }

  // Update global colorMap
  colorMap[slot] = CRGB(r, g, b);

  Serial.print("Color ");
  Serial.print(slot);
  Serial.print(" set to RGB(");
  Serial.print(r);
  Serial.print(",");
  Serial.print(g);
  Serial.print(",");
  Serial.print(b);
  Serial.println(")");

  return true;
}

bool ConfigManager::setColorHSV(uint8_t slot, uint8_t h, uint8_t s, uint8_t v, const char* name) {
  if (!isValidColorSlot(slot)) {
    Serial.println("ERROR: Color slot must be 0-19");
    return false;
  }

  CHSV hsv(h, s, v);
  CRGB rgb;
  hsv2rgb_rainbow(hsv, rgb);

  customColors[slot].r = rgb.r;
  customColors[slot].g = rgb.g;
  customColors[slot].b = rgb.b;
  customColors[slot].isCustom = true;

  if (name && strlen(name) > 0) {
    strncpy(customColors[slot].name, name, 19);
    customColors[slot].name[19] = '\0';
  }

  // Update global colorMap
  colorMap[slot] = rgb;

  Serial.print("Color ");
  Serial.print(slot);
  Serial.print(" set to HSV(");
  Serial.print(h);
  Serial.print(",");
  Serial.print(s);
  Serial.print(",");
  Serial.print(v);
  Serial.println(")");

  return true;
}

CRGB ConfigManager::getColor(uint8_t slot) {
  if (isValidColorSlot(slot)) {
    return CRGB(customColors[slot].r, customColors[slot].g, customColors[slot].b);
  }
  return CRGB::White; // Fallback
}

void ConfigManager::listColors() {
  Serial.println(F("\n=== Color Palette ==="));
  Serial.println(F("Slot | Name          | RGB          | Status"));
  Serial.println(F("-----|---------------|--------------|--------"));

  for (int i = 0; i < MAX_COLOR_SLOTS; i++) {
    char line[60];
    snprintf(line, 60, "%4d | %-13s | %3d,%3d,%3d | %s",
             i,
             customColors[i].name,
             customColors[i].r, customColors[i].g, customColors[i].b,
             customColors[i].isCustom ? "Custom" : "Default");
    Serial.println(line);
  }

  Serial.println(F("=====================\n"));
}

void ConfigManager::resetColor(uint8_t slot) {
  if (!isValidColorSlot(slot)) {
    Serial.println("ERROR: Color slot must be 0-19");
    return;
  }

  // Reset to default if slot 0-9, otherwise to gray
  if (slot < 10) {
    const char* defaultNames[] = {"Red", "Yellow", "Green", "Cyan", "Blue",
                                    "Magenta", "Orange", "Purple", "White", "Pink"};
    CRGB defaultColors[] = {CRGB::Red, CRGB::Yellow, CRGB::Green, CRGB::Cyan, CRGB::Blue,
                            CRGB::Magenta, CRGB::Orange, CRGB::Purple, CRGB::White, CRGB::Pink};

    strncpy(customColors[slot].name, defaultNames[slot], 19);
    customColors[slot].r = defaultColors[slot].r;
    customColors[slot].g = defaultColors[slot].g;
    customColors[slot].b = defaultColors[slot].b;
    customColors[slot].isCustom = false;
  } else {
    sprintf(customColors[slot].name, "Custom%d", slot - 9);
    customColors[slot].r = 128;
    customColors[slot].g = 128;
    customColors[slot].b = 128;
    customColors[slot].isCustom = false;
  }

  colorMap[slot] = CRGB(customColors[slot].r, customColors[slot].g, customColors[slot].b);

  Serial.print("Color ");
  Serial.print(slot);
  Serial.println(" reset to default");
}

void ConfigManager::resetAllColors() {
  initializeDefaultColors();
  saveCustomColors();
  Serial.println("All colors reset to defaults");
}

void ConfigManager::saveCustomColors() {
  for (int i = 0; i < MAX_COLOR_SLOTS; i++) {
    if (customColors[i].isCustom) {
      char key[20];
      snprintf(key, 20, "color%d", i);

      // Save as: name|r|g|b
      String data = String(customColors[i].name) + "|" +
                    String(customColors[i].r) + "|" +
                    String(customColors[i].g) + "|" +
                    String(customColors[i].b);

      preferences.putString(key, data);
    }
  }
}

void ConfigManager::loadCustomColors() {
  for (int i = 0; i < MAX_COLOR_SLOTS; i++) {
    char key[20];
    snprintf(key, 20, "color%d", i);

    if (preferences.isKey(key)) {
      String data = preferences.getString(key, "");

      if (data.length() > 0) {
        // Parse: name|r|g|b
        int pipe1 = data.indexOf('|');
        int pipe2 = data.indexOf('|', pipe1 + 1);
        int pipe3 = data.indexOf('|', pipe2 + 1);

        String name = data.substring(0, pipe1);
        int r = data.substring(pipe1 + 1, pipe2).toInt();
        int g = data.substring(pipe2 + 1, pipe3).toInt();
        int b = data.substring(pipe3 + 1).toInt();

        strncpy(customColors[i].name, name.c_str(), 19);
        customColors[i].name[19] = '\0';
        customColors[i].r = r;
        customColors[i].g = g;
        customColors[i].b = b;
        customColors[i].isCustom = true;

        // Update global colorMap
        colorMap[i] = CRGB(r, g, b);
      }
    }
  }
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
  else if (cmd == "COLOR LIST" || cmd == "COLORS") {
    listColors();
  }
  else if (cmd.startsWith("COLOR RGB ")) {
    // Format: COLOR RGB <slot> <r> <g> <b> [name]
    String params = cmd.substring(10);
    int firstSpace = params.indexOf(' ');
    int secondSpace = params.indexOf(' ', firstSpace + 1);
    int thirdSpace = params.indexOf(' ', secondSpace + 1);
    int fourthSpace = params.indexOf(' ', thirdSpace + 1);

    if (firstSpace > 0 && secondSpace > 0 && thirdSpace > 0) {
      int slot = params.substring(0, firstSpace).toInt();
      int r = params.substring(firstSpace + 1, secondSpace).toInt();
      int g = params.substring(secondSpace + 1, thirdSpace).toInt();
      int b = params.substring(thirdSpace + 1, fourthSpace > 0 ? fourthSpace : params.length()).toInt();

      String name = "";
      if (fourthSpace > 0) {
        name = params.substring(fourthSpace + 1);
      }

      setColorRGB(slot, r, g, b, name.c_str());
    } else {
      Serial.println("ERROR: Format: COLOR RGB <slot> <r> <g> <b> [name]");
    }
  }
  else if (cmd.startsWith("COLOR HSV ")) {
    // Format: COLOR HSV <slot> <h> <s> <v> [name]
    String params = cmd.substring(10);
    int firstSpace = params.indexOf(' ');
    int secondSpace = params.indexOf(' ', firstSpace + 1);
    int thirdSpace = params.indexOf(' ', secondSpace + 1);
    int fourthSpace = params.indexOf(' ', thirdSpace + 1);

    if (firstSpace > 0 && secondSpace > 0 && thirdSpace > 0) {
      int slot = params.substring(0, firstSpace).toInt();
      int h = params.substring(firstSpace + 1, secondSpace).toInt();
      int s = params.substring(secondSpace + 1, thirdSpace).toInt();
      int v = params.substring(thirdSpace + 1, fourthSpace > 0 ? fourthSpace : params.length()).toInt();

      String name = "";
      if (fourthSpace > 0) {
        name = params.substring(fourthSpace + 1);
      }

      setColorHSV(slot, h, s, v, name.c_str());
    } else {
      Serial.println("ERROR: Format: COLOR HSV <slot> <h> <s> <v> [name]");
    }
  }
  else if (cmd.startsWith("COLOR RESET ")) {
    int slot = cmd.substring(12).toInt();
    resetColor(slot);
  }
  else if (cmd == "COLOR RESET ALL") {
    resetAllColors();
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
    Serial.println("  COLOR RGB <slot> <r> <g> <b> [name] - Set color RGB");
    Serial.println("  COLOR HSV <slot> <h> <s> <v> [name] - Set color HSV");
    Serial.println("  COLOR LIST - List all colors");
    Serial.println("  COLOR RESET <slot> - Reset color to default");
    Serial.println("  COLOR RESET ALL - Reset all colors");
    Serial.println("  SAVE - Save config to flash");
    Serial.println("  RESET - Reset to defaults");
  }
}
