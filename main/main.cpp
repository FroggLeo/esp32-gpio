#include "driver/gpio.h"
#include "esp_log.h"
#include "hal/adc_types.h"
#include "hal/gpio_types.h"
#include "pins.hpp"
#include "soc/gpio_num.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/ledc.h"
#include <cstdint>

static const char* TAG = "main";

gpio_num_t buzzer_pin = Board::D7;
gpio_num_t button_big_pin = Board::D8;
gpio_num_t button_stack_pin = Board::D9;
adc_channel_t button_stack_adc_pin = ADC_CHANNEL_7;

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
    /*button_stack.pin_bit_mask = 1ULL << button_stack_pin;
    button_stack.mode = GPIO_MODE_INPUT;
    button_stack.pull_up_en = GPIO_PULLUP_DISABLE;
    button_stack.pull_down_en = GPIO_PULLDOWN_DISABLE;
    button_stack.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&button_stack);*/
}

adc_oneshot_unit_handle_t adc_handle;

void setup_adc() {
    adc_oneshot_unit_init_cfg_t init_config = {};
    init_config.unit_id = ADC_UNIT_1;

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    adc_oneshot_chan_cfg_t channel_config = {};
    channel_config.bitwidth = ADC_BITWIDTH_DEFAULT;
    channel_config.atten = ADC_ATTEN_DB_12;

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle,button_stack_adc_pin,&channel_config));
}

void delay(int ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

int8_t button_stack_read() {
    int v = 4000;
    adc_oneshot_read(adc_handle, button_stack_adc_pin, &v);
    ESP_LOGI(TAG, "Button stack value: %d", v);
    if (v < 500) {
        return 0;
    } else if (v < 1500) {
        return 1;
    } else if (v < 2100) {
        return 2;
    } else if (v < 2500) {
        return 3;
    }
    return -1;
}

extern "C" void app_main() {
    setup_gpio();
    setup_adc();
    while (true) {
        ESP_LOGI(TAG, "Button pressed: %d", !gpio_get_level(button_big_pin));
        int8_t pressed_button = button_stack_read();
        if (pressed_button != -1) {
            ESP_LOGI(TAG, "Button stack pressed: button %d", pressed_button);
        }
        delay(500);
    }


}