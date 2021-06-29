
# Arduino-SignalState Library

Provides a `SigState` class to manage observed signals and keep track signal repetitions.
The following signal states are supported.

* `SIGSTATE_IDLE`:             start state and state after no signals were received within the wait period
* `SIGSTATE_ACTIVE`:           state directly after receiving a new signal (start of the wait period)
* `SIGSTATE_ACTIVE_WAITING`:   state when waiting for the first repeating signal within the wait period
* `SIGSTATE_ACTIVE_REPEATING`: state when a repeating signal was observed within the wait period.

When reaching the ACTIVE_REPEATING state, the signal state will not go back to ACTIVE or ACTIVE_WAITING
without a signal change. It will either stay ACTIVE_REPEATING when receiving the current signal
again or go IDLE after the wait period.

## Example: Managing an IRremote Input
```cpp
#include "SignalState.h"
#include "IRremote.h"

#define IR_INPUT_PIN 7
#define SYSTEM_LED 13

SigState State;
IRrecv Receiver(IR_INPUT_PIN);

void setup() {
    Receiver.begin(IR_INPUT_PIN, ENABLE_LED_FEEDBACK, SYSTEM_LED);
}

void loop() {
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
        default:                        debug("bad state"); break;
        }
    }
}
```

## Schematic: Exemplary State Changes
```
                  single          press X  hold X
                  press X              |     |  keep holding X
---------------.-----|-----.-----.-----|-----|-----|-----.-----.-
input signal   .  .  X  .  .  .  .  .  X  .  X  .  X  .  .  .  .
signal state   I  I  A  W  W  W  I  I  A  W  R  R  R  R  R  R  I
active signal  .  .  X  X  X  X  .  .  X  X  X  X  X  X  X  X  .
---------------.-----.-----.-----.-----.-----.-----.-----.-----.-
time           0.0   0.5   1.0   1.5   2.0   2.5   3.0   3.5  4.0
(in wait periods)

                press X     press X        press Y
                  |   press Y |   hold X     |   hold Y
        -.-----.--|--.--|--.--|--.--|--.-----|-----|-----.-----.-
         .  .  .  X  .  Y  .  X  .  X  .  .  Y  .  Y  .  .  .  .
         I  I  I  A  W  A  W  A  W  R  R  R  A  W  R  R  R  R  I
         .  .  .  X  X  Y  Y  X  X  X  X  X  Y  Y  Y  Y  Y  Y  .
        -.-----.-----.-----.-----.-----.-----.-----.-----.-----.-
         4.0   4.5   5.0   5.5   6.0   6.5   7.0   7.5   8.0  8.5
```
