
#include "../include/utils.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

void init_led_pin(int pin) {
  gpio_init(pin);
  gpio_set_dir(pin, GPIO_OUT);
}

void init_button_pin(int pin) {
  gpio_init(pin);
  gpio_set_dir(pin, GPIO_IN);
  gpio_pull_up(pin);
}

void init_rot_pin(int pin) {
  gpio_init(pin);
  gpio_set_dir(pin, GPIO_IN);
}

void setup_pwm(uint gpio) {
  gpio_set_function(gpio, GPIO_FUNC_PWM);
  uint slice = pwm_gpio_to_slice_num(gpio);
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, PWM_DIVIDER);
  pwm_config_set_wrap(&config, PWM_TOP);
  pwm_init(slice, &config, true);
}

void set_brightness(uint gpio, uint16_t level) {
  uint slice = pwm_gpio_to_slice_num(gpio);
  uint channel = pwm_gpio_to_channel(gpio);
  pwm_set_chan_level(slice, channel, level);
}

bool button_pressed(uint gpio) {
  return gpio_get(gpio) == 0; // Active low
}
