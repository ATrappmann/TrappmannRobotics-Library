// NAME: StringHelper.cpp
//
// DESC: Helper functions for printing with the stream operator and to format
//       binary and hexadecimal numbers to a well formed string. 
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
#include <TrappmannRobotics_StringHelper.h>

String toBinaryString(const uint8_t value) {
  static String buffer;
  buffer = "0000000" + String(value, BIN);
  return buffer.substring(buffer.length()-8);
}

String toHexString(const uint8_t value) {
  static String buffer;
  buffer = "0" + String(value, HEX);
  return buffer.substring(buffer.length()-2);  
}

String toHexString(const uint16_t value) {
  static String buffer;
  buffer = "000" + String(value, HEX);
  return buffer.substring(buffer.length()-4);  
}

String toHexString(const uint32_t value) {
  static String buffer;
  buffer = "0000000" + String(value, HEX);
  return buffer.substring(buffer.length()-8);  
}

String getBaseName(const char *path) {
  const String pathName = path;
  int idx = pathName.lastIndexOf('\\');
  String baseName;
  if (idx > 0) {
	baseName = pathName.substring(idx+1);
  } else {
	baseName = pathName;
  }
  return baseName;
}
	
String getPathName(const char *path) {
  const String fullPathName = path;
  int idx = fullPathName.lastIndexOf('\\');
  String pathName;
  if (idx > 0) {
	pathName = fullPathName.substring(0, idx);
  } else {
	pathName = ".";
  }
  return pathName;
}
