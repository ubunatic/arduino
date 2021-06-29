/**
 * @file SigState.cpp.h
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

#ifndef SigState_cpp_h
#define SigState_cpp_h

#include "SigState.h"

#ifdef DEBUG_SIGSTATE
#define sigstate_debug(text, value) Serial.print(text); Serial.println(value);
#else
#define sigstate_debug(text, value) void();
#endif

int SigState::next() {
    if (receiveGap() > wait_period) {
        setIdle();                                 // go idle after some time
    }
    else if (last_state == SIGSTATE_ACTIVE) {
        last_state = SIGSTATE_ACTIVE_WAITING;       // wait for repeat after first new signal
    }
    // else don't change SIGSTATE_ACTIVE_REPEATING  // keep repeating
    // else don't change SIGSTATE_ACTIVE_WAITING    // keep waiting until idle or next signal
    // else don't change SIGSTATE_IDLE              // stay idle

    return last_state;
}

int SigState::next(int input_signal) {
    sigstate_debug("cmd: ", input_signal);
    if(input_signal == idle_signal) return next();

    unsigned long now = micros();
    unsigned long receive_gap = now - last_receive;
    last_receive = now;
    if (input_signal == last_signal && receive_gap < wait_period) {
        last_state = SIGSTATE_ACTIVE_REPEATING;
    } else {
        last_signal = input_signal;
        last_state = SIGSTATE_ACTIVE;
    }

    return last_state;
}

void SigState::setIdle() {
    if (last_signal != idle_signal || last_state != SIGSTATE_IDLE) {
        sigstate_debug("going idle after signal: ", last_signal);
        sigstate_debug("going idle after state:", last_state);
        last_state = SIGSTATE_IDLE;
        last_signal = idle_signal;
    }
}

#endif // SigState_cpp_h
#pragma once
