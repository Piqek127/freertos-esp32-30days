#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_random.h"
#include <inttypes.h>

#define QUEUE_LENGTH 10

QueueHandle_t queue;
TaskHandle_t producer_handle;

void producer_task(void *pvParameters) {
    while (1) {
        uint32_t rand_n = esp_random();

        if (xQueueSend(queue, &rand_n, pdMS_TO_TICKS(100)) == pdPASS) {
            printf("Producer sent: %" PRIu32 "\n", rand_n);
            printf("Queue: %d\n", uxQueueMessagesWaiting(queue));
        } 
        else {
            printf("Queue is full - Producer suspended\n");
            vTaskSuspend(NULL);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
        
    }
}

void consumer_task(void *pvParameters) {
    uint32_t value;
    while (1) {
        if (xQueueReceive(queue, &value, pdMS_TO_TICKS(1000)) == pdPASS) {
            printf("Consumer received: %"PRIu32"\n", value);
            if (value % 2 == 0){
                printf(" - Even\n");
            }
            else
                printf(" - Odd\n");
        } else {
            printf("Consumer: Queue empty!\n");
        }

        if (eTaskGetState(producer_handle) == eSuspended) {
        vTaskResume(producer_handle);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}



void app_main(void) {
    queue = xQueueCreate(QUEUE_LENGTH, sizeof(uint32_t));
    if (queue == NULL) {
        printf("Failed to create queue\n");
        return;
    }

    xTaskCreate(producer_task, "Producer", 2048, NULL, 5, &producer_handle);
    xTaskCreate(consumer_task, "Consumer", 2048, NULL, 5, NULL);
}