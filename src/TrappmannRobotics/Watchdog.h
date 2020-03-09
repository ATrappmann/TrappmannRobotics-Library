// NAME: Watchdog.h
// 
// DESC: Headerfile for my watchdog implementation.
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
#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <stdint.h>

typedef void * (*WatchdogCallbackPtr)(uint32_t irqPC);

enum WatchdogPrescalerValue {
  WDTO_16ms = 0,
  WDTO_32ms = 1,
  WDTO_64ms = 2,
  WDTO_125ms = 3,
  WDTO_250ms = 4,
  WDTO_500ms = 5,
  WDTO_1s = 6,
  WDTO_2s = 7,
  WDTO_4s = 8,
  WDTO_8s = 9
};

class Watchdog {
private:
  Watchdog() {}
  
public:
  static void watchdogOn(WatchdogPrescalerValue prescalerValue, WatchdogCallbackPtr watchdogCallbackFunc);
  static void watchdogOff();
  static void watchdogReset();
};

#endif /* WATCHDOG_H */
