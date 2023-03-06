// NAME: Watchdog.cpp
// 
// DESC: My watchdog implementation file.
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
#if !defined(TEENSYDUINO)

#include "Watchdog.h"
#include <Arduino.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

/*
 * Turn on the watchdog in interrupt and system reset mode and set the given WDT Oscillator Prescaler
 * values.
 * A given callback function will be saved and called by the Watchdog Interrupt Service Routine, if the 
 * watchdog has fired. Inside the callback function the sketch may save some important data to the EEPROM.
 */
static WatchdogCallbackPtr _watchdogCallbackFunc = 0L;
void Watchdog::watchdogOn(WatchdogPrescalerValue prescalerValue, WatchdogCallbackPtr watchdogCallbackFunc)
{
  _watchdogCallbackFunc = watchdogCallbackFunc;

  uint8_t register wdctrl = ((1<<WDIE) | (1<<WDE) | ((prescalerValue & 0x08) ? (1<<WDP3) : 0) | (prescalerValue & 0x07));
  __asm__ __volatile__ (
    ".equ MCUSR, 0x34             	; register address for MCUSR\n\t"
    ".equ WDTCSR, 0x0060          	; register address for WDTCSR\n\t"
    "	cli                         ; disable all interrupts\n\t"
    "	wdr                         ; reset watchdog timer\n\t"

	"	eor  r1, r1                	; clear r1\n\t"
    "	out  0x34, r1    			; reset MCUSR, clear WDRF\n\t"

    "	lds  r24, WDTCSR		   	; load WDTCSR\n\t"
    "	ori  r24, 0x18      	   	; enter configuration mode: set WDCE and WDE\n\t"
    "	sts  WDTCSR, r24    		; set WDTCSR\n\t"

    "	sts  WDTCSR, %[WDCTRL]      ; enable IRQ and set watchdog timer\n\t"
    "	sei                         ; re-enable all interrupts\n\t"
    : /* output */ 
    : /* input */
		[WDCTRL]  "r"  (wdctrl)
    : /* clobber list */
		"r1", "r24"
  );
}

/*
 * Completly turn off the watchdog and clear all timer settings.
 */
void Watchdog::watchdogOff() {
  __asm__ __volatile__ (
    ".equ MCUSR, 0x34             	; register address for MCUSR\n\t"
    ".equ WDTCSR, 0x0060          	; register address for WDTCSR\n\t"
    "	cli                       	; disable all interrupts\n\t"
    "	wdr                        	; reset watchdog timer\n\t"
    
	"	eor  r1, r1                	; clear r1\n\t"
    "	out  MCUSR, r1		       	; reset MCUSR, clear WDRF\n\t"

    "	lds  r24, WDTCSR		   	; load WDTCSR\n\t"
    "	ori  r24, 0x18      	   	; enter configuration mode: set WDCE and WDE\n\t"
    "	sts  WDTCSR, r24    		; set WDTCSR\n\t"

    "	sts  WDTCSR, r1  			; disable watchdog\n\t"
    "	sei                         ; re-enable all interrupts\n\t"
    : /* output */
	: /* input */
	: /* clobber list */
		"r1", "r24"
  );
}

/*
 * Call this function to show that everything works as expected.
 * It will reset the watchdog timer to avoid it from firing.
 */
void Watchdog::watchdogReset() {
  wdt_reset();
}

/*
 * By enabling both WDT System Reset Mode and WDT Interrupt Mode, the first timeout
 * will disable the WDT Interrupt Mode and run the interrupt handler. The second
 * timeout then causes a system reset. The interrupt handler then has one timeout
 * period for backing up parameters, for example, to EEPROM.
 * 
 * If a callback function was given to save some data to the EEPROM, it will be called.
 * A Write Complete Flag could be a byte in EEPROM indicating whether the backup
 * operation was finished before the system reset. This flag could be checked in the startup
 * code if the WDT Reset Flag is set, and the backed up parameters can be used for
 * restoring system state or debugging purposes. The flag should be cleared during
 * initialization to invalidate the parameters if other types of resets occur.
 */
#ifdef DEFINE_WATCHDOG_ISR
ISR(WDT_vect) // Watchdog timer interrupt.
{
  // get program counter from stack
  uint8_t register PCL, PCH, IND;
  __asm__ __volatile__ (
    "     in  ZL, 0x3d      ; load SPL to Z-register\n\t"
    "     in  ZH, 0x3e      ; load SPH to Z-register\n\t"
#if defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560) 
    "     adiw ZL, 26       ; add 1 to get last byte pushed onto the stack\n\t"
    "     ld  r26, Z+       ; load EIND from stack\n\t"
#else
    "     clr  r26          ; initialize IND\n\t"
    "     adiw ZL, 25       ; add 1 to get last byte pushed onto the stack\n\t"
#endif
    "     ld  r25, Z+       ; load PCH from stack\n\t"
    "     ld  r24, Z        ; load PCL from stack\n\t"
    "     mov %[PCL], r24   ; save lo-byte of PC\n\t"
    "     mov %[PCH], r25   ; save hi-byte of PC\n\t"
    "     mov %[IND], r26   ; save EIND of PC\n\t"
    : /* output */
      [PCL] "=&d" (PCL),
      [PCH] "=&d" (PCH),
      [IND] "=&d" (IND)
    : /* input */
    : /* clobber list */
      "r24", "r25", "r26"
  );
  uint32_t irqPC = ((((uint32_t)IND) << 16) | (((uint16_t)PCH) << 8) | PCL) << 1;  // convert word-ptr to byte-ptr
  if (_watchdogCallbackFunc) _watchdogCallbackFunc(irqPC);
  while(1); // wait for 2nd interrupt to reset the system
}
#endif

#endif /* !defined(TEENSYDUINO) */