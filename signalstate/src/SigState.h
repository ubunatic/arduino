/**
 * @file SigState.h
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

#ifndef SigState_h
#define SigState_h

#include "Arduino.h"

#define SIGSTATE_IDLE 0
#define SIGSTATE_ACTIVE 1
#define SIGSTATE_ACTIVE_WAITING 2
#define SIGSTATE_ACTIVE_REPEATING 3

inline const char* sigStateName(int state) {
    switch (state) {
    case SIGSTATE_IDLE:             return "SIGSTATE_IDLE";
    case SIGSTATE_ACTIVE:           return "SIGSTATE_ACTIVE";
    case SIGSTATE_ACTIVE_WAITING:   return "SIGSTATE_ACTIVE_WAITING";
    case SIGSTATE_ACTIVE_REPEATING: return "SIGSTATE_ACTIVE_REPEATING";
    default:                       return "UNKNOWN";
    }
}

/*
SigState manages observed signals and keeps track of signal repetitions.
The following states are supported.

    SIGSTATE_IDLE                start state and state after no signals were received within the wait period
    SIGSTATE_ACTIVE              state directly after receiving a new signal (start of the wait period)
    SIGSTATE_ACTIVE_WAITING      state when waiting for the first repeating signal within the wait period
    SIGSTATE_ACTIVE_REPEATING    state when a repeating signal was observed within the wait period.

When reaching the ACTIVE_REPEATING state, the IR state will not go back to ACTIVE or ACTIVE_WAITING
without an IR signal change. It will either stay ACTIVE_REPEATING when receiving the current signal
again or go IDLE after the wait period.

Usage Example:

    // Using an IRrecv Receiver (from IRremote library v3.0).
    if (Receiver.decode()) {
        State.next(Receiver.decodedIRData.command);
        Receiver.resume();
    } else {
        State.next();
    }

    int state = State.state();
    int signal = State.signal();

    if (signal == MY_REPEATABLE_SIGNAL) {
        switch (state) {
        case SIGSTATE_ACTIVE:           startSomething(); break;
        case SIGSTATE_ACTIVE_REPEATING: keepGoing(); break;
        case SIGSTATE_ACTIVE_WAITING:   break; // wait for state change
        case SIGSTATE_IDLE:             break; // do nothing
        }
    }

Example signals and state changes:


                      single            press and                     press X, then Y            press and
                      press X           hold X                        then hold X                hold Y
    input signal   .  .  X  .  .  .  .  .  X  .  X  .  X  .  .  .  .  .  X  .  Y  .  X  .  X  .  .  Y  .  Y  .  .  .  .
    signal state   I  I  A  W  W  W  I  I  A  W  R  R  R  R  R  R  I  I  A  W  A  W  A  W  R  R  R  A  W  R  R  R  R  I
    active signal  .  .  X  X  X  X  .  .  X  X  X  X  X  X  X  X  .  .  X  X  Y  Y  X  X  X  X  X  Y  Y  Y  Y  Y  Y  .
    ---------------.-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.---
    time (in wait  0.0   0.5   1.0   1.5   2.0   2.5   3.0   3.5   4.0   4.5   5.0   5.5   6.0   6.5   7.0   7.5   8.0
          periods)

*/
class SigState {
private:
    // idle_signal is the signal code associated to unknown signals (default=0).
    // For instance, the IRremote library produces the command code `0` for unknown IR signals.
    int idle_signal = 0;
    // last_signal is the last observed or retained signal code
    int last_signal = 0;
    // last_state is the current IR state, according to the received signals and timeouts.
    int last_state = SIGSTATE_IDLE;
    // last_receive is the time in micros when the last IR signal was received.
    unsigned long last_receive = 0;
    // duration in micros until a signal is no longer considered as repeated.
    unsigned long wait_period = 200000L;
public:
    inline SigState() {}
    ~SigState() {}

    // next checks the current state and advances it if necessary.
    int next();
    // next consumes and processes the next input signal and updates the signal state accordingly.
    int next(int input_signal);

    // signal returns the current active signal according to the current state.
    int signal() { return last_signal; }

    // state returns the current state of the SigState.
    int state() { return last_state; }

    // stateName returns the name of the current state.
    const char* stateName() { return sigStateName(last_state); }

    // receiveGap returns the time since the last IR signal was received.
    unsigned long receiveGap()  { return micros() - last_receive; }
    // repeatRange returns the duration within which matching consecutive signals
    // are considered as repeated signals.
    unsigned long repeatRange() { return wait_period; }

    // setWaitingPeriod sets the duration within which matching consecutive signals
    // are considered as repeated signals.
    void setWaitingPeriod(unsigned long micros) { wait_period = micros; }
    // setIdleSignal sets the signal code observed by unknown signals.
    void setIdleSignal(int signal) { idle_signal = signal; }

    // setIdle sets the signal state to idle and resets the last observed signal.
    void setIdle();
};

#endif // SigState_h
#pragma once
