#include "esp32-hal-gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <HardwareSerial.h>

#include "lib/blink.h"
#include "lib/defer.h"
#include "lib/gps.h"

#include "sensors/compass.h"
#include "sensors/gyro.h"
#include "sensors/io.h"
#include "sensors/sensors.h"

#define RECEIVE_TASK_PRIORITY 2
#define SEND_TASK_PRIORITY 1
#define MAX_RECEIVE_LENGTH 100 // Bytes
#define SERIAL_REFRESH_RATE 9600
#define BLUETOOTH_NAME "ESP32"

// Name and path of the file that is used to save the coordinates
#define COORD_FILE "/coord"
// Keyword that is used to get the saved coordinates from the SD card
#define COORD_GET "get"
// Keyword that is used to clear the saved coordinates from the SD card
#define COORD_CLEAR "del"
// Err sent when failed to get the  coordinates from the SD card
#define COORD_GET_FAILED "lat:404|lon:404|"

// Builtin LED for debugging
#define LED 2

// FreeRTOS Tasks
TaskHandle_t SendTask;
TaskHandle_t ReceiveTask;

BluetoothSerial SerialBT;
SemaphoreHandle_t mutex;

void send(void *parameter) {
  String result = "";
  ErrCode err;

  for (;;) {
    err = Gyro::read(result);
    if (err != ErrCode::SUCCESS) {
      xSemaphoreTake(mutex, portMAX_DELAY);
      Serial.println("failed to read from gyro sensor");
      blink(LED);
      xSemaphoreGive(mutex);
    }

    err = Compass::read(result);
    if (err != ErrCode::SUCCESS) {
      xSemaphoreTake(mutex, portMAX_DELAY);
      Serial.println("failed to read from compass sensor");
      blink(LED);
      xSemaphoreGive(mutex);
    }

    result += "|";

    // NOTE: send the readings obtained from the sensors to the user
    SerialBT.println(result);

    result = "";
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void receive(void *parameter) {
  char data[MAX_RECEIVE_LENGTH];
  for (;;) {
    Defer d([] { vTaskDelay(pdMS_TO_TICKS(10)); });

    if (!SerialBT.available()) {
      continue;
    }

    size_t len = SerialBT.readBytesUntil('\n', data, MAX_RECEIVE_LENGTH - 1);
    data[len] = '\0'; // specify the string end
    if (len <= 0) {
      continue;
    }
    String payload = String(data);
    payload.trim();

    // NOTE:
    // when the user send the keyword COORD_GET, if there is saved coordinates
    // then the saved coordinates will be sent back to the user in the format
    // given below,
    // 200|lat:latitude|lon:longitude
    // if there is no saved coordinates then the response will be
    // lat:404|lon:404
    if (payload.equalsIgnoreCase(COORD_GET)) {
      String coords;
      ErrCode err;

      xSemaphoreTake(mutex, portMAX_DELAY);
      err = IO::read(COORD_FILE, coords);
      xSemaphoreGive(mutex);
      if (err != ErrCode::SUCCESS) {
        if (err == ErrCode::READ_FAILED) {
          SerialBT.println("lat:404|lon:404");
          continue;
        }
        SerialBT.println(COORD_GET_FAILED);
        blink(LED);
        continue;
      }

      SerialBT.println(coords + "|");
      continue;
    }

    // NOTE:
    // when the user send the keyword COORD_CLEAR, if there is saved coordinates
    // then the saved coordinates will be deleted from the SD card
    // if there is no saved coordinates then the response will be 400,
    // if there is any other error then the response will be 500
    if (payload.equalsIgnoreCase(COORD_CLEAR)) {
      xSemaphoreTake(mutex, portMAX_DELAY);
      ErrCode err = IO::clear(COORD_FILE);
      xSemaphoreGive(mutex);
      if (err != ErrCode::SUCCESS) {
        if (err == ErrCode::FILE_OPEN_FAILED) {
          SerialBT.println("400");
          blink(LED);
          continue;
        }

        SerialBT.println("500");
        blink(LED);
        continue;
      }

      SerialBT.println("200");
      continue;
    }

    // NOTE:
    // when the user send the coordinates in the format given below,
    // lat:latitude|lon:longitude
    // then the coordinates will be saved in the SD card
    // if the coordinates are saved without any error then the response will be
    // 200
    // if the coordinates are not valid latitudes and longitudes,
    // then the response will be
    // 400
    // if there is any other error then the response will be
    // 500
    Coordinates coord = GPS::extract(payload);
    if (!coord.isValid) {
      SerialBT.println("400");
      xSemaphoreTake(mutex, portMAX_DELAY);
      Serial.println("unidentified coordinates");
      blink(LED);
      xSemaphoreGive(mutex);
      continue;
    }

    payload = "lat:" + String(coord.lat, 6) + "|lon:" + String(coord.lon, 6);
    ErrCode err;

    xSemaphoreTake(mutex, portMAX_DELAY);
    err = IO::write(COORD_FILE, payload.c_str());
    xSemaphoreGive(mutex);
    if (err != ErrCode::SUCCESS) {
      SerialBT.println("500");
      xSemaphoreTake(mutex, portMAX_DELAY);
      Serial.println("failed to save the coordinates");
      blink(LED);
      xSemaphoreGive(mutex);
      continue;
    }

    SerialBT.println("200");
  }
}

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(SERIAL_REFRESH_RATE);
  if (!SerialBT.begin(BLUETOOTH_NAME)) {
    for (;;) {
      Serial.println("bluetooth initialization failed");
      blink(LED);
    }
  }
  ErrCode err;

  err = IO::init();
  if (err != ErrCode::SUCCESS) {
    for (;;) {
      Serial.println("SD card initialization failed");
      blink(LED);
    }
  }
  err = Gyro::init();
  if (err != ErrCode::SUCCESS) {
    for (;;) {
      Serial.println("gyro initialization failed, check the wiring");
      blink(LED);
    }
  }

  err = Compass::init();
  if (err != ErrCode::SUCCESS) {
    for (;;) {
      Serial.println("compass initialization failed, check the wiring");
      blink(LED);
    }
  }

  mutex = xSemaphoreCreateMutex();
  if (mutex == NULL) {
    Serial.println("mutex creation failed");
    blink(LED);
    return;
  }

  xTaskCreatePinnedToCore(send, "send", 10000, NULL, 1, &SendTask, 0);

  xTaskCreatePinnedToCore(receive, "receive", 10000, NULL, 1, &ReceiveTask, 1);

  if (SendTask == NULL || ReceiveTask == NULL) {
    Serial.println("task creation failed");
    blink(LED);
    return;
  }
}

void loop() {}
