#include "digio.h"
#include "pins.h"
#include <avr/io.h>
#include <avr/pgmspace.h>

//returns the number of digital pins
uint8_t  DigIO_numChannels(void){
  return PINS_NUM;
}

// initializes the digital io pins to be all input
void DigIO_init(void) {
  uint8_t nPins=DigIO_numChannels();
  for (int i=0; i<nPins; ++i) {
    // all pins as input
    DigIO_setDirection(i,0);
    // pull up on each pin
    DigIO_setValue(i,1);
  }
}

uint8_t DigIO_setDirection(uint8_t pin, PinDirection dir) {
  if (pin>=PINS_NUM) return 1;
  const Pin* mapping=pins+pin;
  uint8_t mask=1<<mapping->bit;
  if (dir)
    *(mapping->dir_register)|=mask;
  else
    *(mapping->dir_register)&=~mask;
  return 0;
}

PinDirection DigIO_getDirection(uint8_t pin){
  if (pin>=PINS_NUM) return 0;
  const Pin* mapping=pins+pin;
  uint8_t value=*(mapping->dir_register);
  return (value >> pins[pin].bit)&0x1;
}

void DigIO_setValue(uint8_t pin, uint8_t value) {
  if (pin>=PINS_NUM) return 1;
  const Pin* mapping=pins+pin;
  uint8_t mask=1<<mapping->bit;
  if (value)
    *(mapping->out_register)|=mask;
  else
    *(mapping->out_register)&=~mask;
}

uint8_t DigIO_getValue(uint8_t pin){
  if (pin>=PINS_NUM) return 0;
  const Pin* mapping=pins+pin;
  uint8_t value=*(mapping->in_register);
  return (value >> pins[pin].bit)&0x1;
}
