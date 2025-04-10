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

  absolute_time_t timeout_time = make_timeout_time_ms(timeout_ms);
  size_t i = 0;

  while (!time_reached(timeout_time)) {
    if (uart_is_readable(uart1)) {
      char c = uart_getc(uart1);

      if (i < MAX_LORA_STRING - 1) { // -1 to leave space for \0
        response_buffer[i++] = c;
      } else {
        break;
      }
    }
  }

  response_buffer[i] = '\0';

  printf("cmd: %s\nres:%s\n\n", cmd, response_buffer);

  if (strstr(response_buffer, expected_response) != NULL) {
    return true;
  }

  return false;
}
