// NAME: ArduinoInfos.ino
// 
// DESC: Display some useful information about the current Arduino environment.
//
#include <TrappmannRobotics.h>
#include <TrappmannRobotics/System.h>

void setup() {
  /*
   * Get current program counter
   */
  uint32_t PC = TrappmannRobotics::getProgramCounter();
  
  // Reset System Status Register
  MCUSR = 0;
  
  /*
   * Say hello.
   */
  Serial.begin(115200);
  while (!Serial);
  Serial << F("----------------------------------\n");
  Serial << F("Sketch ") << getBaseName(__FILE__) << F(" running...\n");
  Serial << F("Uploaded: ") << TrappmannRobotics::getUploadTimestamp() << LF;

  /*
   * Show Program Counter.
   */
  int pointerSize;
#if defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560) 
  pointerSize = 6;
#else
  pointerSize = 4;
#endif
  Serial << F("Address of setup() function = 0x") << toHexString(((uint32_t)setup) << 1).substring(8-pointerSize) << LF;
  Serial << F("Program Counter for setup() = 0x") << toHexString(PC).substring(8-pointerSize) << LF;
  
  /*
   * Show Arduino board type.
   */
  showArduinoBoardType();

  /*
   * Show CPU signature.
   */
  showSignature();
  
  /*
   * CPU information.
   */
  showCPUInfo();
  
  /*
   * Show some standard sizes.
   */
  Serial << F("sizeof(char)   = ") << sizeof(char) << LF;
  Serial << F("sizeof(int)    = ") << sizeof(int) << LF;
  Serial << F("sizeof(long)   = ") << sizeof(long) << LF;
  Serial << F("sizeof(float)  = ") << sizeof(float) << LF;
  Serial << F("sizeof(double) = ") << sizeof(double) << LF;
  Serial << F("sizeof(void*)  = ") << sizeof(void*) << LF;

  /*
   * Show byte order
   */
  uint16_t value = 0x1234;
  uint8_t *p = (uint8_t*)&value;
  Serial << F("byte-order: ");
  if (0x12 == *p) { // high-byte first
    Serial << F("high-byte first\n");
  }
  else if (0x34 == *p) { // low-byte first
    Serial << F("low-byte first\n");
  }
  else { // unknown byte-order
    Serial << F("UNKNOWN!?\n");
  }
  
  /*
   * Show amount of free memory.
   */
  Serial << F("free memory = ") << TrappmannRobotics::getFreeMemory() << F(" Bytes\n");

  /*
   * Check if the MCU Status Register gets passed to the sketch.
   */
  Serial << F("Resetflags = %") << toBinaryString(System::getResetFlags()) << LF;
  
  if (System::hasValidResetFlags()) {
    Serial << F("Resetflags available for sketch!\n");
    if (System::isResetByPowerOn())   Serial << F("\tPower-On Reset Flag is set\n");
    if (System::isResetByExtern())    Serial << F("\tExternal Reset Flag is set\n");
    if (System::isResetByBrownOut())  Serial << F("\tBrown-out Reset Flag is set\n");
    if (System::isResetByWatchdog())  Serial << F("\tWatchdog System Reset Flag is set\n");
#if defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560) 
    if (System::isResetByJTAG())      Serial << F("\tJTAG Reset Flag\n");
#endif
  }
  else {
    Serial << F("*** WARNING: No valid Resetflags available for the sketch!\n" \
                "\tThe bootloader did not pass the MCU Status Register in r2.\n" \
                "\tConsider a bootloader update from https://github.com/Optiboot/optiboot\n");
   }

  /*
   * Show fuse bytes.
   */
  showFuseBytes();

  /*
   * Show useful hints for debugging.
   */
  Serial << F("Disassemble sketch.hex file: avr-objdump -D -m avr <sketch.ino.hex> >output.dump\n");
  Serial << F("Dump sketch from Arduino   : avrdude -CC:<idepath>\\hardware\\tools\\avr\\etc\\avrdude.conf -pm328p -carduino -PCOM10 -b115200 -D -Uflash:r:<output>.hex:i\n");
  Serial << F("Set Hi-Fuse over ArduinoISP: avrdude -CC:<idepath>\\hardware\\tools\\avr\\etc\\avrdude.conf -pm328p -cstk500v1 -PCOM10 -b19200 -U hfuse:w:0xde:m\n");
  
  /*
   * Done.
   */
  Serial << F("Internal LED blinking now...\n");
  pinMode(LED_PIN, OUTPUT);
}

