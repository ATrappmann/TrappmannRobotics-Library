// NAME: CallWithTimeout.ino
//

//#define DEBUG 1
#include <TrappmannRobotics.h>
#include <TrappmannRobotics/System.h>
#include <TrappmannRobotics/CallWithTimeout.h>

struct myArgs {
  int i;
};

struct myResults {
  int j;
};

void nonHangingFunction(myArgs *args, myResults *results) {
  Serial.print("nonHangingFunction(i="); Serial.print(args->i);
  Serial.print(", j="); Serial.print(results->j); Serial.println(")");
  
  results->j = 42 + args->i;
}

void hangingFunction(myArgs *args, myResults *results) {
  Serial.print("hangingFunction(i="); Serial.print(args->i);
  Serial.print(", j="); Serial.print(results->j); Serial.println(")");

  Serial.println("Simulate hanging...");
  delay(5000); // wait 5s
  
  results->j = 42 + args->i;
  Serial.println("hangingFunction done!??");
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial << F("------------------------------\n");
  Serial << F("Sketch: ") << getBaseName(__FILE__) << LF;
  Serial << F("Uploaded: ") << TrappmannRobotics::getUploadTimestamp() << LF;
  Serial << F("Free Memory: ") << TrappmannRobotics::getFreeMemory() << F(" bytes\n");
  Serial << F("Reset Flags: 0b") << toBinaryString(System::getResetFlags()) << ": ";
  System::printResetFlags(Serial);
  Serial << F("Running...\n");

  myArgs args;
  myResults results;
  bool rc;
  
  SEROUT(F("PC = 0x") << toHexString(TrappmannRobotics::getProgramCounter()) << LF);
  
  args.i = 1; results.j = 99;
  rc = callWithWatchdog(nonHangingFunction, &args, &results, WDTO_1s);
  Serial.print("callWithWatchdog returned: rc="); Serial.print(rc);
  Serial.print(", result="); Serial.print(results.j);
  Serial.print(" - "); Serial.println(rc ? "valid" : "invalid");
  Serial.println();
  
  args.i = 2; results.j = 98;
  rc = callWithWatchdog(hangingFunction, &args, &results, WDTO_2s);
  Serial.print("callWithWatchdog returned: rc="); Serial.print(rc);
  Serial.print(", result="); Serial.print(results.j);
  Serial.print(" - "); Serial.println(rc ? "valid" : "invalid (timeout!)");
  Serial.println();
  
  args.i = 3; results.j = 97;
  rc = callWithWatchdog(nonHangingFunction, &args, &results, WDTO_4s);
  Serial.print("callWithWatchdog returned: rc="); Serial.print(rc);
  Serial.print(", result="); Serial.print(results.j);
  Serial.print(" - "); Serial.println(rc ? "valid" : "invalid");
  Serial.println();

  Serial.println("Done.");
}

void loop() {
  // put your main code here, to run repeatedly:
}