// NAME: CallWithTimeout.cpp
//
#include "CallWithTimeout.h"
#include <Arduino.h>

static uint16_t alarmSP;
static bool alarmRC;

bool callWithWatchdog(void (*func)(void *, void*), void *args, void *results, WatchdogPrescalerValue timeout) {
  Watchdog::watchdogOn(timeout, 0L);

  // save current stack pointer to gobal variable
  asm volatile (
	".equ SPL,	0x3d			; register address of SPL\n\t"\
	".equ SPH,	0x3e			; register address of SPH\n\t"\
    "     in    %A[ASP], SPL    ; load SPL to global variable\n\t"\
    "     in    %B[ASP], SPH    ; load SPH to global variable\n\t"\
    : /* output */\
      [ASP]  "=&d"  (alarmSP)\
  );
  
  // call the given function with its parameters
  asm volatile (
    "   movw  r22, %[RSLT]      ; load results ptr as 2nd argument to registers 22/23\n\t"
    "   movw  r24, %[ARGS]      ; load args ptr as 1st argument to registers 24/25\n\t"
    "   icall                   ; call function func() pointed to by Z-register\n\t"
    : /* output */
    : /* input */
      [FUNC]  "z"   (func),
      [ARGS]  "d"   (args),
      [RSLT]  "d"   (results)
    : /* clobber list */
      /* save all called-saved registers (r2-r17, r28-r29) which could be 
       * moddified by calling the given function
       */
      "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9",
      "r10", "r11", "r12", "r13", "r14", "r15", "r16", "r17",
      "r28", "r29",
      /* registers used by our code */
      "r22", "r23", "r24", "r25"
  );
  
  // check if watchdog was triggered (WDIE is clear) and set return code
  asm volatile (
    ".equ WDTCSR, 0x0060            ; register address for WDTCSR\n\t"
    ".equ WDIE, 6                   ; WDIE bit of WDTCSR\n\t"
    "     wdr                       ; reset watchdog timer to be on the safe side\n\t"
    "     ldi   r25, 1              ; init with true - no alarm\n\t"
    "     lds   r24, WDTCSR         ; load WDTCSR\n\t"
    "     sbrs  r24, WDIE           ; skip next instruction if WDIE bit in register is set\n\t"
    "     eor   r25, r25            ; set to false - alarm was raised\n\t"
    "     sts   %[STATUS], r25      ; set status\n\t"
    : /* output */
      [STATUS] "=m" (alarmRC)
    : /* input */
    : /* clobber list */
      "r24", "r25"
  );
  
  Watchdog::watchdogOff();
  
  return alarmRC;
}

#ifdef DEFINE_TIMEOUT_ISR
ISR(WDT_vect) {
  // r1, r0 and Status Register (0x3f) are pushed on the stack  (3 bytes)
  // r1 is zero
  // register pair (r24/r25) is pushed to the stack and loaded with alarmSP  (2 bytes)
  // r26 is pushed to the stack (1 byte)
  asm volatile (
    "     cli                     ; clear global interrupt flag\n\t"
#if defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560) 
	"     ldi	r26, 9			  ; offset for Mega with 3-byte return address\n\t"
#else	
    "     ldi   r26, 8            ; offset for Uno with 2-byte return address\n\t"
#endif
    "     sub   %A[ALARMSP], r26  ; sub 6 for registers pushed to stack and 2/3 for return address\n\t"
    "     sbc   %B[ALARMSP], r1   ; substract 0 with carry\n\t"
    "     out   0x3d, %A[ALARMSP] ; set new SPL\n\t"
    "     out   0x3e, %B[ALARMSP] ; set new SPH\n\t"
	"	  sei					  ; set global interrupt flag\n\t"
    : /* output */
    : /* input */
      [ALARMSP] "d"   (alarmSP)
    : /* clobber list */
      "r26"
  );  
}
#endif