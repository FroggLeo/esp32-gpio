#include "driver/gpio.h"
#include "esp_log.h"
#include "hal/adc_types.h"
#include "hal/gpio_types.h"
#include "hal/ledc_types.h"
#include "pins.hpp"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_num.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/ledc.h"
#include <cstdint>

static const char* TAG = "main";
static constexpr bool debug = false;

gpio_num_t buzzer_pin = Board::D7;
gpio_config_t buzzer = {};
ledc_timer_config_t buzzer_timer = {};
ledc_channel_config_t buzzer_channel = {};

gpio_num_t button_big_pin = Board::D8;
gpio_config_t button_big = {};

gpio_num_t ec11_pin1 = Board::D10;
gpio_num_t ec11_pin2 = Board::D9;
gpio_num_t ec11_pin3 = Board::D2;
gpio_config_t ec11 = {};

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
    ec11.pin_bit_mask = 1ULL << ec11_pin1;
    ec11.mode = GPIO_MODE_INPUT;
    ec11.pull_up_en = GPIO_PULLUP_ENABLE;
    ec11.pull_down_en = GPIO_PULLDOWN_DISABLE;
    ec11.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&ec11);
    ec11.pin_bit_mask = 1ULL << ec11_pin2;
    gpio_config(&ec11);
    ec11.pin_bit_mask = 1ULL << ec11_pin3;
    gpio_config(&ec11);
}

int8_t check_ec11(gpio_num_t pin1, gpio_num_t pin2) {
    bool curr_pin1 = gpio_get_level(pin1);
    bool curr_pin2 = gpio_get_level(pin2);
    static bool prev_pin1 = curr_pin1; // set current read only on first run
    int8_t direction = 0;
    if (!curr_pin1 && prev_pin1) { // true when current read is LOW and previous read is HIGH
        if (curr_pin2) { // if pin2 is HIGH
            direction = 1;
        } else {
            direction = -1;
        }
    }
    prev_pin1 = curr_pin1;
    return direction;
}

/* unused
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

int8_t button_stack_read() {
    int v = 4000;
    adc_oneshot_read(adc_handle, button_stack_adc_pin, &v);
    if (debug) {ESP_LOGI(TAG, "Button stack value: %d", v);}
    if (v < 600) {
        return 0;
    } else if (v < 1600) {
        return 1;
    } else if (v < 2150) {
        return 2;
    } else if (v < 3000) {
        return 3;
    }
    return -1;
}
*/
void setup_buzzer() {
    buzzer_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    buzzer_timer.timer_num = LEDC_TIMER_0;
    buzzer_timer.duty_resolution = LEDC_TIMER_10_BIT;
    buzzer_timer.freq_hz = 1000;
    buzzer_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&buzzer_timer);
    buzzer_channel.gpio_num = buzzer_pin;
    buzzer_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    buzzer_channel.channel = LEDC_CHANNEL_0;
    buzzer_channel.intr_type = LEDC_INTR_DISABLE;
    buzzer_channel.timer_sel = LEDC_TIMER_0;
    buzzer_channel.duty = 0;
    buzzer_channel.hpoint = 0;
    ledc_channel_config(&buzzer_channel);
}

void buzzer_tone(uint32_t f) {
    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, f);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void buzzer_stop() {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void delay(int ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

extern "C" void app_main() {
    setup_gpio();
    setup_buzzer();

    /*while (true) {
        ESP_LOGI(TAG, "Button pressed: %d", !gpio_get_level(button_big_pin));
        int8_t pressed_button = button_stack_read();
        if (pressed_button != -1) {
            ESP_LOGI(TAG, "Button stack pressed: button %d", pressed_button);
        }
        if (pressed_button == 0) {
            buzzer_tone(500);
        } else if (pressed_button == 1) {
            buzzer_tone(750);
        } else if (pressed_button == 2) {
            buzzer_tone(1000);
        } else if (pressed_button == 3) {
            buzzer_tone(1500);
        } else {
            buzzer_stop();
        }
        delay(50);
    }*/


}