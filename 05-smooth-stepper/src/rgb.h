
#define RGBLED_RED   0
#define RGBLED_GREEN 1
#define RGBLED_BLUE  2

#define RGBLED_COMMON_ANODE 0
#define RGBLED_COMMON_CATHODE 1

class RgbLed {
private:
    int pin_1;
    int pin_2;
    int pin_3;
    bool common_anode = true;
    void rgbLow(int red, int green, int blue);
    void rgbHigh(int red, int green, int blue);
public:
    inline RgbLed(int pin1, int pin2, int pin3, int pin_config) : RgbLed(pin1, pin2, pin3) {
        if      (pin_config == RGBLED_COMMON_ANODE)   common_anode = true;
        else if (pin_config == RGBLED_COMMON_CATHODE) common_anode = false;
        // else ignore unknown values
    }
    inline RgbLed(int pin1, int pin2, int pin3) {
        pin_1 = pin1;
        pin_2 = pin2;
        pin_3 = pin3;
    }
    ~RgbLed() {}
    void setup();
    void rgb(int red, int green, int blue) {
        if (common_anode) rgbLow(red, green, blue);
        else              rgbHigh(red, green, blue);
    }
    void off()         { rgb(0, 0, 0); }
    void white(int v)  { rgb(v/3, v/3, v/3); }
    void red(int v)    { rgb(v, 0, 0); }
    void green(int v)  { rgb(0, v, 0); }
    void blue(int v)   { rgb(0, 0, v); }

    void pulse(int color, int num_pulses);
};
