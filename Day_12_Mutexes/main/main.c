#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_timer.h"

#define TAG "DAY12"
#define UART_PORT UART_NUM_0

SemaphoreHandle_t uart_mutex;

// Task function that simulates writes to UART
void uart_task(void *pvParameter) {
    // Get the task name, passed as parameter
    const char *task_name = (const char *) pvParameter;

    while (1) {
        // Try to take the mutex before accessing UART
        if (xSemaphoreTake(uart_mutex, portMAX_DELAY)) {
            for (int i = 0; i < 5; i++) {
                printf("%s writing line %d\n", task_name, i);
                vTaskDelay(pdMS_TO_TICKS(200));
            }
            // Release the mutex
            xSemaphoreGive(uart_mutex);
        }
        // Delay before next attempt
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void uart_logger_task(void *pvParameter) {

    while (1) {
        // Try to take the mutex before accessing UART
        if (xSemaphoreTake(uart_mutex, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Time: %lld ms", esp_timer_get_time() / 1000 );
            // Release the mutex
            xSemaphoreGive(uart_mutex);
        }
        // Delay before next attempt
        vTaskDelay(pdMS_TO_TICKS(1000));
    }   
}

void app_main() {
    // Create mutex
    uart_mutex = xSemaphoreCreateMutex();
    if (uart_mutex == NULL) {
        printf("Failed to create mutex\n");
        return;
    }

    // Create two tasks sharing UART
    xTaskCreate(uart_task, "TaskA", 2048, "Task A", 4, NULL);
    xTaskCreate(uart_task, "TaskB", 2048, "Task B", 3, NULL);
    xTaskCreate(uart_logger_task, "Timer", 2048, NULL, 5, NULL);
}