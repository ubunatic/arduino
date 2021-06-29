#include "Arduino.h"
#include "song.h"

#define LED_13 13  // built-in LED at pin 13
#define LED_4 4
#define BUT_3 3
#define BUZ_12 12
#define TOGGLE_DELAY_MS 300

bool on_off_prev = false;
volatile bool on_off = false;
volatile unsigned long clicks = 0;
volatile unsigned long toggles = 0;
volatile unsigned long last_clicked_ms = 0;

// toggle is an Interrupt Service Routine (ISR) that changes the on_off state of the program.
// If you extend this, please adhere to the rules: http://gammon.com.au/interrupts.
// Also read here on how to avoid data races: https://www.stderr.nl/Blog/Hardware/Electronics/Arduino/Sleeping.html
//
// Learnings
// ---------
//
// * Interrupts are disabled inside ISRs.
// * Single byte-values are safe, multi-byte values are not and need to be protected.
// * `noInterrupts()` and `interrupts()` allow for easy protection outside of ISRs.
// * You can read `millis()` inside ISRs but the value will not change.
//
void buttonPressedISR() {
    clicks++;
    unsigned long now = millis();
    if (last_clicked_ms + TOGGLE_DELAY_MS < now) {
        on_off = !on_off;
        toggles++;
        last_clicked_ms = now;
    }
}

// printStatus safely reads global state and prints it to the serial console.
void printStatus() {

    noInterrupts();
    bool state = on_off;
    unsigned long time_ms = millis();
    unsigned long dur_last_click_ms = time_ms - last_clicked_ms;
    interrupts();

    Serial.print("on_off="); Serial.print(state);
    Serial.print(", clicks="); Serial.print(clicks);
    Serial.print(", toggles="); Serial.print(toggles);
    Serial.print(", time_ms="); Serial.print(time_ms);
    Serial.print(", dur_last_click_ms="); Serial.println(dur_last_click_ms);
}

bool isOn() {
    // noInterrupts();
    bool state = on_off;
    // interrupts();
    return state;
}

void setup()
{
    pinMode(BUT_3, INPUT_PULLUP);  // hardware button to switch of music
    attachInterrupt(digitalPinToInterrupt(BUT_3), buttonPressedISR, RISING);

    pinMode(LED_13, OUTPUT); // on-board debug LED
    pinMode(BUZ_12, OUTPUT);  // the actual "singer"
    pinMode(LED_4, OUTPUT);  // the "singing" LED

    Serial.begin(9600);
    Serial.println("setup done");
    delay(1000);
    Song.begin(BUZ_12, LED_4);
    Serial.println("starting playlist");
    on_off = true;
}

void pause(int delay_ms) {
    digitalWrite(LED_13, LOW);  // turn off LED_13 to indicate sleep
    delay(delay_ms);
    digitalWrite(LED_13, HIGH);  // turn on LED_13 to show activity
}

void klick_sound(int buz, int led) {
    digitalWrite(led, HIGH);
    digitalWrite(buz, HIGH);
    pause(100);
    digitalWrite(led, LOW);
    digitalWrite(buz, LOW);
    pause(100);
}

void loop() {
    bool on = isOn();
    if (on != on_off_prev) {
        on_off_prev = on;
        klick_sound(BUZ_12, LED_4);
        printStatus();
        if (on) Song.load(0);
        else    Song.stop();
    }
    if (on) {
        digitalWrite(LED_13, HIGH);
    } else {
        digitalWrite(LED_13, LOW);
    }

    // try to play and directly return to allow for fluent playback.
    if (on) Song.next();
    else delay(10);
    // otherwise add some delay to allow for interrupts to be triggered.
}
