#pragma once
#include <stdint.h>

typedef struct EEPROM_SPACE;

// initializes the eeprom subsystem so that write/read operations will work on a ring buffer
struct EEPROM_SPACE* EEPROM_init(void);
// reads in dest size bytes from eeprom ring buffer
void EEPROM_read(void* dest, struct EEPROM_SPACE* eeprom, uint16_t size);
// writes in eeprom ring buffer size bytes from src
void EEPROM_write(struct EEPROM_SPACE* eeprom, const void* src,  uint16_t size);
