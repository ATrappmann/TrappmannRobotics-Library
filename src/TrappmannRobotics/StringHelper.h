// NAME: StringHelper.h
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
#ifndef STRINGHELPER_H
#define STRINGHELPER_H

#include <WString.h>
#include <Print.h>

#define LF '\n'
#if !defined(TEENSYDUINO)
#define CR '\r'
#endif
#define NUL '\0'

// Printing with stream operator
template<class T> inline Print& operator <<(Print &obj, T arg) { obj.print(arg); return obj; }
//inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

// Hex and Binary printing
String toBinaryString(const uint8_t value);

String toHexString(const uint8_t value);
String toHexString(const uint16_t value);
String toHexString(const uint32_t value);
String toHexString(const void *ptr);

// Extract filename from path 
String getBaseName(const char *path);
String getPathName(const char *path);

#endif /* STRINGHELPER_H */