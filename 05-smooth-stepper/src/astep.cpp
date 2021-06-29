# include "Arduino.h"
# include "astep.h"

#define MINUTE 60L * 1000L * 1000L

// setup configures the stepper's pins, inital direction and RPM.
void SmoothStepper::setup() {
    pinMode(pin_1, OUTPUT);
    pinMode(pin_2, OUTPUT);
    pinMode(pin_3, OUTPUT);
    pinMode(pin_4, OUTPUT);
    setRPMRange(min_rmp, max_rmp); // ensure we have a valid RPM and RPM Range
    setDir(direction);             // ensure we have a valid direction
}

// setDir sets the stepper direction.
void SmoothStepper::setDir(int dir) {
    if (dir != direction) {
        if (dir == DIR_CW) direction = DIR_CW;
        else               direction = DIR_CCW;
    }
}

// setRPM sets the rotations per minute of the stepper.
// This requires the stepper to have a correct maximum number of steps configured
// via the contructor.
void SmoothStepper::setRPM(int rpm) {
    if      (rpm < min_rmp) rpm = min_rmp; // cap to min RPM
    else if (rpm > max_rmp) rpm = max_rmp; // cap to max RPM
    step_delay_micros = MINUTE / motor_steps / max(1, rpm);
    this->rpm = rpm;
}

// setRPMRange sets and limits the steppers RPM range.
// Independent of the given values, the min_rpm cannot be below 1 and the
// max_rpm cannot be below the min_rpm.
void SmoothStepper::setRPMRange(int min_rpm, int max_rpm) {
    this->min_rmp = max(1, min_rmp);
    this->max_rmp = max(this->min_rmp, max_rmp);
    setRPM(rpm);  // reset RPM in case range changed
}

// step will either signal the stepper to move ONE step and return 1 or
// reject to signal the stepper if the previous phase is still active and return 0.
// The return value can be used to count the absolute number of steps stepped.
int SmoothStepper::step() {
    unsigned long now = micros();
    step_call_gap_micros = now - step_call_time_micros;  // allow tracking how often step is called
    step_call_time_micros = now;
    if (now - step_time_micros < step_delay_micros) {    // don't step if previous phase is not finished
        return 0;                                        // and return 0 to indicate a skipped step
    }
    step_gap_micros = now - step_time_micros;            // allow tracking how often the stepper steps
    step_time_micros = now;
    nextPhase();                                         // send the next phase signal to the controller
    return 1;                                            // and confirm the step to the caller.
}

// turn steps num_steps in the current direction and will block the execution until finished.
// It returns the absolute number of steps stepped, i.e. the original value of @steps.
// @param num_steps absolute number of steps to step.
int SmoothStepper::turn(unsigned int num_steps) {
    if (num_steps > 0) {
        int i = num_steps;
        while (i > 0) i = i - step();  // will block execution until finished

        // to ensure the last step is also stepped, we need to "await" the last steps signal
        unsigned long now = micros();
        unsigned long end = step_time_micros + step_delay_micros;
        if (now < end) delayMicroseconds(end - now);
    }
    return num_steps;
}

// stop turns off all controller pins.
// After finishing your movement, always call stop to avoid heating up the stepper!
void SmoothStepper::stop(){
    if (active) {
        digitalWrite(pin_1, LOW);
        digitalWrite(pin_2, LOW);
        digitalWrite(pin_3, LOW);
        digitalWrite(pin_4, LOW);
        active = false;
    }
}

// nextPhase advances the stepper signal to the next signal phase.
void SmoothStepper::nextPhase() {
    int phase = this->phase;
    if (direction == DIR_CW) {
        if (phase == PHASE_3) phase = PHASE_0;
        else                  phase++;
    } else {
        if (phase == PHASE_0) phase = PHASE_3;
        else                  phase--;
    }
    this->phase = phase;
    if (!active) active = true;
    runPhase(phase);
}

void SmoothStepper::runPhase(int phase) {
    switch (phase) {
    case PHASE_0:  // 1010
        digitalWrite(pin_1, HIGH);
        digitalWrite(pin_2, LOW);
        digitalWrite(pin_3, HIGH);
        digitalWrite(pin_4, LOW);
        break;
    case PHASE_1:  // 0110
        digitalWrite(pin_1, LOW);
        digitalWrite(pin_2, HIGH);
        digitalWrite(pin_3, HIGH);
        digitalWrite(pin_4, LOW);
        break;
    case PHASE_2:  //0101
        digitalWrite(pin_1, LOW);
        digitalWrite(pin_2, HIGH);
        digitalWrite(pin_3, LOW);
        digitalWrite(pin_4, HIGH);
        break;
    case PHASE_3:  //1001
        digitalWrite(pin_1, HIGH);
        digitalWrite(pin_2, LOW);
        digitalWrite(pin_3, LOW);
        digitalWrite(pin_4, HIGH);
        break;
    default:
        Serial.print("invalid phase:");
        Serial.println(phase);
    }
}
