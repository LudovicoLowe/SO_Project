#include "eeprom.h"
#include "buffer_utils.h"
#include "avr/eeprom.h"

static EEPROM_SPACE eeprom;

struct EEPROM_SPACE* EEPROM_init(void){
  EEPROM_SPACE* eeprom_s=&eeprom;

  eeprom_s->LOG_NUMBER=0;
  eeprom_s->b_start=0;
  eeprom_s->b_end=0;
  eeprom_s->b_size=0;

  return eeprom_s;
}

void EEPROM_read(void* dest, uint16_t size){
  uint8_t* d=(uint8_t*)dest;
  uint8_t end=d+(uint8_t)size;
  while(d<end){
    eeprom_busy_wait();
    while (eeprom->b_size==0);  //untill there is nothing to read in the buffer
    ATOMIC_BLOCK(ATOMIC_FORCEON){
      *d=eeprom_read_byte(eeprom->b_start);
      BUFFER_GET(eeprom->b, BUFFER_SIZE);
    }
    ++d;
  }
}

void EEPROM_write(const void* src, uint16_t size){
  uint8_t* s=(uint8_t*)src;
  uint8_t* end=s+(uint8_t)size;
  while(s<end){
    eeprom_busy_wait();
    while (eeprom->b_size>=BUFFER_SIZE);  //until there is some space in the buffer
    ATOMIC_BLOCK(ATOMIC_FORCEON){
      eeprom_write_byte(eeprom->b_end, *s);
      BUFFER_PUT(eeprom->b, BUFFER_SIZE);
    }
    ++s;
  }
}
