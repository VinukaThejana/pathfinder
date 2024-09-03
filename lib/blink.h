#ifndef BLINK
#define BLINK

#include <Arduino.h>
#include "esp32-hal-gpio.h"

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
        vTaskDelay(pdMS_TO_TICKS(blink_delay));
        digitalWrite(pin, LOW);
        vTaskDelay(pdMS_TO_TICKS(blink_delay));
    }
}

#endif // !BLINK
