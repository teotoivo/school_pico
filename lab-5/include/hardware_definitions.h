#ifndef HARDWARE_DEFINITIONS_H
#define HARDWARE_DEFINITIONS_H

#include <hardware/i2c.h>

#define LED_D3 20
#define LED_D2 21
#define LED_D1 22

#define SW_0 9
#define SW_1 8
#define SW_2 7

#define OPTO_FORK 28 // Input with pullup

#define STEPPER_CONTROLLER_0 2
#define STEPPER_CONTROLLER_1 3
#define STEPPER_CONTROLLER_2 6
#define STEPPER_CONTROLLER_3 13

#define PIEZO_SENSOR 27

// lorawan uses uart 1
#define UART_TX_PIN 4
#define UART_RX_PIN 5

#define EEPROM_ADDR 0x50
#define EEPROM_PORT i2c0
#define EEPROM_sda_pin 16
#define EEPROM_scl_pin 17

#endif // !HARDWARE_DEFINITIONS_H
