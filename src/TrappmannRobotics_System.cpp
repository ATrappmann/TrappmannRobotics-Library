// NAME: System.cpp
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
#include <TrappmannRobotics.h>
#include "TrappmannRobotics_System.h"
#include <Arduino.h>

#if defined(__avr__)

/*
 * First, we need a variable to hold the reset cause that can be written before
 * early sketch initialization (that might change r2), and won't be reset by the
 * various initialization code.
 * avr-gcc provides for this via the ".noinit" section.
 */
static uint8_t resetFlags __attribute__ ((section(".noinit")));

/*
 * Next, we need to put some code to save reset cause from the bootload (in r2)
 * to the variable.  Again, avr-gcc provides special code sections for this.
 */
void resetFlagsInit(void) __attribute__ ((naked)) __attribute__ ((section(".init0")));
void resetFlagsInit(void) {
  /*
   * save the reset flags passed from the bootloader
   * This is a "simple" matter of storing (STS) r2 in the special variable
   * that we have created.  We use assembler to access the right variable.
   */
  __asm__ __volatile__ ("sts %[RESETFLAGS], r2\n" : [RESETFLAGS] "=m" (resetFlags) :);
}

System::System() {
  // never called, but necessary for linking otherwise resetFlagsInit will be removed by the optimizer!
  if (0xee == resetFlags) resetFlagsInit();
}

uint8_t System::getResetFlags() {
  return resetFlags;
}

#if defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560)
bool System::isResetByJTAG() {
  return (!isResetByPowerOn() && (resetFlags & (1<<JTRF)));
}
#endif

/*
 * There are several reasons for a reset of the Arduino controller.
 * If you want the reason for the startup (ie. to reconstruct some
 * configuration data saved in the EEPROM), call this funktion and
 * check, if Sketch was started by the watchdog timer.
 */
bool System::isResetByWatchdog() {
  return (!isResetByPowerOn() && !isResetByExtern() && (resetFlags & (1<<WDRF)));
}

bool System::isResetByBrownOut() {
  return (!isResetByPowerOn() && (resetFlags & (1<<BORF)));
}

/*
 *
 */
bool System::isResetByExtern() {
  return (!isResetByPowerOn() && (resetFlags & (1<<EXTRF)));
}

/*
 *
 */
bool System::isResetByPowerOn() {
  return (resetFlags & (1<<PORF));
}

void System::printResetFlags(Print& out) {
#if defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560)
  if (resetFlags & (1<<JTRF))	out.print("JTRF ");
#endif
  if (resetFlags & (1<<WDRF))	out.print("WDRF ");
  if (resetFlags & (1<<BORF))	out.print("BORF ");
  if (resetFlags & (1<<EXTRF))	out.print("EXTRF ");
  if (resetFlags & (1<<PORF)) 	out.print("PORF ");
  out.println();
}

/*
 * Check if bootloader did pass the status of MCUSR in register R2 to our sketch
 * so that the function resetFlagsInit in section ".init0" could initialize the
 * global variable "resetFlags".
 * If not, consider updating the Arduino bootloader. The latest version of
 * optiboot can be found at https://github.com/Optiboot/optiboot
 */
bool System::hasValidResetFlags() {
  if (0 == resetFlags) return false;
//  if (0 != (resetFlags & 0b11100000)) return false;
  return true;
}
#endif

/*
 * Halt system.
 */
void System::halt() {
  Serial << F("Halted! Press RESET to start again\n");
  Serial.flush();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  exit(0); // halt system
}

void System::halt(const String& msg) {
  Serial << "ALERT: " << msg << '\n';
  halt();
}
