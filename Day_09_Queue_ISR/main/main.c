#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BUTTON_GPIO_1 0   // Example: BOOT button on many ESP32 boards
#define BUTTON_GPIO_2 GPIO_NUM_4
#define TAG "DAY9"

QueueHandle_t button_queue;

// ISR handler
static void IRAM_ATTR button_isr_handler(void *arg) {
    uint32_t gpio_num = (uint32_t) arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Send GPIO number to the queue (non-blocking, ISR safe)
    xQueueSendFromISR(button_queue, &gpio_num, &xHigherPriorityTaskWoken);

    // If a higher-priority task was woken, yield immediately
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

// Task to process button presses
void button_task(void *pvParameter) {
    uint32_t io_num;
    while (1) {
        // Wait indefinitely until ISR sends a message
        if (xQueueReceive(button_queue, &io_num, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Button pressed on GPIO %" PRIu32, io_num);
            // Debounce or handle logic here
            if (io_num == 0)
                printf("Pressed button on main board\n");
            else 
                printf("Pressed gpio4's button\n");
        }
    }
}

void config_button(int io_num){
     // Configure button pin
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,  // Trigger on falling edge
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << io_num),
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE
    };
    gpio_config(&io_conf);

}

void app_main() {
    // Create queue for GPIO events
    button_queue = xQueueCreate(10, sizeof(uint32_t));

    config_button(BUTTON_GPIO_1);
    config_button(BUTTON_GPIO_2);

    // Install ISR service
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO_1, button_isr_handler, (void *) BUTTON_GPIO_1);
    gpio_isr_handler_add(BUTTON_GPIO_2, button_isr_handler, (void *) BUTTON_GPIO_2);

    // Start button processing task
    xTaskCreate(button_task, "ButtonTask", 2048, NULL, 10, NULL);
}