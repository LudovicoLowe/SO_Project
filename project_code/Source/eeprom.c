#include <util/atomic.h>
#include "avr/eeprom.h"
#include "buffer_utils.h"
#include "eeprom.h"

EEPROM_SPACE eeprom_s;

struct EEPROM_SPACE* EEPROM_init(void){
  struct EEPROM_SPACE* eeprom=&eeprom_s;

  eeprom->LOG_NUMBER=0;
  eeprom->b_start=0;
  eeprom->b_end=0;
  eeprom->b_size=0;

  return eeprom;
}

int EEPROM_read(void* dest, uint16_t size, struct EEPROM_SPACE* eeprom){
  uint8_t* d=(uint8_t*)dest;
  uint8_t* end=d+size;
  uint16_t s;
  while(d<end){
    s=eeprom->b_start;
    eeprom_busy_wait();
    if (eeprom->b_size==0) return 0;  //if there is nothing to read in the buffer, exit..
    ATOMIC_BLOCK(ATOMIC_FORCEON){
      *d=eeprom_read_byte((uint8_t*)s);
      BUFFER_GET(eeprom->b, EEPROM_DIM);
    }
    ++d;
  }
  return 1;
}

int EEPROM_write(const void* src, uint16_t size, struct EEPROM_SPACE* eeprom){
  uint8_t* s=(uint8_t*)src;
  uint8_t* end=s+size;
  uint16_t d;
  while(s<end){
    d=eeprom->b_end;
    eeprom_busy_wait();
    if (eeprom->b_size>=EEPROM_DIM) return 0;  //if there is some space in the buffer, exit..
    ATOMIC_BLOCK(ATOMIC_FORCEON){
      eeprom_write_byte((uint8_t*)d, *s);
      BUFFER_PUT(eeprom->b, EEPROM_DIM);
    }
    ++s;
  }
  return 1;
}
