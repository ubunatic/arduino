/*
 * IRSignal.ino
 *
 * Demonstrates IR signal state management using the SignalState library.
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
#include "IRremote.h"

// our system supports three signals, idle + two key presses (or IR signals)
#define CMD_IDLE 0
#define CMD_LEFT 67
#define CMD_RIGHT 68

#define IR_INPUT_PIN 7
#define SYSTEM_LED 13

SigState State;
IRrecv Receiver(IR_INPUT_PIN);

void setup() {
    Serial.begin(9600);
    // define how the program tells the State that no new signal was received
    State.setIdleSignal(CMD_IDLE);
    Receiver.begin(IR_INPUT_PIN, ENABLE_LED_FEEDBACK, SYSTEM_LED);
}

void step(int command) {
    switch (command) {
    case CMD_LEFT:  /* start moving left */ break;
    case CMD_RIGHT: /* start moving right */ break;
    }
}

void move(int command) {
    switch (command) {
    case CMD_LEFT:  /* keep moving left */ break;
    case CMD_RIGHT: /* keep moving right */ break;
    }
}

void loop() {
    if (Receiver.decode()) {
        State.next(Receiver.decodedIRData.command);
        Receiver.resume();
    } else {
        State.next();
    }

    int state = State.state();
    int command = State.signal();

    if (command != CMD_IDLE) {
        switch (state) {
        case SIGSTATE_ACTIVE:           step(command); break;
        case SIGSTATE_ACTIVE_REPEATING: move(command); break;
        case SIGSTATE_ACTIVE_WAITING:   break; // wait for state change
        case SIGSTATE_IDLE:             break; // do nothing
        }
    }
}
