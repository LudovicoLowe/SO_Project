#pragma once
#include <stdint.h>
#include "packet_utils.h"

#define EEPROM_DIM (30 * LOG_DIM)

typedef struct EEPROM_SPACE{
  uint8_t LOG_NUMBER;
  volatile uint8_t b_start;
  volatile uint8_t b_end;
  volatile uint8_t b_size;
} EEPROM_SPACE;

// initializes the eeprom subsystem so that write/read operations will work on a ring buffer
struct EEPROM_SPACE* EEPROM_init(void);

// reads in dest size bytes from eeprom ring buffer
int EEPROM_read(void* dest, uint16_t size, struct EEPROM_SPACE* eeprom);

// writes in eeprom ring buffer size bytes from src
int EEPROM_write(const void* src,  uint16_t size, struct EEPROM_SPACE* eeprom);
