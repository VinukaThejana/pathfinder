#include <Arduino.h>
#include <BluetoothSerial.h>
#include <HardwareSerial.h>
#include "esp32-hal-gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lib/blink.h"
#include "lib/defer.h"
#include "lib/gps.h"

#include "sensors/io.h"
#include "sensors/sensors.h"
#include "sensors/ultrasonic.h"

#define RECEIVE_TASK_PRIORITY 2
#define SEND_TASK_PRIORITY 1
#define MAX_RECEIVE_LENGTH 100 // Bytes
#define SERIAL_REFRESH_RATE 9600
#define BLUETOOTH_NAME "ESP32"

// Name and path of the file that is used to save the coordinates
#define COORD_FILE "/coord"
// Keyword that is used to get the saved coordinates from the SD card
#define COORD_GET "give"
// Err sent when failed to get the  coordinates from the SD card
#define COORD_GET_FAILED "err_failed_to_get_saved_coordinates"

// Builtin LED for debugging
#define LED 2

// FreeRTOS Tasks
TaskHandle_t SendTask;
TaskHandle_t ReceiveTask;

BluetoothSerial SerialBT;
SemaphoreHandle_t mutex;

void send(void * parameter) {
    String result = "";
    ErrCode err;

    for (;;) {
        err = Ultrasonic::read(result);
        if (err != ErrCode::SUCCESS) {
            xSemaphoreTake(mutex, portMAX_DELAY);
            Serial.println(
                "failed to read from ultrasonic sensor"
            );
            blink(LED);
            xSemaphoreGive(mutex);
        }

        SerialBT.println(result);
        result = "";
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
        payload.trim();

        if (payload.equalsIgnoreCase(COORD_GET)) {
            String coords;
            ErrCode err;

            xSemaphoreTake(mutex, portMAX_DELAY);
            err = IO::read(COORD_FILE, coords);
            xSemaphoreGive(mutex);
            if (err != ErrCode::SUCCESS) {
                SerialBT.println(COORD_GET_FAILED);
                blink(LED);
                continue;
            }

            SerialBT.println(coords);
            continue;
        }

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
        ErrCode err;

        xSemaphoreTake(mutex, portMAX_DELAY);
        err = IO::write(COORD_FILE, payload.c_str());
        xSemaphoreGive(mutex);
        if (err != ErrCode::SUCCESS) {
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
    ErrCode err = IO::init();
    if (err != ErrCode::SUCCESS) {
        for (;;) {
            Serial.println(
                "SD card initialization failed"
            );
            blink(LED);
        }
    }
    Ultrasonic::init();

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
