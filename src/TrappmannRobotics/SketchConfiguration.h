// NAME: SketchConfiguration.h
//
// DESC: Class for storing Sketch configuration data in the EEPROM.
//
// This file is part of the TrappmannRobotics-Library for the Arduino environment.
// https://github.com/ATrappmann/TrappmannRobotics-Library
//
// MIT License
//
// Copyright (c) 2020-21 Andreas Trappmann
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
#ifndef SKETCH_CONFIGURATION_H
#define SKETCH_CONFIGURATION_H

#include <EEPROM.h>

#define EEPROM_CONFIG_ADDR  0

template <class T>
class SketchConfiguration {
protected:
  // The checksum always has to be the first in the list of attributes.
  // It is stored last, to signal that EEPROM data is valid.
  uint8_t checksum;

  // Magic byte which is specific for the sketch to show, that
  // this is our configuration dataset.
  uint8_t magic;

public:
  T data;

public:
  bool loadConfig(const uint8_t magic) {
    return loadConfig(EEPROM_CONFIG_ADDR, magic);
  }
  bool loadConfig(const uint16_t addr, const uint8_t magic) {
    EEPROM.get(addr, *this);
    return isValid(magic);
  }

  void saveConfig(const uint8_t magic) {
    saveConfig(EEPROM_CONFIG_ADDR, magic);
  }
  void saveConfig(const uint16_t addr, const uint8_t magic) {
    this->magic = magic;
    this->checksum = calcChecksum();
    EEPROM.put(EEPROM_CONFIG_ADDR, *this);
  }

  void deleteConfig() {
    deleteConfig(EEPROM_CONFIG_ADDR);
  }
  void deleteConfig(const uint16_t addr) {
    checksum = 0;
    EEPROM.update(EEPROM_CONFIG_ADDR, checksum);
  }

  bool isValid(const uint8_t magic) {
    if (magic != this->magic) return false;
    if (calcChecksum() != this->checksum) return false;
    return true;
  }

private:
  uint8_t calcChecksum() {
    uint8_t checksum = 0;
    uint8_t *p =(uint8_t*)this;
    for (unsigned int i=0; i<sizeof(T); i++) {
      checksum ^= p[1+i]; // skip checksum field
    }
    return checksum;
  }

};

#endif /* SKETCH_CONFIGURATION_H */
