#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void hello_task(void *pvParameters) {
    while (1) {
        printf("Hello from FreeRTOS!\n");
        vTaskDelay(pdMS_TO_TICKS(1000)); // delay for 1 second
    }
}

void app_main() {
    xTaskCreate(
        hello_task,     // Task function
        "HelloTask",    // Name
        2048,           // Stack size
        NULL,           // Parameters
        5,              // Priority
        NULL            // Task handle
    );
}