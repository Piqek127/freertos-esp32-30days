#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "DAY18"
#define ONE_SHOT_DELAY 5000
#define PERIODIC_DELAY 500
#define LED_PIN GPIO_NUM_2

TimerHandle_t one_shot_timer;
TimerHandle_t periodic_timer;

void one_shot_callback(TimerHandle_t xTimer) {
    gpio_set_level(LED_PIN, !gpio_get_level(LED_PIN));
    ESP_LOGI(TAG, "One-shot timer expired!");    
}

void periodic_callback(TimerHandle_t xTimer) {
    static int count = 0;
    count++;
    gpio_set_level(LED_PIN, !gpio_get_level(LED_PIN));
    ESP_LOGI(TAG, "Periodic timer tick %d", count);

    if (count == 10) {
        ESP_LOGI(TAG, "Stopping periodic timer after 10 ticks");
        xTimerStop(xTimer, 0);
    }
}

void app_main() {

    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_INPUT_OUTPUT);

    one_shot_timer = xTimerCreate("OneShot",
                                  pdMS_TO_TICKS(ONE_SHOT_DELAY),
                                  pdFALSE,
                                  NULL,
                                  one_shot_callback);

    periodic_timer = xTimerCreate("Periodic",
                                  pdMS_TO_TICKS(PERIODIC_DELAY),
                                  pdTRUE,
                                  NULL,
                                  periodic_callback);

    xTimerStart(one_shot_timer, 0);
    xTimerStart(periodic_timer, 0);
}