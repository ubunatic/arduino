#include "Arduino.h"
#include "rgb.h"

void RgbLed::rgbLow(int red, int green, int blue) {
    analogWrite(pin_1, 255 - red);
    analogWrite(pin_2, 255 - green);
    analogWrite(pin_3, 255 - blue);
}

void RgbLed::rgbHigh(int red, int green, int blue) {
    analogWrite(pin_1, red);
    analogWrite(pin_2, green);
    analogWrite(pin_3, blue);
}

void RgbLed::setup() {
    pinMode(pin_1, OUTPUT);
    pinMode(pin_2, OUTPUT);
    pinMode(pin_3, OUTPUT);
}

// pulse turns the LED luminance from 0 to 64 and back for the given color (reg, green, or blue).
// This will block execution for about half a second.
void RgbLed::pulse(int color, int num_pulses) {
    int max_lum   = 64;
    int step_up   = max(max_lum / 16, 1);
    int step_down = -step_up;
    int step_ms   = 15;
    // total time is 16 * 2 * 15 ms = 480ms

    int lum = 0;
    int step = step_up;
    while (num_pulses > 0) {
        lum += step;
        if (lum <= 0)  { lum = 0;  step = step_up;   num_pulses--; }  // full pulse
        if (lum >= 64) { lum = 64; step = step_down; }                // half pulse
        switch (color) {
        case RGBLED_RED:   red(lum);   break;
        case RGBLED_GREEN: green(lum); break;
        case RGBLED_BLUE:  blue(lum);  break;
        }
        delay(step_ms);
    }
    off();
}
