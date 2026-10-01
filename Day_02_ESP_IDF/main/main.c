#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void hello_task(void *pvParameter) {
    int counter = 0;
    while (1) {
        counter += 1;
        printf("[2] Day 2: Hello from task (count = %d)\n", counter);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void) {
    xTaskCreate(hello_task, "hello_task", 2048, NULL, 5, NULL);
}