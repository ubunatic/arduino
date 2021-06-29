/*
 * TwoSignals.ino
 *
 * Demonstrates basic signal state management using the SignalState library.
 *
 *  This file is part of Arduino-SignalState https://github.com/ubunatic/arduino/signalstate.
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

#include "SignalState.h"

// Our system supports three signals, idle + two key presses (or IR signals).
#define IDLE 0
#define SIG_A 1
#define SIG_B 2

// We read these signals via two digital input pins.
#define PIN_A 8
#define PIN_B 9

// We handle single signaks and repeated signals differently.
#define REPEAT true
#define NO_REPEAT false

// Helper function for less verbose print code.
#define print(msg, value) Serial.print(msg); Serial.print(value);

// State will manage our signal state for the input signals.
SigState State;

// In this demo we just count single signals and repeated signals to log them.
unsigned long count_a = 0;
unsigned long count_b = 0;

void setup() {
    Serial.begin(9600);
    // define how the program tells the State that no new signal was received
    State.setIdleSignal(IDLE);
    pinMode(PIN_A, INPUT);
    pinMode(PIN_B, INPUT);
}

int readSignal() {
    if      (digitalRead(PIN_A) == 1) return SIG_A;
    else if (digitalRead(PIN_B) == 1) return SIG_B;
    else                              return IDLE;
}

void processSignal(int signal, bool repeating) {
    if (repeating) {
        // Let's update counts if any key is repeated.
        // Do not print here, since this will trigger very often!
        switch (signal) {
        case SIG_A: count_a++; break;
        case SIG_B: count_b++; break;
        }
    } else {
        print("resetting count for signal=", signal);
        print(", old count_a=", count_a);
        print(", old count_b=", count_b);
        switch (signal) {
        case SIG_A: count_a = 0; break;
        case SIG_B: count_b = 0; break;
        }
        print(", new count_a=", count_a);
        print(", new count_b=", count_b);
        Serial.println();
    }
}

void loop() {
    // Read the next signal and update the signal state.
    State.next(readSignal());
    int state  = State.state();
    int signal = State.signal();

    switch (state) {
    case SIGSTATE_IDLE:             break; // do nothing
    case SIGSTATE_ACTIVE:           processSignal(signal, NO_REPEAT); break;
    case SIGSTATE_ACTIVE_WAITING:   break; // wait for next signal
    case SIGSTATE_ACTIVE_REPEATING: processSignal(signal, REPEAT); break;
    }
}
