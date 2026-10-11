/*
 * DAY 10 NOTES: BINARY SEMAPHORES
 *
 * WHAT IT IS
 *  - A flag with two states: taken (0) and given (1).
 *  - It signals "something happened". It carries no data.
 *  - Not a lock: any task or ISR can give it, and it has no owner.
 *
 * CORE CALLS
 *  sem = xSemaphoreCreateBinary();          starts at 0 (taken)
 *  xSemaphoreTake(sem, portMAX_DELAY);      block until given
 *  xSemaphoreGive(sem);                     from a task
 *  xSemaphoreGiveFromISR(sem, &woken);      from an ISR
 *
 * MAIN PATTERN: ISR -> semaphore -> task
 *  1. Button press triggers an interrupt (falling edge, pull-up enabled).
 *  2. The ISR only gives the semaphore, then calls portYIELD_FROM_ISR()
 *     if woken is true.
 *  3. A task sleeping in xSemaphoreTake wakes and does the real work.
 *
 * WHY THIS DESIGN
 *  - ISRs must be short and can't block or print.
 *  - A blocked task uses no CPU until the event arrives.
 *  - IRAM_ATTR keeps the ISR in internal RAM so it works while flash is busy.
 *
 * VS THE OTHERS
 *  - Mutex: protects shared data, same task takes and gives, priority inheritance.
 *  - Queue: carries data and keeps order. Use it to know WHAT happened
 *    (e.g. which button).
 *  - Binary semaphore: only says THAT an event happened.
 *
 * CHALLENGE: TWO TASKS WAITING ON ONE SEMAPHORE
 *  - Each press wakes only ONE task. A semaphore is not a broadcast.
 *  - Which task wins depends on priority and wait order.
 *    OBSERVED: ...
 *  - To wake ALL tasks with one event, use an event group.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BOARD_BUTTON 0
#define TAG "DAY10"

SemaphoreHandle_t button_semaphore;

// ISR: only give the semaphore, keep it short
static void IRAM_ATTR button_isr_handler(void *arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(button_semaphore, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

// Both tasks block on the same semaphore; only one wakes per press
void task_a(void *pvParameter) {
    while (1) {
        if (xSemaphoreTake(button_semaphore, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Task A takes semaphore!");
        }
    }
}

void task_b(void *pvParameter) {
    while (1) {
        if (xSemaphoreTake(button_semaphore, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Task B takes semaphore");
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
    button_semaphore = xSemaphoreCreateBinary();
    if (button_semaphore == NULL) {
        ESP_LOGE(TAG, "Failed to create semaphore");
        return;
    }

    config_button(BOARD_BUTTON);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BOARD_BUTTON, button_isr_handler, NULL);

    xTaskCreate(task_a, "TaskA", 2048, NULL, 9, NULL);
    xTaskCreate(task_b, "TaskB", 2048, NULL, 10, NULL);
}