// NAME: NewSketch.ino
//
// DESC: Basic template for a new Sketch.
//
// CONNECTIONS: 
//  Arduino Dx -> XX
//
// Copyright (c) 2020 by Andreas Trappmann
// All rights reserved.
//

#include <TrappmannRobotics.h>
#include <TrappmannRobotics/System.h>

void setup() {
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(115200);
  while (!Serial);
  Serial << F("------------------------------\n");
  Serial << F("Sketch: ") << getBaseName(__FILE__) << LF;
  Serial << F("Uploaded: ") << TrappmannRobotics::getUploadTimestamp() << LF;
  Serial << F("Free Memory: ") << TrappmannRobotics::getFreeMemory() << F(" bytes\n");
  Serial << F("Reset Flags: 0b") << toBinaryString(System::getResetFlags()) << LF;
  Serial << F("Running...\n");
}

void loop() {
  static bool state = false;
  if (state) {
    digitalWrite(LED_PIN, LOW);
  }
  else digitalWrite(LED_PIN, HIGH);
  state = !state;
  delay(1000);
}
