#include <Arduino.h>

TaskHandle_t SendTask;
TaskHandle_t ReceiveTask;

void send(void * parameter) {
    for (;;) {

    }
}

void receive(void * parameter) {
    for (;;) {

    }
}

void setup() {
    xTaskCreatePinnedToCore(
        send, "send", 1000, NULL, 1, &SendTask, 0
    );

    xTaskCreatePinnedToCore(
        receive, "receive", 1000, NULL, 1, &ReceiveTask, 1
    );
}

void loop() {
}
