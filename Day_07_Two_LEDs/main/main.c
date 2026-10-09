/*
 * Day 7 – Hands-on Mini Project: Blinking Two LEDs with Two Tasks
 *
 * This example demonstrates timing differences between
 * vTaskDelay() (relative delay) and vTaskDelayUntil() (absolute
 * periodic schedule). Two LEDs blink at
 * different rates:
 *  - Task A: LED1 toggles every 500 ms using vTaskDelay()
 *            -> may drift over long runs
 *  - Task B: LED2 toggles every 1000 ms using vTaskDelayUntil()
 *            -> stays on a fixed cadence
 *
 * Wiring:
 *   LED1 (GPIO2 by default) -> resistor -> GND
 *   LED2 (GPIO4 by default) -> resistor -> GND
 *
 * Notes:
 *   - Adjust LED1_GPIO / LED2_GPIO for your hardware if needed.
 *   - Prints drift information to the console to illustrate timing behavior.
 */

#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_sys.h"

#define TAG "DAY7"

// === Adjust these for your board if necessary ===
#ifndef LED1_GPIO
#define LED1_GPIO GPIO_NUM_2   // Often has onboard LED on many DevKit boards
#endif

#ifndef LED2_GPIO
#define LED2_GPIO GPIO_NUM_4
#endif


#ifndef BUTTON_GPIO
#define BUTTON_GPIO GPIO_NUM_5
#endif

TaskHandle_t taskA_handle= NULL;

//Inline mean pasting the entire code when called
static inline void configure_led(gpio_num_t pin)
{
    // Configure pin as output and start low
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io);
    gpio_set_level(pin, 0);
}

static inline void configure_button(gpio_num_t pin)
{
    // Configure pin as output and start low
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io);
}

static inline void toggle_led(gpio_num_t pin)
{
    int current = gpio_get_level(pin);
    gpio_set_level(pin, !current);
}

/*
 * Task A: Blink LED1 using vTaskDelay() (relative delay).
 * Demonstrates drift accumulation when the loop body takes non-zero time.
 */
static void taskA_delay(void *pv)
{
    const TickType_t period_ticks = pdMS_TO_TICKS(500); // 500 ms
    uint32_t iteration = 0;
    //Esp log function with I(info) tag
    ESP_LOGI(TAG, "Task A (vTaskDelay) started on core %d", xPortGetCoreID());

    while (1) {
        toggle_led(LED1_GPIO);
        iteration++;
        esp_rom_delay_us(15000);
        // Print a timestamp (ms) to visualize drift
        TickType_t now_ticks = xTaskGetTickCount();
        uint32_t now_ms = now_ticks * portTICK_PERIOD_MS;
        //PRIu32 automatically fit %...  based on system currently
        ESP_LOGI(TAG, "[A] t=%" PRIu32 " ms, iter=%" PRIu32, now_ms, iteration);

        // Relative delay: next wake-up occurs 'period_ticks' after this call
        vTaskDelay(period_ticks);
    }
}

/*
 * Task B: Blink LED2 using vTaskDelayUntil() (absolute periodic schedule).
 * Keeps a stable cadence referenced to the first captured tick.
 */
static void taskB_delay_until(void *pv)
{
    const TickType_t period_ticks = pdMS_TO_TICKS(1000); // 1000 ms
    TickType_t last_wake = xTaskGetTickCount();          // Reference (t0)
    uint32_t iteration = 0;

    ESP_LOGI(TAG, "Task B (vTaskDelayUntil) started on core %d", xPortGetCoreID());

    while (1) {
        toggle_led(LED2_GPIO);
        esp_rom_delay_us(10000);
        iteration++;

        TickType_t now_ticks = xTaskGetTickCount();
        uint32_t now_ms = now_ticks * portTICK_PERIOD_MS;
        ESP_LOGI(TAG, "[B] t=%" PRIu32 " ms, iter=%" PRIu32, now_ms, iteration);

        // Absolute delay: wake exactly every 'period_ticks' since last_wake
        vTaskDelayUntil(&last_wake, period_ticks);
    }
}

/*
 * Optional status task:
 * Prints how far Task A has "slipped" from an ideal schedule
 * compared to an ideal 500 ms cadence. This is a didactic metric, not a strict
 * real-time analysis tool.
 */
static void task_status(void *pv)
{
    const TickType_t period_ticks = pdMS_TO_TICKS(2000); // every 2 s
    TickType_t last_wake = xTaskGetTickCount();
    uint32_t seconds = 0;

    ESP_LOGI(TAG, "Status task started on core %d", xPortGetCoreID());

    while (1) {
        vTaskDelayUntil(&last_wake, period_ticks);
        seconds += 2;

        // Report uptime and tick count
        TickType_t ticks = xTaskGetTickCount();
        uint32_t ms = ticks * portTICK_PERIOD_MS;
        ESP_LOGI(TAG, "[STATUS] uptime ~%" PRIu32 " ms (%" PRIu32 "s)", ms, seconds);
    }
}

static void task_button(void *pv)
{
    uint32_t last = 0;
    bool paused = false;

    while(1){
        uint32_t now = gpio_get_level(BUTTON_GPIO);
        if(last == 1 && now == 0){
            if(paused){
                paused = false;
                vTaskResume(taskA_handle);
                ESP_LOGI(TAG, "Task A resumed");
            }
            else {
                vTaskSuspend(taskA_handle);
                paused = true;
                ESP_LOGI(TAG, "Task A suspended");
            }
        }
        
        last = now;
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}


void app_main(void)
{
    
    // --- GPIO setup
    configure_led(LED1_GPIO);
    configure_led(LED2_GPIO);
    configure_button(BUTTON_GPIO);


    // --- Create tasks
    // Priorities kept the same to let the scheduler time-slice fairly
    BaseType_t okA = xTaskCreate(taskA_delay, "TaskA_Delay", 2048, NULL, 5, &taskA_handle);
    BaseType_t okB = xTaskCreate(taskB_delay_until, "TaskB_DelayUntil",
                                 2048, NULL, 5, NULL);

    // Optional status task (lower priority)
    xTaskCreate(task_status, "TaskStatus", 2048, NULL, 3, NULL);
    xTaskCreate(task_button, "Button", 2048, NULL, 4, NULL);

    if (okA != pdPASS || okB != pdPASS) {
        ESP_LOGE(TAG, "Failed to create one or more tasks");
    }
}