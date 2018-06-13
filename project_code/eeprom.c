#include "eeprom.h"
#include "avr/eeprom.h"

void EEPROM_init(void){
}

void EEPROM_read(void* dest, const uint16_t src, uint16_t size){
  uint8_t * d=(uint8_t*)dest;
  uint16_t s=src;
  uint16_t end=src+size;
  while(s<end){
    eeprom_busy_wait();
    *d=eeprom_read_byte((uint8_t*)s);
    ++s;
    ++d;
  }
}

void EEPROM_write(uint16_t dest, const void* src,  uint16_t size){
  const uint8_t * s=(uint8_t*)src;
  const uint8_t * end=s+size;
  while(s<end){
    eeprom_busy_wait();
    eeprom_write_byte((uint8_t*)dest, *s);
    ++s;
    ++dest;
  }
}
