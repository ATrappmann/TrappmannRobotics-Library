# TrappmannRobotics-Library

This is an Arduino library with a collection of helper functions I always use in 
my projects.

It contains methods to control the Watchdog and fire a given callback function.
This function could save important data to the EEPROM by using the SystemConfig class.
To check for a reboot reason, methods are available in the System class. To use
this class it may be necessary to upgrade the bootloader of your Arduino. The 
bootloaders I have used are included in the subdirectory. Copy these files to your
IDE installation and build them. The process of uploading a new bootloader is 
documented there. Check https://www.arduino.cc/en/hacking/bootloader for starters.

Written by Andreas Trappmann.
MIT license, check LICENSE for more information.
All text above must be included in any redistribution

Release Notes:

Version 1.0 - 07.03.2020

	* Initial Version
