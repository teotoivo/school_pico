#include "utils.h"
#include "hardware/i2c.h"
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

void i2c_init_custom(i2c_inst_t *i2c, uint16_t sda_pin, uint16_t scl_pin,
                     uint32_t baudrate) {
  i2c_init(i2c, baudrate);

  gpio_set_function(sda_pin, GPIO_FUNC_I2C);
  gpio_set_function(scl_pin, GPIO_FUNC_I2C);

  gpio_pull_up(sda_pin);
  gpio_pull_up(scl_pin);
}
