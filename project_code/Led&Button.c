#include <avr/io.h>
#include <util/delay.h>
 
#define delay 1000

int main (void) {
	DDRB = 0b00100000; // (PB5=D13)  Setto l'uscita PB5 come uscita
	DDRD = 0b00000000; // (PD2=D2)  Setto l'uscita PD2 come entrata
 
	while(1) {
		if (PIND == 0b00000010) { //se il valore letto dall'uscita PD2 è bit positivo (tensione alta -> pulsante premuto)
			PORTB = 0b00100000;   //accendo il led
		}
		else PORTB = 0b00000000;  //altrimenti lo spengo
		_delay_ms(delay);
	}
}