// NAME: TrappmannRobotics.cpp
//
// DESC: This is the implementation of some useful helper functions.
//
// This file is part of the TrappmannRobotics-Library for the Arduino environment.
// https://github.com/ATrappmann/TrappmannRobotics-Library
//
// MIT License
//
// Copyright (c) 2020-2023 Andreas Trappmann
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

const char *TrappmannRobotics::getUploadTimestamp() {
	static const char *msg = __DATE__ " " __TIME__;
	return msg;
}

#if defined(__avr__)
/*
 * Calculate free RAM on the heap which is available for dynamic memory allocation.
 * __brkval is a pointer to the end of heap space or 0, if nothing was allocated.
 * topOfStack is a local variable, which will be allocated on the top of the stack.
 * __malloc_heap_start is a pointer to the start of the heap space.
 */
uint32_t TrappmannRobotics::getFreeMemory() {
  extern char *__brkval;
  char topOfStack;
  return (__brkval ? &topOfStack - __brkval : &topOfStack - __malloc_heap_start);
}

uint32_t TrappmannRobotics::getProgramCounter() {
  // get program counter from stack
  uint16_t PC;
  uint8_t  IND;
  __asm__ __volatile__ (
	".equ SPL,	0x3d			  ; register address of SPL\n\t"\
	".equ SPH,	0x3e			  ; register address of SPH\n\t"\
	"	  		rjmp .gpc2		  ; \n\t"
	".gpc1:		in   ZL, SPL      ; load SPL to Z-register\n\t"
    "     		in   ZH, SPH      ; load SPH to Z-register\n\t"
    "     		adiw ZL, 1        ; add 1 to get last byte pushed onto the stack\n\t"
#if defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560)
    "     		ld   r26, Z+      ; load EIND from stack\n\t"
#else
    "     		clr  r26          ; initialize IND\n\t"
#endif
    "     		ld   r25, Z+      ; load PCH from stack\n\t"
    "     		ld   r24, Z       ; load PCL from stack\n\t"
    "		    movw %[PC], r24   ; save PC\n\t"
    "     		mov  %[IND], r26  ; save EIND of PC\n\t"
	"			ret 			  ; return\n\t"
	".gpc2:		rcall .gpc1		  ; call subroutine to push PC on the stack\n\t"
    : /* output */
      [PC]  "=&d" (PC),
      [IND] "=&d" (IND)
    : /* input */
    : /* clobber list */
      "r24", "r25", "r26", "r30", "r31"
  );
  uint32_t LPC = ((((uint32_t)IND) << 16) | PC) << 1;  // convert word-ptr to byte-ptr
  return LPC;
}
#endif
