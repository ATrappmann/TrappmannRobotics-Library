// NAME: SystemConfig.h
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
#ifndef SYSTEMCONFIG_H
#define SYSTEMCONFIG_H

#include <stdint.h>

class ConfigHeader {
protected:
  // The checksum always has to be the first in the list of attributes.
  // It is stored last, to signal that EEPROM data is valid.
  uint8_t checksum;

  // Magic byte which is specific for our sketch to show, that 
  // this is our configuration dataset.
  uint8_t magic;

protected:
  ConfigHeader() {}
};

class SystemConfig : private ConfigHeader {
// System config data can be accessed freely
public:
  // Reset cycle counter to discover endless reset loops.
  uint8_t resetCounter;
  
  // Remaining free RAM when this configuration was saved.
  uint32_t freeMemoryAtIRQ;

  // Program counter when the Watchdog fired the interrupt.
  uint16_t programCounterAtIRQ;

  // Current time when Watchdog fired.
  uint32_t timeAtIRQ;
  
public:
  SystemConfig();

  bool loadConfig();
  void saveConfig();

  void invalidateEEPROM();
  void init();

  void dumpEEPROM(Print& out);
  
private:
  bool isValid();
  uint8_t calcChecksum();
};

#endif /* SYSTEMCONFIG_H */
