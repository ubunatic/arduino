#include "Arduino.h"
#include "buzz.h"

// playNote plays a note of a specific frequency
void playNote(int buz, int led, int freq, int tempo) {
    // The tempo is the length of a note as a fraction of one second, e.g.,
    // a value of 12 means 1/12 of a second, 3 means 1/3 of a second, etc.
    // A 1/3 note will play the sound for 0.33s and then add a pause of equal length.
    // Higher tempo values lead to lower delay.
    int dur_ms = SECOND_MS / tempo;
    buzz(buz, led, freq, dur_ms);
    delay(dur_ms);
}

// buzz swings the buzzers membrane to produce a sound of a specific frequency and length.
void buzz(int buz, int led, long freq, long dur_ms) {
    if (freq == 0) {
        // 0 Hz means silence
        delay(dur_ms);
        return;
    }

    digitalWrite(led, HIGH);

    long pulse_width = 1000000 / freq / 2;  // half-pulse width used for high and low flank
    long pulses = freq * dur_ms / 1000;     // how often to pulse within the total duration

    for (long i = 0; i < pulses; i++) {
        digitalWrite(buz, HIGH);            // push out the speaker diaphragm
        delayMicroseconds(pulse_width);     // wait half a pulse
        digitalWrite(buz, LOW);             // pull back the diaphragm
        delayMicroseconds(pulse_width);     // wait half a pulse
    }

    digitalWrite(led, LOW);
}