/*
 * Blink in 1s intervals.
 */
void loop() {
  static bool state = false;
  if (state) {
    digitalWrite(LED_PIN, LOW);
  }
  else digitalWrite(LED_PIN, HIGH);
  state = !state;
  delay(1000);
}

/*
 * Show Arduino board infomation.
 */
void showArduinoBoardType() {
  Serial << F("Arduino board type = ")
#if defined(ARDUINO_AVR_UNO)
         << F("Uno")
#elif defined(ARDUINO_AVR_MEGA)
         << F("Mega")
#elif defined(ARDUINO_AVR_MEGA2560)
         << F("Mega2560")
#elif defined(ARDUINO_AVR_NANO)
         << F("Nano")         
#elif defined(ARDUINO_AVR_MICRO)
         << F("Micro")         
#elif defined(ARDUINO_AVR_PRO)
         << F("Pro")         
#elif defined(ARDUINO_AVR_LEONARDO)
         << F("LEONARDO")         
#else
         << F("Unknown - check boards.txt")         
#endif
         << LF;            
}

/*
 * Read CPU signature.
 */
void showSignature() {
  uint8_t sig1, sig2, sig3;
  uint8_t tempByte; 
  __asm__ __volatile__ (
    ".equ SPMCSR, 0x37        ; register address for SPMCSR\n\t"
    
    "ldi ZH, 0x00             ; load Z-Pointer with 0x0000\n\t"
    "ldi ZL, 0x00\n\t"
    
    "; Register SPMCSR - Store Program Memory Control and Status Register\n\t"
    "; SPMIE RWWSB SIGRD RWWSRE BLBSET PGWRT PGERS SELFPRGEN\n\t"
    "ldi %[TEMP], 0b00100001  ; set SIGRD and SPMEN bits in SPMCSR\n\t"
 
    "out SPMCSR, %[TEMP]\n\t"
    "lpm %[SIG1], Z+           ; load signature byte 1\n\t"
    "inc ZL\n"
    "nop\n"
    "nop\n"
    "out SPMCSR, %[TEMP]\n\t"
    "lpm %[SIG2], Z+           ; load signature byte 2\n\t"
    "inc ZL\n"
    "nop\n"
    "nop\n"
    "out SPMCSR, %[TEMP]\n\t"
    "lpm %[SIG3], Z            ; load signature byte 3\n\t"   
    : /* output */
      [SIG1]  "=&d" (sig1),
      [SIG2]  "=&d" (sig2),
      [SIG3]  "=&d" (sig3)
    : /* input */
      [TEMP] "d"  (tempByte)
  );  

  Serial << F("CPU signature bytes: ") << toHexString(sig1) << " " << toHexString(sig2) << " " << toHexString(sig3) << " = ";
  if (0x1E == sig1) {
    if ((0x94 == sig2) && (0x06 == sig3)) {
      Serial << F("ATmega168\n");
    }
    else if ((0x95 == sig2) && (0x0F == sig3)) {
      Serial << F("ATmega328p\n");
    }
    else if ((0x97 == sig2) && (0x03 == sig3)) {
      Serial << F("ATmega1280\n");
    }    
    else if ((0x98 == sig2) && (0x01 == sig3)) {
      Serial << F("ATmega2560\n");
    }
    else Serial << F("UNKNOWN!?\n");
  }
  else Serial << F("UNKNOWN! - Not an AVR processor?\n");
}

/*
 * Show CPU informations.
 */
void showCPUInfo() {
  Serial << F("CPU type = ")
#if defined(__AVR_ATmega168__)  
         << F("ATmega168")
#elif defined(__AVR_ATmega328P__)  
         << F("ATmega328P")
#elif defined(__AVR_ATmega1280__)  
         << F("ATmega1280")
#elif defined(__AVR_ATmega2560__)  
         << F("ATmega2560")
#elif defined(__AVR_ATmega32u4__)  
         << F("ATmega32u4")
#else
         << F("Unknown - check boards.txt")
#endif
         << LF;
                  
  Serial << F("CPU clock = ") << (F_CPU / 1000000L) << F("MHz\n");  
}

/*
 * Show Fuse Bytes.
 */
