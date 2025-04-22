#include "loging.h"
#include "eeprom.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint16_t crc16(const uint8_t *data_p, size_t length) {
  uint8_t x;
  uint16_t crc = 0xFFFF;
  while (length--) {
    x = crc >> 8 ^ *data_p++;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^
          ((uint16_t)x);
  }
  return crc;
}

void write_log_entry(const char *log_entry) {
  uint8_t buffer[LOG_ENTRY_SIZE];
  size_t len = strlen(log_entry);

  // ensure string fits in LOG_ENTRY_SIZE - 4
  // first byte marker for if entry being used
  // last 3 \0 crc high crc low
  if (len > LOG_ENTRY_SIZE) {
    printf("ERROR: Log message too long max %d bytes\n", LOG_ENTRY_SIZE - 2);
    return;
  }

  memcpy(buffer, log_entry, len + 1); // +1 for \0

  uint16_t crc = crc16(buffer, len + 1);
  // add crc to the end of buffer
  buffer[LOG_ENTRY_SIZE - 2] = (crc >> 8) & 0xFF; // high byte
  buffer[LOG_ENTRY_SIZE - 1] = crc & 0xFF;        // low byte

  uint16_t log_address = 0;
  for (uint32_t i = 0; i < MAX_LOG_ENTRIES; i++) {
    uint16_t current_address = i * LOG_ENTRY_SIZE;
    if (eeprom_read_byte(current_address) == 0) {
      // empty entry
      log_address = current_address;
      break;
    }
  }

  // if log is full / empty erase it
  if (log_address == 0) {
    erase_log();
    printf("Log full erasing\n");
  }

  for (uint32_t i = 0; i < LOG_ENTRY_SIZE; i++) {
    eeprom_write_byte(log_address + i, buffer[i]);
  }
}

void read_log() {
  char log_buffer[LOG_ENTRY_SIZE - 2]; // -2 for excluding crc
  for (int i = 0; i < MAX_LOG_ENTRIES; i++) {
    uint16_t log_address = i * 64;
    if (eeprom_read_byte(log_address) == 0) { // Empty entry
      break;                                  // No more entries
    }

    // Read the log entry
    for (int j = 0; j < LOG_ENTRY_SIZE - 3; j++) { // -3 for \0 and crc
      log_buffer[j] = eeprom_read_byte(log_address + j);
      if (log_buffer[j] == 0) {
        log_buffer[j] = '\0'; // Ensure null termination
        break;
      }
    }

    // Read the CRC
    uint16_t stored_crc =
        (eeprom_read_byte(log_address + LOG_ENTRY_SIZE - 2) << 8) |
        eeprom_read_byte(log_address + LOG_ENTRY_SIZE - 1);

    // Validate CRC
    if (crc16((uint8_t *)log_buffer, strlen(log_buffer) + 1) != stored_crc) {
      printf("Invalid log entry: CRC mismatch\n");
      break;
    }

    // Print the valid log entry
    printf("> %s\n", log_buffer);
  }
}

void erase_log() {
  for (int i = 0; i < MAX_LOG_ENTRIES; i++) {
    uint16_t log_address = i * LOG_ENTRY_SIZE;
    eeprom_write_byte(log_address, 0); // Invalidating the log entry
  }
}
