// NAME: SystemConfig.cpp
//
// DESC: Class for storing configuration data to the EEPROM.
//
// This file is part of the TrappmannRobotics-Library for the Arduino environment.
// https://github.com/ATrappmann/TrappmannRobotics-Library
//
// MIT License
//
// Copyright (c) 2020 Andreas Trappmann
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include <Arduino.h>
#include <EEPROM.h>
#include "SystemConfig.h"

#define MAGIC_HEADER_ID     0x42

#define EEPROM_CONFIG_ADDR  0

SystemConfig::SystemConfig() { }

bool SystemConfig::loadConfig() {
  EEPROM.get(EEPROM_CONFIG_ADDR, *this);
  return isValid();
}

void SystemConfig::saveConfig() {
  // start writing config data to EEPROM
  checksum = 0;
  magic = MAGIC_HEADER_ID;
  EEPROM.put(EEPROM_CONFIG_ADDR, *this);
  
  // to show that saving data to the EEPROM was successful,
  // last thing to do is calculate and store the checksum to
  // make EEPROM data valid.
  checksum = calcChecksum();
  EEPROM.put(EEPROM_CONFIG_ADDR, checksum);
}

void SystemConfig::invalidateEEPROM() {
  checksum = 0;
  EEPROM.put(EEPROM_CONFIG_ADDR, checksum);
}

void SystemConfig::init() {
  resetCounter = 0;
  freeMemoryAtIRQ = 0;
  programCounterAtIRQ = 0;
  timeAtIRQ = 0;
}

bool SystemConfig::isValid() {
  if (MAGIC_HEADER_ID != this->magic) return false;
  if (calcChecksum() != this->checksum) return false;
  return true;  
}

uint8_t SystemConfig::calcChecksum() {
  uint8_t checksum = 0;
  uint8_t *p =(uint8_t*)this;

  // calc checksum for SystemConfig data but skip data from ConfigHeader
  for (int i=sizeof(ConfigHeader); i<sizeof(SystemConfig); i++) {
    checksum ^= p[i];
  }
  
  return checksum;
}

void SystemConfig::dumpEEPROM(Print& out) {
  out.print("SystemConfig: ");
  for (int i=0; i<sizeof(SystemConfig); i++) {
    uint8_t value;
    EEPROM.get(EEPROM_CONFIG_ADDR+i, value);
    String buffer = "0" + String(value, HEX);
    out.print(buffer.substring(buffer.length()-2));
    out.print(" ");
  }
  out.println();
}
