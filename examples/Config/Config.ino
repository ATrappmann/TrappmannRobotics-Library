// NAME: Config.ino
//
// DESC: Example for storing configuration data persistent to the EEPROM.
//
#define DEBUG 1
#include <TrappmannRobotics.h>
#include <TrappmannRobotics/SketchConfiguration.h>

#define MAGIC_HEADER_ID     42

struct SketchConfig {
  int value;
};

SketchConfiguration<SketchConfig> config;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial << F("Running...\n");

  dumpEEPROM(Serial);

  if (config.loadConfig(MAGIC_HEADER_ID)) {
    Serial.print("Found! value="); Serial.println(config.data.value);
    config.deleteConfig();
  }
  else {
    Serial.println("Not found! Saving...");
    config.data.value = 0x42;
    config.saveConfig(MAGIC_HEADER_ID);
  }

  dumpEEPROM(Serial);

}

void dumpEEPROM(Print& out) {
  out.print("SystemConfig: ");
  for (int i=0; i<sizeof(config); i++) {
    uint8_t value;
    EEPROM.get(EEPROM_CONFIG_ADDR+i, value);
    String buffer = "0" + String(value, HEX);
    out.print(buffer.substring(buffer.length()-2));
    out.print(" ");
  }
  out.println();
}

void loop() {}
