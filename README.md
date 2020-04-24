# TrappmannRobotics-Library
This is an Arduino library with a collection of helper functions I always use in
my projects.

## TrappmannRobotics
The `class TrappmannRobotics` is is the main class of the library.
It contains basic methods I use in every project.

### char * getUploadTimestamp()
`getUploadTimestamp` returns the static string `"__DATE__  __TIME__"` which was
constructed while this class was compiled. Printing this string in `setup()`
to `Serial` helps you keep track of Sketch versions.

### uint32_t getFreeMemory()
`getFreeMemory` calculates the free RAM on the heap which is available for
dynamic memory allocation.

### uint32_t getProgramCounter()
`getProgramCounter` returns the program counter to the next instruction
after the call to this method.

## StringHelper.h
The functions in `StringHelper.h` bundle functions for printing with the
stream operator and to format binary and hexadecimal numbers to a well formed string.

### String toBinaryString(const uint8_t value)
`toBinaryString` returns the binary representation of an 8-bit value.
The `String` is always 8 characters long because it is filled up with
leading zeros if the value is to small.

### String toHexString(const uint8_t value)
### String toHexString(const uint16_t value)
### String toHexString(const uint32_t value)
`toHexString` returns the hexadecimal representation of an 8/16/32-bit value.
The `String` is always 2/4/8 characters long because it is filled up with
leading zeros if the value is to small.

### String getBaseName(const char * path)
`getBaseName` returns only the last element of a backslash ('\') separated string.
This can be used to extract the name of the Sketch from the compiler generated
string `__FILE__` an print it in `setup()` to `Serial`.

### String getPathName(const char * path)
`getPathName` is the complement to `getBaseName`. It returns the only the
path of a backslash ('\') separated string while skipping the last element.

## Debug.h
This file defines the makro `SEROUT(msg)` which can be switched on by
inserting `#define DEBUG` in your code. The message may use the stream
operator for printing, which was introduced in `StringHelper.h`.
If defined, it will print the message to `Serial`.
Otherwise the makro eliminates the debugging message from your code.

This makes inserting of debugging information to your code very handy because
you do not have to delete or comment these lines if you are finished. Getting
rid of debugging output can be simply reached by commenting out the
line `//#define DEBUG` in your code.

## System
The `class System` handles the reset Flags and contains a method to `halt`
the Sketch on the microcontroller.

### bool hasValidResetFlags()
`hasValidResetFlags` tells you, if your Arduino Uno, Nano or Mega is able
to return the reset flags of the MCU Status Register to your Sketch.
If this function returns `false`, consider updating your bootloader to a
newer version. You will find updated and tested bootloaders in the `bootloaders`
directory of this library. Copy these files to your Arduino IDE
installation and build them. The process of uploading a new bootloader is
documented there. Check https://www.arduino.cc/en/hacking/bootloader for starters.

### uint8_t getResetFlags()
`getResetFlags` returns the reset flags from the MCU Status Register of an
Arduino Uno, Nano or Mega. The return value of this function has only valid
informations, if the bootloader is able to provide the reset flags to the
Sketch.

### bool isResetByPowerOn()
`isResetByPowerOn` checks the reset flags, if the Power-On Reset Flag (`PORF`)
is set.
In this case it returns `true`, otherwise `false`.

### bool isResetByExtern()
`isResetByExtern` checks the reset flags, if the Extern Reset Flag (`EXTRF`)
is set and no Power-On Reset was detected at the same time.
In this case it returns `true`, otherwise `false`.

### bool isResetByBrownOut()
`isResetByBrownOut` checks the reset flags, if the Brown-Out Reset Flag (`BORF`)
is set and no Power-On Reset was detected at the same time.
In this case it returns `true`, otherwise `false`.

### bool isResetByWatchdog()
`isResetByWatchdog` checks the reset flags, if the Watchdog Reset Flag (`WDRF`)
is set and no Power-On Reset or External Reset were detected at the same time.
In this case it returns `true`, otherwise `false`.

### bool isResetByJTAG()
This is available only on an Arduino Mega.
`isResetByJTAG` checks the reset flags, if the JTAG Reset Flag (`JTRF`)
is set and no Power-On Reset was detected at the same time.
In this case it returns `true`, otherwise `false`.

### halt()
`halt` is a convenience method which prints a short message to `Serial`, turns
on the internal LED and calls `exit(0)`.

This method is useful for debugging to stop the Sketch at unexpected
conditions.

## Watchdog
The `class Watchdog` simplifies the use of the builtin Watchdog for Arduino
boards. When turned on, a given callback functions will be called from the
pre-implemented Interrupt Service Routine.

### WatchdogPrescalerValue
The possible Watchdog timeout values are defined in `enum WatchdogPrescalerValue`.

### WatchdogCallbackPtr
Definition of the prototyp for the expected callback functions which gets
called, if a Watchdog timeout occures.
```
typedef void (*WatchdogCallbackPtr)(uint32_t irqPC);
```
The callback function will be called with a single parameter. It contains
the program counter to where the Watchdog interrupted the Sketch when it fired.

### void watchdogOn(WatchdogPrescalerValue prescalerValue, WatchdogCallbackPtr watchdogCallbackFunc)
`watchdogOn` turns on the Watchdog of your Arduino board with the timeout
value given in `prescalerValue`. To avoid fireing of the Watchdog, you have to
call `watchdogReset` before the timeout is reached.

If the timeout is reached, the pre-implemented Interrupt Service Routine calls
the callback function which was given in `watchdogCallbackFunc`.
The program counter is passed to this function to give you a hint, where the
Sketch was interrupted by the Watchdog.

After completing the callback function, the Interrupt Service Routine will
call an endless loop. This pauses the Sketch until the second timeout of the
Watchdog fires, which then now resets the Sketch.

In combination with the methods of the class `System`, the `setup()` function
can determine the case of the restart and restore some important parameters which
could be saved to the EEPROM inside the callback function.
See the `Watchdog` example from this library.

### void watchdogOff()
`watchdogOff` turns off the Watchdog of your Arduino board.

### void watchdogReset()
`watchdogReset` resets the Watchdog timer. This is the keep-alive call to avoid
unwanted fireing of the Watchdog. Call this method in the `loop()` function
of your Sketch.

## SystemConfig
The `class SystemConfig` is an example class on how to save important
configuration data of your Sketch to the EEPROM of your Arduino board.
The configuration is protected by a checksum and a magic byte, which should
be unique for a Sketch. To change it, you have to modify `SystemConfig.cpp`.

A useful example can be found in the demo Sketch `Watchdog.ino` which is
included in this library.

# License

Copyright (c) 2020 by Andreas Trappmann.

MIT license, check LICENSE file for more information.

All text above must be included in any redistribution

# Release Notes:

Verison 1.1 - 11.03.2020

	* Added documentation.

Version 1.0 - 07.03.2020

	* Initial Version
