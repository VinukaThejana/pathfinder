#ifndef BLINK
#define BLINK

#include "esp32-hal-gpio.h"
#include "esp32-hal.h"
#include "vtask.h"
#include <Arduino.h>

// Number of blinks
#define BLINK_DURATION 5
// The interval between blinks in miliseconds
#define BLINK_DELAY 200

inline void blink(
    int pin,
    int blink_duration = BLINK_DURATION,
    int blink_delay = BLINK_DELAY
) {
    for (int i = 1; i <= blink_duration; i++) {
        digitalWrite(pin, HIGH);
        delay(blink_delay);
        digitalWrite(pin, LOW);
        delay(blink_delay);
    }
}

inline void vblink(
    int pin,
    int blink_duration = BLINK_DURATION,
    int blink_delay = BLINK_DELAY
) {
    for (int i = 1; i <= blink_duration; i++) {
        digitalWrite(pin, HIGH);
        vtaskdelay(blink_delay);
        digitalWrite(pin, LOW);
        vtaskdelay(blink_delay);
    }
}


#endif // !BLINK
