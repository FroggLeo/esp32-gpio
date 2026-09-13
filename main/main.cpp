#include "driver/gpio.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "pins.hpp"
#include "soc/gpio_num.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "main";

gpio_num_t buzzer_pin = Board::D7;
gpio_num_t button_big_pin = Board::D8;
gpio_num_t button_stack_pin = Board::D9;

gpio_config_t buzzer = {};
gpio_config_t button_big = {};
gpio_config_t button_stack = {};

void setup_gpio() {
    buzzer.pin_bit_mask = 1ULL << buzzer_pin;
    buzzer.mode = GPIO_MODE_OUTPUT;
    buzzer.pull_down_en = GPIO_PULLDOWN_DISABLE;
    buzzer.pull_up_en = GPIO_PULLUP_DISABLE;
    buzzer.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&buzzer);
    button_big.pin_bit_mask = 1ULL << button_big_pin;
    button_big.mode = GPIO_MODE_INPUT;
    button_big.pull_up_en = GPIO_PULLUP_ENABLE;
    button_big.pull_down_en = GPIO_PULLDOWN_DISABLE;
    button_big.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&button_big);
    button_stack.pin_bit_mask = 1ULL << button_stack_pin;
    button_stack.mode = GPIO_MODE_INPUT;
    button_stack.pull_up_en = GPIO_PULLUP_DISABLE;
    button_stack.pull_down_en = GPIO_PULLDOWN_DISABLE;
    button_stack.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&button_stack);
}

void delay(int ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

extern "C" void app_main() {
    setup_gpio();
    while (true) {
        ESP_LOGI(TAG, "Button pressed: %d", gpio_get_level(button_big_pin));
        delay(500);
    }


}