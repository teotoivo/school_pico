#include "LoRa.h"
#include "hardware_definitions.h"
#include "pico/stdlib.h"
#include "utils.h"
#include <hardware/gpio.h>
#include <hardware/structs/io_bank0.h>
#include <hardware/uart.h>
#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BAUD_RATE 9600

void program();

int main() {
  // Initialize chosen serial port
  stdio_init_all();

  printf("\033[2J\033[H"); // clears the screen

  bool program_active = false;

  init_button_pin(SW_0);

  // init uart
  uart_init(uart1, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  // Loop forever
  while (true) {
    if (button_pressed(SW_0)) {
      program_active = true;
    }

    if (program_active) {
      program_active = false;
      program();
    }
  }
}

void program() {
  const uint8_t MAX_TRIES = 5;

  while (true) {
    char res[MAX_LORA_STRING];

    bool connected = false;
    for (uint8_t i = 0; i < MAX_TRIES; i++) {
      if (send_command("AT\n", "OK", res, 500)) {
        printf("Connected\n");
        connected = true;
        break;
      }
    }

    if (!connected) {
      printf("Module not responding\n");
      break;
    }

    if (send_command("AT+VER\r", "+VER", res, 500)) {
      printf("Firmware version: %s\n", res);
    } else {
      printf("Module stopped responding\n");
      break;
    }

    break;
  }
}
