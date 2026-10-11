/*
 * EVENT GROUPS (PREVIEW FOR DAY 15)
 *  - A set of flag bits, one per event: #define BUTTON_BIT (1 << 0)
 *  - xEventGroupSetBitsFromISR() sets a bit, xEventGroupWaitBits() waits for it.
 *  - A set bit stays set until cleared, so decide who clears it and when.
 *    The "clear on exit" argument controls this.
 *  - Every task waiting on the bit can wake, so it works as a broadcast.
 */


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BOARD_BUTTON 0
#define BUTTON_BIT (1 << 0)
#define TAG "DAY10"

static EventGroupHandle_t events;

// ISR: only give the semaphore, keep it short
static void IRAM_ATTR button_isr_handler(void *arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(events, BUTTON_BIT, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

// Both tasks block on the same semaphore; only one wakes per press
void task_a(void *pvParameter) {
    while (1) {
        if (xEventGroupWaitBits(events, BUTTON_BIT, pdTRUE /* clear on exit */, pdFALSE, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Task A receives event group");
        }
    }
}

void task_b(void *pvParameter) {
    while (1) {
        if (xEventGroupWaitBits(events, BUTTON_BIT, pdTRUE /* clear on exit */, pdFALSE, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Task B receives event group");
        }
    }
}

// Pull-up: pin idles HIGH, a press pulls it LOW (falling edge)
void config_button(int ionum) {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << ionum),
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE
    };
    gpio_config(&io_conf);
}

void app_main() {
    // Create the semaphore before the interrupt and tasks that use it
    events = xEventGroupCreate();
    if (events == NULL) {
        ESP_LOGE(TAG, "Failed to create event group");
        return;
    }

    config_button(BOARD_BUTTON);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BOARD_BUTTON, button_isr_handler, NULL);

    xTaskCreate(task_a, "TaskA", 2048, NULL, 10, NULL);
    xTaskCreate(task_b, "TaskB", 2048, NULL, 10, NULL);
}