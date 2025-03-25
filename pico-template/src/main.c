#include "/usr/arm-none-eabi/include/stdio.h"
#include "pico/stdlib.h"

int main() {
  const uint led_pin = 22;
  uint count = 0;

  // Initialize LED pin
  gpio_init(led_pin);
  gpio_set_dir(led_pin, GPIO_OUT);

  // Initialize chosen serial port
  stdio_init_all();

  // Loop forever
  while (true) {

    // Blink LED
    gpio_put(led_pin, true);
    printf("Blinking! %u\r\n", ++count);
    sleep_ms(1000);
    gpio_put(led_pin, false);
    sleep_ms(1000);
  }
}
