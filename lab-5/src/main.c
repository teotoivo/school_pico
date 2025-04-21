#include "eeprom.h"
#include "hardware_definitions.h"
#include "pico/stdlib.h"
#include "utils.h"
#include <hardware/gpio.h>
#include <hardware/timer.h>
#include <pico/time.h>
#include <stdint.h>
#include <stdio.h>

#define LOG(fmt, ...)                                                          \
  printf("%lld s: " fmt "\n", (uint64_t)(time_us_64() / 1000000), ##__VA_ARGS__)

int main() {
  stdio_init_all(); // needed for uart

  init_button_pin(SW_0);
  init_button_pin(SW_1);
  init_button_pin(SW_2);
  uint8_t buttons[3] = {SW_0, SW_1, SW_2};
  bool button_held[3] = {0};

  init_led_pin(LED_D1);
  init_led_pin(LED_D2);
  init_led_pin(LED_D3);
  uint8_t leds[3] = {LED_D3, LED_D2, LED_D1};
  uint8_t led_state[3] = {0};

  i2c_init_custom(EEPROM_PORT, EEPROM_sda_pin, EEPROM_scl_pin,
                  EEPROM_BAUD_RATE);

  // offset addresses from max address to store led states at the end of eeprom
  uint16_t addresses[3][2] = {{EEPROM_MAX_ADDRESS - 5, EEPROM_MAX_ADDRESS - 6},
                              {EEPROM_MAX_ADDRESS - 3, EEPROM_MAX_ADDRESS - 2},
                              {EEPROM_MAX_ADDRESS - 1, EEPROM_MAX_ADDRESS}};

  // // CORRUPT EEPROM to simulate data failure
  // eeprom_write_byte(addresses[0][0], 0xAA); // invalid value
  // eeprom_write_byte(addresses[0][1], 0xBB); // not ~0xAA
  // eeprom_write_byte(addresses[1][0], 0xCC); // invalid value
  // eeprom_write_byte(addresses[1][1], 0xDD); // not ~0xCC
  // eeprom_write_byte(addresses[2][0], 0xEE); // invalid value
  // eeprom_write_byte(addresses[2][1], 0xFF); // not ~0xEE

  clear_terminal();
  LOG("BOOT");

  // load led states
  for (int i = 0; i < 3; i++) {
    uint8_t byte = eeprom_read_byte(addresses[i][0]);
    uint8_t inverted_byte = eeprom_read_byte(addresses[i][1]);

    if ((byte == 0 || byte == 1) && byte == (uint8_t)(~inverted_byte)) {
      led_state[i] = byte;
    } else {
      // invalid

      eeprom_write_byte(addresses[0][0], 0);
      eeprom_write_byte(addresses[0][1], ~0);

      eeprom_write_byte(addresses[1][0], 1);
      eeprom_write_byte(addresses[1][1], ~1);

      eeprom_write_byte(addresses[2][0], 0);
      eeprom_write_byte(addresses[2][1], ~0);

      led_state[0] = 0;
      led_state[1] = 1;
      led_state[2] = 0;
      LOG("Led states not found resetting to default");
      break;
    }
  }
  LOG("Led states loaded");
  for (int i = 0; i < 3; i++) {
    LOG("LED %d: %s", i, led_state[i] ? "ON" : "OFF");
  }
  // Loop forever
  while (true) {
    for (int i = 0; i < 3; i++) {
      if (button_pressed(buttons[i])) {
        if (!button_held[i]) {
          button_held[i] = true;
          led_state[i] = !led_state[i];

          eeprom_write_byte(addresses[i][0], led_state[i]);
          eeprom_write_byte(addresses[i][1], (uint8_t)(~led_state[i]));

          LOG("LED %d: %s", i, led_state[i] ? "ON" : "OFF");
        }
      } else {
        button_held[i] = false;
      }
      gpio_put(leds[i], led_state[i]);
      sleep_ms(100);
    }
  }
}
