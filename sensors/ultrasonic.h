/**
 * Sensor configuration
 *
 *   Ultrasonic Sensor
 *       - Echo = 12
 *       - Trig = 13
 *
 **/

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "esp32-hal-gpio.h"
#include "sensors.h"
#include <Arduino.h>

#define ECHO 12
#define TRIG 13

// speed of sound in cm/microsecond
#define SOUND_SPEED 0.034
// Sensor code provided for the ultrasonic sensor
#define SENSOR_CODE 0

class Ultrasonic {
private:
  static long duration;
  static float distance;

public:
  static void init() {
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    duration = 0;
    distance = 0;
  }

  static ErrCode read(String &value) {
    digitalWrite(TRIG, LOW);
    vTaskDelay(pdMS_TO_TICKS(2));
    digitalWrite(TRIG, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));
    digitalWrite(TRIG, LOW);

    duration = pulseIn(ECHO, HIGH);
    distance = duration * SOUND_SPEED / 2; // Distance in cm

    if (!value.isEmpty()) {
      value += "|";
    }

    value += String(SENSOR_CODE) + ":" + String(distance, 2);
    return ErrCode::SUCCESS;
  }
};

#endif // !ULTRASONIC_H

long Ultrasonic::duration = 0;
float Ultrasonic::distance = 0;
