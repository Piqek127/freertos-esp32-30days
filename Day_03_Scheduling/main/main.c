#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void task_unpinned(void *pvParameters) {
    while (1) {
        printf("Unpinned Task running on Core %d\n", xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_pinned_core1(void *pvParameters) {
    while (1) {
        printf("Pinned Task running on Core %d\n", xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void) {
    xTaskCreate(task_unpinned, "Task Unpinned", 2048, NULL, 5, NULL);

#if CONFIG_FREERTOS_UNICORE
    // Single-core chip (or "run on first core only" enabled)
    xTaskCreate(task_pinned_core1, "Task Core1", 2048, NULL, 5, NULL);
#else
    // Dual-core chip: pin to Core 1
    xTaskCreatePinnedToCore(task_pinned_core1, "Task Core1",
                            2048, NULL, 5, NULL, 1);
#endif
}

