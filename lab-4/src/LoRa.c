#include "LoRa.h"
#include <hardware/timer.h>
#include <hardware/uart.h>
#include <pico/time.h>
#include <pico/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool send_command(const char *cmd, const char *expected_response,
                  char *response_buffer, uint32_t timeout_ms) {

  uart_write_blocking(uart1, (const uint8_t *)cmd, strlen(cmd));
  uart_write_blocking(uart1, (const uint8_t *)"\r\n", 2);

  size_t i = 0;

  if (uart_is_readable_within_us(uart1, 1000 * timeout_ms)) {
    while (uart_is_readable_within_us(uart1, 1000 * timeout_ms)) {
      char c = uart_getc(uart1);

      if (i < MAX_LORA_STRING - 1) { // -1 to leave space for \0
        response_buffer[i++] = c;
      } else {
        break;
      }
    }
  }

  response_buffer[i] = '\0';

  if (strstr(response_buffer, expected_response) != NULL) {
    return true;
  }

  return false;
}
