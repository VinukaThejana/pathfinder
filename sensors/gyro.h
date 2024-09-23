/**
 * Sensor configuration
 *
 *   Gyro Sensor
 *       - SCL = 26
 *       - SDA = 25
 *
 **/

#ifndef GYRO_H
#define GYRO_H

#include "sensors.h"
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <Wire.h>
#include <cmath>
#include <cstdlib>

#define SCL 26
#define SDA 25

#define MAX_RETRIES 3
#define RETRY_DELAY 50

#define PITCH_SENSOR_CODE 7
#define ROLL_SENSOR_CODE 8

class Gyro {
private:
  static Adafruit_ADXL345_Unified accel;
  static float pitch;
  static float roll;
  static float x;
  static float y;
  static float z;

public:
  static ErrCode init() {
    Wire.begin(SDA, SCL);
    vTaskDelay(pdMS_TO_TICKS(50));

    for (int attempts = 0; attempts < 3; attempts++) {
      if (accel.begin()) {
        // Set range to +-4g (can be adjusted)
        accel.setRange(ADXL345_RANGE_4_G);
        return ErrCode::SUCCESS;
      }
      vTaskDelay(pdMS_TO_TICKS(100));
    }

    return ErrCode::WIRING_FAILED;
  }

  static ErrCode read(String &value) {
    sensors_event_t event;
    accel.getEvent(&event);

    x = event.acceleration.x;
    y = event.acceleration.y;
    z = event.acceleration.z;

    pitch = atan2(-x, sqrt(y * y + z * z)) * 180.0 / PI;
    roll = atan2(y, z) * 180.0 / PI;

    if (!value.isEmpty()) {
      value += "|";
    }
    value += String(PITCH_SENSOR_CODE) + ":" + String(pitch, 2);
    value += "|";
    value += String(ROLL_SENSOR_CODE) + ":" + String(roll, 2);

    return ErrCode::SUCCESS;
  }
};

#endif // !GYRO_H

Adafruit_ADXL345_Unified Gyro::accel = Adafruit_ADXL345_Unified(12345);

float Gyro::pitch = 0;
float Gyro::roll = 0;

float Gyro::x = 0;
float Gyro::y = 0;
float Gyro::z = 0;
