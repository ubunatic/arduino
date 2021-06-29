// Arduino Libs

#include "Arduino.h"
#include "Wire.h"
#include "IRremote.h"

// Local Libs

#include "funduino_ir.h"    // Funduino IR remote codes
#include "astep.h"          // non-blocking smooth tiny stepper
#include "SignalState.h"    // signal state management
#include "rgb.h"            // manage RGB LED
#include "metrics.h"        // basic loop time tracking
#include "debug.h"          // single debug macro, requires a print(text) function

// Used Pins

#define SYSTEM_LED_13 13    // LED at pin 13 (built-in LED)
#define RGB_LED_09     9    // LED at pin 9  (with PWM support)
#define RGB_LED_10    10    // LED at pin 10 (with PWM support)
#define RGB_LED_11    11    // LED at pin 11 (with PWM support)
#define IR_RECV_7      7    // IR receiver at pin 11

// Physical Parameters

#define STEPS_FULL       2048  // steps for a full rotation
#define REPEAT_RANGE  200000L  // defines how fast IR signals can be received (with some added buffer time)
#define IDLE_RANGE   1000000L  // After 1 second turn off the Motor

// Device Management

SmoothStepper Motor(STEPS_FULL, 3,5,4,6);  // stepper controller at pins 3,4,5,6
IRrecv Receiver(IR_RECV_7);                // IR receiver at pin 11
SigState State;                            // manage signal state
RgbLed Rgb(RGB_LED_09, RGB_LED_10, RGB_LED_11, RGBLED_COMMON_ANODE);
LoopMetrics Mx;                            // track execution time of critical loop parts

int steps = 0;
int max_steps = 0;
unsigned long last_moved_steps = 0;
unsigned long moved_steps = 0;

void setup()
{
    Serial.begin(9600);
    while (!Serial) delay(100);

    Serial.println("# starting stepper setup");
    Motor.setRPM(5);
    Receiver.begin(IR_RECV_7, ENABLE_LED_FEEDBACK, SYSTEM_LED_13);
    pinMode(SYSTEM_LED_13, OUTPUT);
    State.setIdleSignal(FDIR_UNSPECIFIED);
    State.setWaitingPeriod(REPEAT_RANGE);

    Rgb.setup();
    Rgb.pulse(RGBLED_GREEN, 1); // indicate system start finished
    Serial.println("# stepper setup finished");
}

void print() {
    // Receiver.printIRResultShort(&Serial);

    Serial.print("cmd: "); Serial.print(funduino_command(State.signal()));
    Serial.print(", state: "); Serial.print(State.stateName());
    Serial.print(", rec_gap: "); Serial.print(State.receiveGap());

    Serial.print(", steps: "); Serial.print(steps);
    Serial.print(", max_steps: "); Serial.print(max_steps);
    Serial.print(", call_gap: "); Serial.print(Motor.getCallGap());
    Serial.print(", step_gap: "); Serial.print(Motor.getStepGap());

    Serial.print(", max_lt: "); Serial.print(Mx.maxLoopTime());
    Serial.print(", avg_lt: "); Serial.print(Mx.avgLoopTime());
    Serial.print(", rpm: ");    Serial.print(Motor.getRPM());
    Serial.print(", dir: ");    Serial.print(Motor.dirName());
    Serial.print(", phase: ");  Serial.print(Motor.getPhase());

    Serial.println();
}

void print(const char *text) {
    Serial.print("msg: ");
    Serial.print(text);
    Serial.print(", ");
    print();
}

// stops the motor and returns the moved steps from the last movement.
int stop() {
    if (Motor.getActive()) {
        Motor.stop();
        Rgb.off();
        last_moved_steps = moved_steps;
        steps = 0;
        moved_steps = 0;
        return last_moved_steps;
    }
    return 0;
}

void step() {
    max_steps = max(steps, max_steps);
    if (steps > 0) {
        if(Motor.step()) {
            moved_steps++;
            steps--;
        }
    }
}

void idle() { State.setIdle(); }

void reset() {
    print("reset");
    stop();
    Mx.reset();
    steps = 0;
    max_steps = 0;
    last_moved_steps = 0;
    moved_steps = 0;
}

// run runs a command given a known command code.
void run(int command) {
    switch (command) {
    // common controls
    case FDIR_UNSPECIFIED:          break;
    case FDIR_UP:   Motor.incRPM(); break;
    case FDIR_DOWN: Motor.decRPM(); break;

    // function keys
    case FDIR_A: print("status");          break;
    case FDIR_B: print("status");          break;
    case FDIR_C: reset(); print("status"); break;
    case FDIR_X: stop();  print("stop");   break;

    // fixed step movement
    case FDIR_1: moved_steps += Motor.turn(1); break;
    case FDIR_2: moved_steps += Motor.turn(2); break;
    case FDIR_3: moved_steps += Motor.turn(3); break;
    case FDIR_4: moved_steps += Motor.turn(4); break;
    case FDIR_5: moved_steps += Motor.turn(5); break;
    case FDIR_6: moved_steps += Motor.turn(6); break;
    case FDIR_7: moved_steps += Motor.turn(7); break;
    case FDIR_8: moved_steps += Motor.turn(8); break;
    case FDIR_9: moved_steps += Motor.turn(9); break;

    default:
        Serial.print("invalid command: ");
        Serial.println(command);
        break;
    }
}

void loop()
{
    unsigned long loop_start = micros();

    // Advance IRstate

    if (Receiver.decode()) {
        State.next(Receiver.decodedIRData.command);
        Receiver.resume();
    } else {
        State.next();
    }

    int state = State.state();
    int signal = State.signal();

    // Handle idle state

    if (state == SIGSTATE_IDLE) {
        if (Motor.getActive()) {
            stop();
            Mx.observe(micros() - loop_start);  // record metrics before expensive print
            print("move finished");             // print status after every finished move
        } else {
            Mx.observe(micros() - loop_start);  // record metrics for idle loop
        }
        return;
    }

    // Handle movement

    if (steps < 0) {
        // this can happen when steps are modified by custom commands
        stepper_debug("WARNING: resetting steps after seeing negative steps");
        steps = 0;
    }

    int dir;

    switch (signal) {
    case FDIR_RIGHT: dir = DIR_CW;  Rgb.green(64); break;
    case FDIR_LEFT:  dir = DIR_CCW; Rgb.red(64);   break;
    default:         dir = DIR_UNSPECIFIED;        break;
    }

    // Process new and pending movement requests depending on the IR state

    if (dir != DIR_UNSPECIFIED) {
        switch (state) {
        case SIGSTATE_ACTIVE_REPEATING: // keep at least one steps queued
            stepper_debug("repeating");
            if (steps == 0) steps = 1;
            break;
        case SIGSTATE_ACTIVE:           // add one step from one key press
            Motor.setDir(dir);
            steps += 1;
            stepper_debug("active");
            break;
        case SIGSTATE_ACTIVE_WAITING:   // process pending steps (0 or 1)
            stepper_debug("wait");
            break;
        case SIGSTATE_IDLE:             // already handled at beginning of loop
            break;
        default:
            stepper_debug("bad state");
            break;
        }
        step();
        Mx.observe(micros() - loop_start);
        return;
    }

    // Process other controller commands (non-movement commands)

    print("control");
    run(signal);
    idle();
}
