#include "../include/utils.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define LED_D3 20
#define LED_D2 21
#define LED_D1 22

#define SW_0 9
#define SW_1 8
#define SW_2 7

#define BRIGHTNES_SCROLL_SPEED 1

#define DEFAULT_BRIGHTNESS 500

int main() {
  uint count = 0;

  bool led_on = false;
  bool power_held_down = false;

  uint brightness = DEFAULT_BRIGHTNESS; // 0 - 1000  0.1% precision

  setup_pwm(LED_D1);
  setup_pwm(LED_D2);
  setup_pwm(LED_D3);

  init_button_pin(SW_0);
  init_button_pin(SW_1);
  init_button_pin(SW_2);

  // Initialize chosen serial port
  stdio_init_all();

  // Loop forever
  while (true) {
    printf("%d\n", count++);

    if (power_held_down &&
        !button_pressed(
            SW_1)) { // if button is released set power_held_down to false
      power_held_down = false;
    } else if (!power_held_down &&
               button_pressed(SW_1)) { // if button is pressed for the first
                                       // time toggle led
      if (led_on && brightness == 0) {
        brightness = DEFAULT_BRIGHTNESS;

      } else {
        led_on = !led_on;
      }
      power_held_down = true;
      sleep_ms(50); // wait for switch bounce
    }

    if (led_on) {
      if (button_pressed(SW_2)) {
        if (brightness > 0) {
          brightness--;
          sleep_ms(BRIGHTNES_SCROLL_SPEED);
        }
      } else if (button_pressed(SW_0)) {
        if (brightness < 1000) {
          brightness++;
          sleep_ms(BRIGHTNES_SCROLL_SPEED);
        }
      }
      // set leds to the correct brightness
      set_brightness(LED_D3, brightness);
      set_brightness(LED_D2, brightness);
      set_brightness(LED_D1, brightness);
    } else {
      // turn of leds
      set_brightness(LED_D1, 0);
      set_brightness(LED_D2, 0);
      set_brightness(LED_D3, 0);
    }
  }
}
