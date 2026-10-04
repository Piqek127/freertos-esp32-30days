#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t low_task_handle  = NULL;
TaskHandle_t med_task_handle  = NULL;
TaskHandle_t high_task_handle = NULL;

void low_priority_task(void *pvParameter)
{
    while (1)
    {
        printf("Low Priority Task running every 1 second on core %d\n", xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void medium_priority_task(void *pvParameter)
{
    while (1)
    {
        printf("Medium Priority Task running every 500 ms on core %d\n", xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void high_priority_task(void *pvParameter)
{
    for (int i = 1; i <= 5; i++)
    {
        printf("High Priority Task iteration %d on core %d\n", i, xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    printf("High Priority Task lowering its priority to lowest on core %d ...\n", xPortGetCoreID());
    vTaskPrioritySet(NULL, 1);   // NULL = this task

    while (1)
    {
        printf("High Priority Task (now low priority) still running on core %d ...\n", xPortGetCoreID());  
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    xTaskCreate(low_priority_task,    "LowPriorityTask",    2048, NULL, 2, &low_task_handle);
    xTaskCreate(medium_priority_task, "MediumPriorityTask", 2048, NULL, 3, &med_task_handle);
    xTaskCreate(high_priority_task,   "HighPriorityTask",   2048, NULL, 4, &high_task_handle);
}