#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "freertos/semphr.h"

#define TAG "DAY_11"
#define MAX_RESOURCES 10

SemaphoreHandle_t resource_sem;


void producer_task(void *pvParameters) {
    TickType_t last_wake = xTaskGetTickCount();
    while (1) {
        UBaseType_t count = uxSemaphoreGetCount(resource_sem);
        printf("COUNT: %u\n", (unsigned)count);
        
        if (!xSemaphoreGive(resource_sem)) {
            printf("Counting semaphore is full\n");
        }
        else{
            printf("Producer gave resource\n");
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(500));
    }
}

void consumer_task_1(void *pvParameters) {

    int id = (int) pvParameters;
    while (1) {
        if (xSemaphoreTake(resource_sem, portMAX_DELAY)) {
            printf("Task %d consuming resource\n", id);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}


void consumer_task_2(void *pvParameters) {
    int id = (int) pvParameters;
    while (1) {
        if (xSemaphoreTake(resource_sem, portMAX_DELAY)) {
            printf("Task %d consuming resource\n", id);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}  



void app_main(void) {
    resource_sem = xSemaphoreCreateCounting(MAX_RESOURCES,
        0);
 
    if (resource_sem == NULL) {
        printf("Failed to create counting semaphore\n");
        return;
    }
 
    xTaskCreate(producer_task, "ProducerTask", 2048, NULL, 5, NULL);
    xTaskCreate(consumer_task_1, "Task1", 2048, (void *) 1, 5, NULL);
    xTaskCreate(consumer_task_2, "Task2", 2048, (void *) 2, 5, NULL);
}