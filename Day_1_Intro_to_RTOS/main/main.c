#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#define LED_PIN GPIO_NUM_2

void hello_task(void *pvParameters) {
    while (1) {
        printf("Hello from FreeRTOS!\n");
        vTaskDelay(pdMS_TO_TICKS(1000)); // delay for 1 second
    }
}

void blink_led(void *pvParameters) {
    while (1){
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main() {
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    xTaskCreate(
        blink_led,
        "Blink LED",
        2048,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        hello_task,     // Task function
        "HelloTask",    // Name
        2048,           // Stack size
        NULL,           // Parameters
        5,              // Priority
        NULL            // Task handle
    );
}