void showFuseBytes() {
  uint8_t register lowFuseByte;  // 0x0000
  uint8_t register lockBits;     // 0x0001
  uint8_t register extFuseByte;  // 0x0002
  uint8_t register highFuseByte; // 0x0003
  uint8_t register tempByte;
  __asm__ __volatile__ (
    ".equ SPMCSR, 0x37        ; register address for SPMCSR\n\t"
    
    "ldi ZH, 0x00             ; load Z-Pointer with 0x0000\n\t"
    "ldi ZL, 0x00\n\t"
    
    "; Register SPMCSR - Store Program Memory Control and Status Register\n\t"
    "; SPMIE RWWSB - RWWSRE BLBSET PGWRT PGERS SELFPRGEN\n\t"
    "ldi %[TEMP], 0b00001001  ; set BLBSET and SELFPRGEN bits in SPMCSR\n\t"
    
    "out SPMCSR, %[TEMP]\n\t"
    "lpm %[LFB], Z+           ; load Fuse low byte\n\t"

    "out SPMCSR, %[TEMP]\n\t"
    "lpm %[LB], Z+            ; load Lock Bits\n\t"

    "out SPMCSR, %[TEMP]\n\t"
    "lpm %[EFB], Z+           ; load Extended Fuse byte\n\t"
    
    "out SPMCSR, %[TEMP]\n\t"
    "lpm %[HFB], Z            ; load Fuse high byte\n\t"
    : /* output */
      [LFB] "=&d" (lowFuseByte),
      [LB]  "=&d" (lockBits),
      [EFB] "=&d" (extFuseByte),
      [HFB] "=&d" (highFuseByte)
    : /* input */
      [TEMP] "d"  (tempByte)
  );

  Serial << F("BL Lock Bits   = 0b") << toBinaryString(lockBits)     << F(" = 0x") << toHexString(lockBits) << LF;
  Serial << F("Fuse Low Byte  = 0b") << toBinaryString(lowFuseByte)  << F(" = 0x") << toHexString(lowFuseByte) << LF;
  Serial << F("Fuse High Byte = 0b") << toBinaryString(highFuseByte) << F(" = 0x") << toHexString(highFuseByte) << LF;
  Serial << F("Ext. Fuse Byte = 0b") << toBinaryString(extFuseByte)  << F(" = 0x") << toHexString(extFuseByte) << LF;
}

/*
 * Show bootloader version.
 * NOTE: Does not work from Application code section. Must be run from
 * Bootloader section only!
 */
#ifdef PROGRAM_MODE
void showBootloaderVersion(uint16_t addr) {
  String buffer = "000" + String(addr, HEX);
  Serial << F("Reading bootloader version from 0x") << buffer.substring(buffer.length()-4) << LF;
  
  uint8_t major;
  uint8_t minor;
  uint8_t rampz = ((addr & 0x10000)?1:0);
  uint8_t register tempByte;
  
  __asm__ __volatile__ (
    ".equ RAMPZ, 0x3b         ; RAMPZ register of ATmega2560\n\t"
    ".equ SPMCSR, 0x37        ; register address for SPMCSR\n\t"

    "ldi %[TEMP], 0b11101111  ; load r0 with bootloader lock bits\n\t"
    "mov r0, %[TEMP]\n\t"
    "clr r1\n\t"
    
    "ldi ZH, 0x00             ; load Z-Pointer with 0x0001 for compatibility\n\t"
    "ldi ZL, 0x01\n\t"
    
    "; Register SPMCSR - Store Program Memory Control and Status Register\n\t"
    "; SPMIE RWWSB - RWWSRE BLBSET PGWRT PGERS SELFPRGEN\n\t"
    "ldi %[TEMP], 0b00001001  ; set BLBSET and SELFPRGEN bits in SPMCSR\n\t"
    "out SPMCSR, %[TEMP]\n\t"

    "spm                      ; write bootloader lock bits\n\t"

    "clr r0                   ; restore r0\n\t"

    "nop\n"
    "nop\n"
    "mov ZL, %[RZ]           ; load bit 17 of address to RAMP_Z\n\t"
    "out RAMPZ, ZL\n\t"
    
    "mov ZH, %B3             ; load Z-Pointer with addr\n\t"
    "mov ZL, %A3\n\t"

    "; get version\n\t"
    "LPM %[MINOR], Z+        ; load version from data space\n\t"
    "LPM %[MAJOR], Z\n\t"
    : /* output */
      [MAJOR] "=&d" (major),
      [MINOR] "=&d" (minor)
    : /* input */
      [RZ]    "r"   (rampz),
              "r"   (addr),
      [TEMP]  "d"   (tempByte)
  );

  Serial << F("Bootloader version = ") << major << "." << minor << LF;
}
#endif
