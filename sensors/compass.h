/**
 * Sensor configuration
 *
 *   Compass Sensor
 *       - SCL = 21
 *       - SDA = 22
 *
 **/

#ifndef COMPASS_H
#define COMPASS_H

#include "sensors.h"
#include <Adafruit_HMC5883_U.h>
#include <Arduino.h>
#include <Wire.h>
#include <cmath>
#include <cstdlib>

#define SCL 21
#define SDA 22

#define MAX_RETRIES 3
#define RETRY_DELAY 50

#define SENSOR_CODE 9

class Compass {
private:
  static Adafruit_HMC5883_Unified mag;
  static float x, y, z;
  static float heading;
  static float declination_angle;

public:
  static ErrCode init() {
    Wire.begin(SDA, SCL);
    vTaskDelay(pdMS_TO_TICKS(50));

    for (int attempts = 0; attempts < 3; attempts++) {
      if (mag.begin()) {
        return ErrCode::SUCCESS;
      }
      vTaskDelay(pdMS_TO_TICKS(100));
    }

    return ErrCode::WIRING_FAILED;
  }

  static ErrCode read(String &value) {
    sensors_event_t event;
    bool valid_reading = false;

    for (int attempts = 0; attempts < MAX_RETRIES && !valid_reading;
         attempts++) {
      if (mag.getEvent(&event)) {
        x = event.magnetic.x;
        y = event.magnetic.y;
        z = event.magnetic.z;

        if (abs(x) > 0.01 || abs(y) > 0.01 || abs(z) > 0.01) {
          valid_reading = true;

        } else {
          vTaskDelay(pdMS_TO_TICKS(RETRY_DELAY));
        }
      } else {
        vTaskDelay(pdMS_TO_TICKS(RETRY_DELAY));
      }
    }

    if (!valid_reading) {
      return ErrCode::INVALID_READING;
    }

    heading = atan2(y, x) * 180 / M_PI;
    if (heading < 0) {
      heading += 360;
    }

    if (!value.isEmpty()) {
      value += "|";
    }
    value += String(SENSOR_CODE) + ":" + String(heading, 2);
    return ErrCode::SUCCESS;
  }
};

#endif // !COMPASS_H

Adafruit_HMC5883_Unified Compass::mag = Adafruit_HMC5883_Unified(12345);

float Compass::x = 0.0;
float Compass::y = 0.0;
float Compass::z = 0.0;
float Compass::heading = 0.0;

// Find your declination here: http://www.magnetic-declination.com/
float Compass::declination_angle = 0.13;
