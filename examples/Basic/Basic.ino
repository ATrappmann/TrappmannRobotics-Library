// NAME: Basic.ino
//
// DESC: Basic sketch template.
//
// CONNECTIONS: 
//  Arduino Dx -> XX
//
// Copyright (c) 2018-2020 by Andreas Trappmann
// All rights reserved.
//

#include <TrappmannRobotics.h>
#include <TrappmannRobotics/Watchdog.h>
#include <TrappmannRobotics/System.h>

void setup() {
  uint32_t pc = TrappmannRobotics::getProgramCounter();
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  Serial << F("------------------------------\n");
  Serial << F("Sketch: ") << getBaseName(__FILE__) << LF;
  Serial << F("Uploaded: ") << TrappmannRobotics::getUploadTimestamp() << LF;
  Serial << F("Free Memory: ") << TrappmannRobotics::getFreeMemory() << LF;
  Serial << F("Address of setup() function 0x") << toHexString(((uint32_t)setup) << 1) << LF;
  Serial << F("Program Counter of setup(): 0x") << toHexString(pc) << LF;

  if (System::hasValidResetFlags()) {
    if (System::isResetByPowerOn())  Serial << F("Power-On Reset Flag is set\n");
    if (System::isResetByExtern())   Serial << F("External Reset Flag is set\n");
    if (System::isResetByWatchdog()) Serial << F("Watchdog Reset Flag is set\n");
  }
  else Serial << F("Bootloader does not pass reset flags to sketch\n");

}

void loop() {
  Serial << F("Done.") << LF;
  Serial << F("Press RESET to start again") << LF;
  Serial.flush();
  digitalWrite(LED_PIN, HIGH);
  exit(0); // halt
}
