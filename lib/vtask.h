#ifndef VTASK
#define VTASK

#include <Arduino.h>

inline void vtaskdelay(int d) {
    vTaskDelay(d / portTICK_PERIOD_MS);
}

#endif // !VTASK
