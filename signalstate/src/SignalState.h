/**
 * @file SignalState.h
 *
 * @brief Public API of the Arduino-SignalState library.
 *
 * This file is part of Arduino-SignalState https://github.com/ubunatic/arduino/signalstate.
 *
 ************************************************************************************
 * MIT License
 *
 * Copyright (c) 2021 Uwe Jugel
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************************
 */

#ifndef SignalState_h
#define SignalState_h

#define VERSION_SIGNALSTATE "1.0.0"
#define VERSION_SIGNALSTATE_MAJOR 1
#define VERSION_SIGNALSTATE_MINOR 0

// #define DEBUG_SIGNALSTATE // Enable debug output from the SigState library.

#include "SigState.h"
/*
 * Include the sources here to enable compilation with macro values set by user program.
 */
#include "SigState.cpp.h"

#endif // SignalState_h
#pragma once
