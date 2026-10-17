#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#define TAG "DAY15"

#define BIT_TASK_A   (1 << 0)
#define BIT_TASK_B   (1 << 1)
#define BIT_TASK_C   (1 << 2)

EventGroupHandle_t sync_event_group;

void taskA(void *pvParameter) {
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "Task A finished work, setting BIT_TASK_A");
    xEventGroupSetBits(sync_event_group, BIT_TASK_A);
    vTaskDelete(NULL);
}

void taskB(void *pvParameter) {
    vTaskDelay(pdMS_TO_TICKS(4000));
    ESP_LOGI(TAG, "Task B finished work, setting BIT_TASK_B");
    xEventGroupSetBits(sync_event_group, BIT_TASK_B);
    vTaskDelete(NULL);
}

void taskC(void *pvParameter) {
    vTaskDelay(pdMS_TO_TICKS(6000));
    ESP_LOGI(TAG, "Task C finished work, setting BIT_TASK_C");
    xEventGroupSetBits(sync_event_group, BIT_TASK_C);
    vTaskDelete(NULL);
}

void coordinator_task(void *pvParameter) {
    ESP_LOGI(TAG, "Coordinator waiting for Task A and B...");
    EventBits_t bits = 0;
    
    
    while (__builtin_popcount(bits) < 2) {
        bits |= xEventGroupWaitBits(
                sync_event_group,
                BIT_TASK_A | BIT_TASK_B | BIT_TASK_C,
                pdTRUE,
                pdTRUE,
                portMAX_DELAY
            );
    }

    switch (bits & (BIT_TASK_A | BIT_TASK_B | BIT_TASK_C)) {
    case BIT_TASK_A | BIT_TASK_B:
        ESP_LOGI(TAG, "A and B finished first");
        break;
    case BIT_TASK_A | BIT_TASK_C:
        ESP_LOGI(TAG, "A and C finished first");
        break;
    case BIT_TASK_B | BIT_TASK_C:
        ESP_LOGI(TAG, "B and C finished first");
        break;
    default:
        ESP_LOGI(TAG, "All three finished together");
        break;
}

    vTaskDelete(NULL);
}

void app_main() {
    sync_event_group = xEventGroupCreate();

    xTaskCreate(taskA, "TaskA", 2048, NULL, 5, NULL);
    xTaskCreate(taskB, "TaskB", 2048, NULL, 5, NULL);
    xTaskCreate(taskC, "TaskC", 2048, NULL, 5, NULL);
    xTaskCreate(coordinator_task, "Coordinator", 2048, NULL, 6, NULL);
}