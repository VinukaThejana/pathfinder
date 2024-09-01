#include <HardwareSerial.h>
#include "esp32-hal-gpio.h"
#include <Arduino.h>
#include <BluetoothSerial.h>
#include "lib/blink.h"
#include "lib/gps.h"

#define LED 2
#define RECEIVE_TASK_PRIORITY 2
#define SEND_TASK_PRIORITY 1
#define MAX_RECEIVE_LENGTH 100 // Bytes
#define SERIAL_REFRESH_RATE 9600
#define BLUETOOTH_NAME "ESP32"

// FreeRTOS Tasks
TaskHandle_t SendTask;
TaskHandle_t ReceiveTask;

BluetoothSerial SerialBT;
SemaphoreHandle_t mutex;

void send(void * parameter) {
    for (;;) {
        // TODO: Add the sensors here
    }
}

void receive(void * parameter) {
    char data[MAX_RECEIVE_LENGTH];
    for (;;) {
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

        // TODO: use the latitude and the longitude as needed
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
