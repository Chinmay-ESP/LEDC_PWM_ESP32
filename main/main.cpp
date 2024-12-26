#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

class LEDController {
public:
    LEDController(gpio_num_t pin, ledc_channel_t channel, ledc_timer_t timer, uint32_t freq, ledc_timer_bit_t resolution)
        : gpio_pin(pin), ledc_channel(channel), ledc_timer(timer), frequency(freq), duty_resolution(resolution) {
        configureTimer();
        configureChannel();
        ledc_fade_func_install(0);
    }

    void fadeInAndOut() {
        while (true) {
            fadeUp();
            fadeDown();
        }
    }

private:
    gpio_num_t gpio_pin;
    ledc_channel_t ledc_channel;
    ledc_timer_t ledc_timer;
    uint32_t frequency;
    ledc_timer_bit_t duty_resolution;

    void configureTimer() {
        ledc_timer_config_t timer_config = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = duty_resolution,
            .timer_num = ledc_timer,
            .freq_hz = frequency,
            .clk_cfg = LEDC_AUTO_CLK
        };
        ledc_timer_config(&timer_config);
    }

    void configureChannel() {
        ledc_channel_config_t channel_config = {
            .gpio_num = gpio_pin,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = ledc_channel,
            .timer_sel = ledc_timer,
            .duty = 0,
            .hpoint = 0
        };
        ledc_channel_config(&channel_config);
    }

    void fadeUp() {
        for (int duty = 0; duty <= 255; ++duty) {
            ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, ledc_channel, duty, 0);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }

    void fadeDown() {
        for (int duty = 255; duty >= 0; --duty) {
            ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, ledc_channel, duty, 0);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
};

extern "C" void app_main() {
    LEDController ledController(GPIO_NUM_23, LEDC_CHANNEL_0, LEDC_TIMER_0, 5000, LEDC_TIMER_8_BIT);
    ledController.fadeInAndOut();
}
