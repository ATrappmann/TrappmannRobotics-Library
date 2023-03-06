// NAME: System.h
//
// DESC: Class to query information about the system. Currently with methods to
//       check the reset flags for the reason of the reboot. 
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
#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include <Print.h>

class System {
#if !defined(TEENSYDUINO)	
private:
  System();
  
public:
  static uint8_t getResetFlags();
  static bool hasValidResetFlags();
  
#if defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560) 
  static bool isResetByJTAG();
#endif  
  static bool isResetByWatchdog();
  static bool isResetByBrownOut();
  static bool isResetByExtern();
  static bool isResetByPowerOn();
  static void printResetFlags(Print& out);
#endif /* !defined(TEENSYDUINO) */
public:  
  static void halt();
};

#endif /* SYSTEM_H */