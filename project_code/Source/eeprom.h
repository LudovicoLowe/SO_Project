#pragma once
#include <stdint.h>

typedef struct{
  volatile uint8_t LOG_NUMBER;
  volatile uint8_t b_start;
  volatile uint8_t b_end;
  volatile uint8_t b_size;
} EEPROM_SPACE;

// initializes the eeprom subsystem so that write/read operations will work on a ring buffer
struct EEPROM_SPACE* EEPROM_init(void);
// reads in dest size bytes from eeprom ring buffer
void EEPROM_read(void* dest, uint16_t size);
// writes in eeprom ring buffer size bytes from src
void EEPROM_write(const void* src,  uint16_t size);
