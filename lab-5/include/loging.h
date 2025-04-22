#ifndef LOGING_H
#define LOGING_H
#include <stddef.h>
#include <stdint.h>

#define MAX_LOG_ENTRIES 32
#define LOG_ENTRY_SIZE 64

uint16_t crc16(const uint8_t *data_p, size_t length);
void write_log_entry(const char *log_entry);
void erase_log();
void read_log();

#endif // !loging
