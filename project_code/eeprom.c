#include "eeprom.h"
#include "buffer_utils.h"
#include "avr/eeprom.h"

typedef struct EEPROM_SPACE{
  volatile uint8_t b_start;
  volatile uint8_t b_end;
  volatile uint8_t b_size;
} EEPROM_SPACE;

struct EEPROM_SPACE* EEPROM_init(void){
  struct EEPROM_SPACE eeprom;

  eeprom->b_start=0;
  eeprom->b_end=0;
  eeprom->b_size=0;

  return &eeprom;
}

void EEPROM_read(void* dest, struct EEPROM_SPACE* eeprom, uint16_t size){
  uint8_t * d=(uint8_t*)dest;
  uint16_t s=src;
  uint16_t end=src+size;
  while(s<end){
    eeprom_busy_wait();
    while(eeprom->b_size==0);  //untill there is nothing to read in the buffer
    ATOMIC_BLOCK(ATOMIC_FORCEON){
      *d=eeprom_read_byte(eeprom->b_start);
      BUFFER_GET(eeprom->b, BUFFER_SIZE);
    }
    ++d;
  }
}

void EEPROM_write(struct EEPROM_SPACE* eeprom, const void* src,  uint16_t size){
  const uint8_t * s=(uint8_t*)src;
  const uint8_t * end=s+size;
  while(s<end){
    eeprom_busy_wait();
    while (eeprom->b_size>=BUFFER_SIZE);  //until there is some space in the buffer
    ATOMIC_BLOCK(ATOMIC_FORCEON){
      eeprom_write_byte(eeprom->b_end, c);
      BUFFER_PUT(eeprom->b, BUFFER_SIZE);
    }
    ++s;
  }
}
