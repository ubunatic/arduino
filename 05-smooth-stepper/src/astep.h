#define DIR_UNSPECIFIED  0
#define DIR_CW  1
#define DIR_CCW 2

#define PHASE_0 0
#define PHASE_1 1
#define PHASE_2 2
#define PHASE_3 3

#define MAX_SPEED_28BYJ_48 15 // 28BYJ-48 runs stable at 15 - 17 RPM

class SmoothStepper {
private:
    int pin_1;
    int pin_2;
    int pin_3;
    int pin_4;
    int motor_steps;
    int phase = PHASE_0;
    int direction = DIR_CW;
    int rpm = 1;
    int max_rmp = MAX_SPEED_28BYJ_48;
    int min_rmp = 1;
    bool active = false;
    unsigned long step_time_micros = 0;
    unsigned long step_gap_micros = 0;
    unsigned long step_delay_micros = 0;
    unsigned long step_call_gap_micros = 0;
    unsigned long step_call_time_micros = 0;
    void nextPhase();
    void runPhase(int phase);
    void setup();
public:
    inline SmoothStepper(int num_steps, int pin1, int pin2, int pin3, int pin4) {
        motor_steps = max(1, num_steps);
        pin_1 = pin1;
        pin_2 = pin2;
        pin_3 = pin3;
        pin_4 = pin4;
        setup();
    }
    ~SmoothStepper() {}
    int step();
    int turn(unsigned int steps);
    void stop();
    void setRPM(int rpm);
    void setRPMRange(int min_rpm, int max_rpm);
    void setDir(int dir);

    // increases RPM by 1 up to the max_rpm.
    void incRPM() { setRPM(rpm + 1); }
    // decreases RPM by 1 down to the min_rmp.
    void decRPM() { setRPM(rpm - 1); }

    int           getRPM()     { return rpm; }
    int           getDir()     { return direction; }
    unsigned long getCallGap() { return step_call_gap_micros; }
    unsigned long getStepGap() { return step_gap_micros; }
    int           getPhase()   { return phase; }
    bool          getActive()  { return active; }

    const char* dirName() {
        switch (direction) {
            case DIR_UNSPECIFIED: return "DIR_UNSPECIFIED";
            case DIR_CW:          return "DIR_CW";
            case DIR_CCW:         return "DIR_CCW";
            default:              return "UNKNOWN";
        }
    }
};
