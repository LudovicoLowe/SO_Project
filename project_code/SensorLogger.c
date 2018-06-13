#include <avr/io.h>
#include <util/delay.h>
#include "digio.h"

int main (void) {
  // all pins as inputs with pull up resistors
  DigIO_init()

  uint8_t termo_pin=6;
  uint8_t start_pin=0;
	while(1) {
    for (int i=0; i<DigIO_numChannels(); ++i){
      //cambio i valori dei registri di input relativi ai pin che non corrispondono a quello del termometro..
      if (i!=6) {
        uint8_t pin=(start_pin+i)%DigIO_numChannels();
        DigIO_setValue(pin,i%2);
      }
      //....
      ++start_pin;
      if (start_pin>=DigIO_numChannels()) start_pin=0;
      _delay_ms(1000); //aspetto 1 secondo
    }
	}
}
