/**
* Sensor configuration
*
*   SD Card reader
*       - CS   = 5
*       - MOSI = 23
*       - MISO = 19
*       - SCK  = 18
*
**/

#include <HardwareSerial.h>
#include <Arduino.h>
#include <BluetoothSerial.h>
#include "esp32-hal-gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lib/blink.h"
#include "lib/defer.h"
#include "lib/gps.h"
#include "lib/io.h"

#define RECEIVE_TASK_PRIORITY 2
#define SEND_TASK_PRIORITY 1
#define MAX_RECEIVE_LENGTH 100 // Bytes
#define SERIAL_REFRESH_RATE 9600
#define BLUETOOTH_NAME "ESP32"
#define COORD_FILE "/coord"

#define LED 2
#define SD_CS 5

// FreeRTOS Tasks
TaskHandle_t SendTask;
TaskHandle_t ReceiveTask;

BluetoothSerial SerialBT;
SemaphoreHandle_t mutex;

void send(void * parameter) {
    for (;;) {
        // TODO: Add the sensors here
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void receive(void * parameter) {
    char data[MAX_RECEIVE_LENGTH];
    for (;;) {
        Defer d([]{ vTaskDelay(pdMS_TO_TICKS(10)); });

        if (!SerialBT.available()) {
            continue;
        }

        size_t len = SerialBT.readBytesUntil('\n', data, MAX_RECEIVE_LENGTH - 1);
        data[len] = '\0'; // specify the string end
        if (len <= 0) {
            continue;
        }
        String payload = String(data);

        Coordinates coord = GPS::extract(payload);
        if (!coord.isValid) {
            xSemaphoreTake(mutex, portMAX_DELAY);
            Serial.println(
                "coordinates are not valid"
            );
            blink(LED);
            xSemaphoreGive(mutex);
            continue;
        }

        payload = "lat:" + String(coord.lat, 6) + "|lon:" + String(coord.lon, 6);
        IO::ErrCode err;

        xSemaphoreTake(mutex, portMAX_DELAY);
        err = IO::write(COORD_FILE, payload.c_str());
        xSemaphoreGive(mutex);
        if (err != IO::SUCCESS) {
            xSemaphoreTake(mutex, portMAX_DELAY);
            Serial.println(
                "failed to save the coordinates"
            );
            blink(LED);
            xSemaphoreGive(mutex);
            continue;
        }
    }
}

void setup() {
    pinMode(LED, OUTPUT);
    Serial.begin(SERIAL_REFRESH_RATE);

    if (!SerialBT.begin(BLUETOOTH_NAME)) {
        for (;;) {
            Serial.println(
                "bluetooth initialization failed"
            );
            blink(LED);
        }
    }
    if (!SD.begin(SD_CS)) {
        for (;;) {
            Serial.println(
                "SD card initialization failed"
            );
            blink(LED);
        }
    }

    mutex = xSemaphoreCreateMutex();
    if (mutex == NULL) {
        Serial.println(
            "mutex creation failed"
        );
        blink(LED);
        return;
    }

    xTaskCreatePinnedToCore(
        send, "send", 10000, NULL, 1, &SendTask, 0
    );

    xTaskCreatePinnedToCore(
        receive, "receive", 10000, NULL, 1, &ReceiveTask, 1
    );

    if (SendTask == NULL || ReceiveTask == NULL) {
        Serial.println(
            "task creation failed"
        );
        blink(LED);
        return;
    }
}


void loop() {
}
