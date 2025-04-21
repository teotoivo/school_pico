#ifndef UTILS_H
#define UTILS_H
#include "pico/stdlib.h"
#include <hardware/i2c.h>
#include <stdint.h>

void init_button_pin(uint8_t pin);
void init_led_pin(uint8_t pin);
bool button_pressed(uint8_t pin);
void i2c_init_custom(i2c_inst_t *i2c, uint16_t sda_pin, uint16_t scl_pin,
                     uint32_t baudrate);
#endif
