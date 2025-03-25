
#ifndef UTILS_H
#define UTILS_H
#include "pico/stdlib.h"

#define PWM_TOP 999
#define PWM_DIVIDER 125.0f

void setup_pwm(uint gpio);
void init_led_pin(int pin);
void init_button_pin(int pin);
bool button_pressed(uint gpio);
void set_brightness(uint gpio, uint16_t level);
void init_rot_pin(int pin);
#endif
