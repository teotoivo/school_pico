#ifndef EEPROM_H
#define EEPROM_H
#include <stdint.h>

#define EEPROM_MAX_ADDRESS 0x7FFF

#define EEPROM_BAUD_RATE 100000 // 100KHz

uint8_t eeprom_read_byte(uint16_t memory_address);
void eeprom_write_byte(uint16_t memory_address, uint8_t data);

#endif // !EEPROM_H
