#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"

// 0 = binary semaphore (priority inversion happens)
// 1 = mutex            (priority inheritance fixes it)
#define USE_MUTEX 1
 
// Run everything on one core so the medium task can really starve the low task
#define DEMO_CORE 0

// Binary semaphore used for resource sharing (NO priority inheritance)
SemaphoreHandle_t uart_lock;

static void busy_work(uint32_t iterations)
{
    volatile uint32_t i = 0;

    while (i < iterations) {
        i++;
    }
}

// Low priority task that holds a resource
void low_task(void *pv) {
    while (1) {
        if (xSemaphoreTake(uart_lock, portMAX_DELAY)) {
            printf("[LOW ] acquired lock\n");
            busy_work(5000000);
            printf("[LOW ] releasing lock\n");
            xSemaphoreGive(uart_lock);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// High priority task that needs the same resource
void high_task(void *pv) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(500)); // Start later
        int64_t t0 = esp_timer_get_time();
        printf("[HIGH] wants lock\n");

        if (xSemaphoreTake(uart_lock, portMAX_DELAY)) {
            int64_t waited_us = esp_timer_get_time() - t0;

            printf("[HIGH] got lock after %lld us\n",
                (long long)waited_us);

            xSemaphoreGive(uart_lock);
        }
    }
}

// Medium priority task to demonstrate priority inversion
void medium_task(void *pv) {
    while (1) {
        printf("[MED ] hogging CPU\n");
        busy_work(5000000);               // under the 5 s task watchdog timeout
        printf("[MED ] done\n");
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

// Entry point
void app_main() {
#if USE_MUTEX
    uart_lock = xSemaphoreCreateMutex();
    printf("=== Using MUTEX (priority inheritance) ===\n");
#else
    uart_lock = xSemaphoreCreateBinary();
    xSemaphoreGive(uart_lock);   // binary semaphores start empty
    printf("=== Using BINARY SEMAPHORE (no inheritance) ===\n");
#endif
    xTaskCreatePinnedToCore(low_task,    "LowTask",  3072, NULL, 2,  NULL, DEMO_CORE);
    xTaskCreatePinnedToCore(medium_task, "MedTask",  3072, NULL, 5,  NULL, DEMO_CORE);
    xTaskCreatePinnedToCore(high_task,   "HighTask", 3072, NULL, 10, NULL, DEMO_CORE);
}