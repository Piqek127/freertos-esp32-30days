#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"

#define TRIG_PIN GPIO_NUM_4
#define ECHO_PIN GPIO_NUM_5
#define LED_PIN GPIO_NUM_2

void task_blink_led(void *pvParameter) {
    while (1) {

        printf("vTaskDelay time: %lu ms\n",
               (unsigned long)(xTaskGetTickCount() * portTICK_PERIOD_MS));

        gpio_set_level(LED_PIN, !gpio_get_level(LED_PIN));
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1s
    }
}

void task_sample_sensor(void *pvParameter) {
    TickType_t last_wake = xTaskGetTickCount(); 
    //30ms is based on the max effective range of the sensor
    int64_t TIME_OUT = 30000;//microseconds;

    while (1) {

        printf("vTaskDelayUntil time: %lu ms\n",
            (unsigned long)(xTaskGetTickCount() * portTICK_PERIOD_MS));
        
        //Send pulse to TRIG pin
        gpio_set_level(TRIG_PIN, 1);
        esp_rom_delay_us(10);
        gpio_set_level(TRIG_PIN, 0);

        //Check if an echo has released
        int64_t wait_start = esp_timer_get_time();
        while (gpio_get_level(ECHO_PIN) == 0){

            if( esp_timer_get_time() - wait_start > TIME_OUT){
                printf("No echo\n");
                goto next_cycle;
            }
        }

        //Check is the echo has returned
        //If not usually means it is out of the sensor's range
        int64_t t_start = esp_timer_get_time();
        while (gpio_get_level(ECHO_PIN) == 1){
            if( esp_timer_get_time() - t_start > TIME_OUT){
                printf("Echo too long\n");
                goto next_cycle;
            }
        }

        int64_t t_end = esp_timer_get_time();
        printf("Distance: %.1f cm\n", (t_end - t_start) / 58.0f);

        next_cycle:    
            vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(1000));
    }
}

void app_main() {
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_INPUT_OUTPUT);

    gpio_reset_pin(TRIG_PIN);
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(TRIG_PIN, GPIO_FLOATING);

    gpio_reset_pin(ECHO_PIN);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);

    xTaskCreate(task_blink_led, "BlinkLed", 2048, NULL, 5, NULL);
    xTaskCreate(task_sample_sensor, "SampleSensor", 4096, NULL, 5, NULL);
}