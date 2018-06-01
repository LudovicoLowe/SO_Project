#include <avr/io.h>
#include <util/delay.h>
 
#define BLINK_DELAY_MS 1000
 
int main (void)
{
 /* set pin 5 of PORTB for output */
 /* pin5 di PORTB per l'output */
 DDRB |= _BV(DDB5);
 
 while(1) {
  /* set pin 5 high to turn led on */
  /* pin5 high accendere il led */
  PORTB |= _BV(PORTB5);
  _delay_ms(BLINK_DELAY_MS);
 
  /* set pin 5 low to turn led off */
  /* pin5 low per spegnere il led */
  PORTB &= ~_BV(PORTB5);
  _delay_ms(BLINK_DELAY_MS);
 }
}