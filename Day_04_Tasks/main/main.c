#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#define LED_PIN GPIO_NUM_2

TaskHandle_t task_handle_blink_led = NULL;

void blink_led_task(void *pvParameters) {
    int counter = 0;
    while (1) {
        gpio_set_level(LED_PIN, !gpio_get_level(LED_PIN));
        printf("Blink_led Task running, counter = %d\n", ++counter);
        vTaskDelay(pdMS_TO_TICKS(500));

        if (counter >= 20) {
            printf("Blink led Task self deletion...\n");
            task_handle_blink_led = NULL;
            vTaskDelete(NULL);
        }
    }
}

void control_task(void *pvParameters) {
    printf("Control Task running...\n");
    vTaskDelay(pdMS_TO_TICKS(5000));

    if (task_handle_blink_led != NULL) {
        printf("Control Task deleting Blink led Task...\n");
        vTaskDelete(task_handle_blink_led);   // Delete other task
    }
    vTaskDelete(NULL); 
    task_handle_blink_led = NULL;  // Delete self
}

void app_main(void) {

    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_INPUT_OUTPUT);

    // Create Blink_led Task
    xTaskCreate(
        blink_led_task,           // Task function
        "Blink led Task",         // Name
        2048,                 // Stack size (bytes in ESP-IDF)
        NULL,                 // Parameters
        5,                    // Priority
        &task_handle_blink_led    // Handle
    );

    // Create Control Task
    xTaskCreate(
        control_task,         // Task function
        "Control Task",       // Name
        2048,                 // Stack size (bytes in ESP-IDF)
        NULL,                 // Parameters
        4,                    // Priority
        NULL                  // No handle needed
    );
}