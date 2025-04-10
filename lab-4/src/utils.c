#include "utils.h"
#include "pico/stdlib.h"
#include <stdint.h>

void init_led_pin(uint8_t pin) {
  gpio_init(pin);
  gpio_set_dir(pin, GPIO_OUT);
}

void init_button_pin(uint8_t pin) {
  gpio_init(pin);
  gpio_set_dir(pin, GPIO_IN);
  gpio_pull_up(pin);
}

bool button_pressed(uint8_t pin) {
  return gpio_get(pin) == 0; // Active low
}
