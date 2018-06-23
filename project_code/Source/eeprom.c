#include <util/atomic.h>
#include "avr/eeprom.h"
#include "buffer_utils.h"
#include "eeprom.h"

static EEPROM_SPACE eeprom_s;

struct EEPROM_SPACE* EEPROM_init(void){
  struct EEPROM_SPACE* eeprom=&eeprom_s;

  eeprom->LOG_NUMBER=0;
  eeprom->b_start=0;
  eeprom->b_end=0;
  eeprom->b_size=0;

  return eeprom;
}

void EEPROM_read(void* dest, uint16_t size, struct EEPROM_SPACE* eeprom){
  uint8_t* d=(uint8_t*)dest;
  uint8_t* end=d+size;
  uint16_t s;
  while(d<end){
    s=eeprom->b_start;
    eeprom_busy_wait();
    while (eeprom->b_size==0);  //untill there is nothing to read in the buffer
    ATOMIC_BLOCK(ATOMIC_FORCEON){
      *d=eeprom_read_byte((uint8_t*)s);
      BUFFER_GET(eeprom->b, BUFFER_SIZE);
    }
    ++d;
  }
}

void EEPROM_write(const void* src, uint16_t size, struct EEPROM_SPACE* eeprom){
  uint8_t* s=(uint8_t*)src;
  uint8_t* end=s+size;
  uint16_t d;
  while(s<end){
    d=eeprom->b_end;
    eeprom_busy_wait();
    while (eeprom->b_size>=BUFFER_SIZE);  //until there is some space in the buffer
    ATOMIC_BLOCK(ATOMIC_FORCEON){
      eeprom_write_byte((uint8_t*)d, *s);
      BUFFER_PUT(eeprom->b, BUFFER_SIZE);
    }
    ++s;
  }
}
