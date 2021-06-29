#include "Arduino.h"

#define LED_13 13  // built-in LED at pin 13
#define LED_07 7
#define LED_08 8

#define LOOP_TIME_MS 1000

unsigned int num_leds = 0;

void addLED(unsigned int pin)
{
    pinMode(pin, OUTPUT);
    num_leds++;
}

void setup()
{
    addLED(LED_13);
    addLED(LED_07);
    addLED(LED_08);
}

void flank(unsigned int pin, unsigned int delay_ms) {
    digitalWrite(pin, HIGH);
    delay(1 * delay_ms / 10 );
    digitalWrite(pin, LOW);
    delay(9 * delay_ms / 10);
}

void loop()
{
    unsigned int blink_time_ms = LOOP_TIME_MS / num_leds;
    flank(LED_13, blink_time_ms);
    flank(LED_07, blink_time_ms);
    flank(LED_08, blink_time_ms);
}
