#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>

void init_button_pin(uint8_t pin);
void init_led_pin(uint8_t pin);
bool button_pressed(uint8_t pin);
#endif
