// NAME: Watchdog.ino
//
// DESC: Beispiel, wie man den Arduino mit einem Watchdog überwachen kann.
//
// LINK: https://arduino-projekte.webnode.at/registerprogrammierung/watchdog-interrupt/
//
#include <TrappmannRobotics.h>
#include <TrappmannRobotics/Watchdog.h>
#include <TrappmannRobotics/System.h>
#include <TrappmannRobotics/SystemConfig.h>

SystemConfig sysconfig;

void setup() {
  Watchdog::watchdogOff();
  
  Serial.begin(115200);
  while (!Serial);
  Serial << F("------------------------------\n");
  Serial << F("Sketch: ") << getBaseName(__FILE__) << LF;
  Serial << F("Uploaded: ") << TrappmannRobotics::getUploadTimestamp() << LF;
  Serial << F("Reset Flags: 0b") << toBinaryString(System::getResetFlags()) << LF;

  if (!System::hasValidResetFlags()) {
    Serial << F("ERROR: resetFlags not initialized.\n" \
                "The bootloader did not pass the MCUSR in register R2 to the sketch.\n" \
                "Consider a bootloader update from https://github.com/Optiboot/optiboot\n" \
                "Done. Halting System!\n");
    Serial.flush();
    exit(0);
  }
  if (System::isResetByPowerOn())  Serial << F("--> Power-On Reset\n");
  if (System::isResetByExtern())   Serial << F("--> External Reset\n");
  if (System::isResetByWatchdog()) Serial << F("--> Watchdog Reset\n");
    
  sysconfig.dumpEEPROM(Serial);

  Serial << F("Checking for Watchdog-Reset... ");
  if (System::isResetByWatchdog()) {
    Serial << F("yes\n");
    
    if (sysconfig.loadConfig()) {
      Serial << F("Watchdog-Counter       = ") << sysconfig.resetCounter << LF;
      Serial << F("Free memory at IRQ     = ") << sysconfig.freeMemoryAtIRQ << F(" bytes\n");
      Serial << F("Program counter at IRQ = 0x"); Serial.println(sysconfig.programCounterAtIRQ, HEX);
      Serial << F("Time at IRQ            = ") << sysconfig.timeAtIRQ << F(" ms\n");      
      
      if (sysconfig.resetCounter > 0) {
        Serial << F("Watchdog counter exceeds limit.\n" \
                    "System halted.\n");
        Serial.flush();
        exit(0);
      }
      
    }
    else {
      Serial << F("No SystemConfig data was stored in EEPROM\n");
      // start with new configuration
      sysconfig.init();
    }
  }
  else {
    Serial << F("no\n");
    
    // invalidate EEPROM, because startup was not because of Watchdog Reset
    sysconfig.invalidateEEPROM();
    Serial << F("Invalidated EEPROM data!\n");
  
    // start with new configuration
    sysconfig.init();
  }
  
  Serial << F("Setting Watchdog-Timer to 1s\n");
  Watchdog::watchdogOn(WDTO_1s, watchdogCallback);
  
  Serial.println(F("Running..."));
}

int count = 0;
void loop() {
  Watchdog::watchdogReset();  // Zähler zurücksetzen

  Serial << F("loop #") << (++count) << LF;
  delay(1500);  // wati for watchdog

  Watchdog::watchdogOff();
  Serial.println(F("INTERNAL ERROR: This should never been happend!?"));
  Serial.println(F("Halting system"));
  Serial.flush();
  exit(0);
}

/*   
 * This callback function is called from the Watchdog ISR.
 * It should not contain any blocking function calls.
 */
void watchdogCallback(uint32_t irqPC) {
  Serial << F("*** ISR Watchdog-Callback, time=") << millis() << ", PC=0x"; Serial.println(irqPC, HEX);

  // save config data
  sysconfig.resetCounter++;
  sysconfig.freeMemoryAtIRQ = TrappmannRobotics::getFreeMemory();
  sysconfig.programCounterAtIRQ = irqPC;
  sysconfig.timeAtIRQ = millis();
  sysconfig.saveConfig();
  
  Serial.println("ISR done.");
  Serial.flush();
}
