#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "packet_utils.h"
#include "io.h"
#include "dht.h"

volatile unsigned long t0_ovflw_cnt = 0;

unsigned long micros() {
    unsigned long m;
	uint8_t temp = SREG, t;

	cli();
    m=t0_ovflw_cnt;
	t = TCNT0;

	if ((TIFR0 & _BV(TOV0)) && (t < 255)) ++m;

	SREG = temp;

	return ((m << 8) + t) * (64 / CCxMS);
}

int DHT_readSensor(struct LOG* l, uint8_t pin) {
	uint8_t buf[5];  // buffer to recieve
	uint8_t bit = 7;  // byte = 8 bit
	uint8_t idx = 0;

	//empty buffer
  memset(buf, 0, sizeof(buf));

	//request
  DigIO_setDirection(pin, 1);
  DigIO_setValue(pin, 0);
	_delay_ms(18);
	DigIO_setValue(pin, 1);
	_delay_us(40);
	DigIO_setDirection(pin, 0);

	//acknowledge or timeout
	unsigned int cnt = 10000;
	while(DigIO_getValue(pin) == 0) if (--cnt == 0) return 1;
	cnt = 10000;
	while(DigIO_getValue(pin) == 1) if (--cnt == 0) return 1;

	//read output or timeout
  unsigned long t;
	for (int i=0; i<40; ++i) {  //data = 5 bytes = 40 bytes
		cnt = 10000;
		while(DigIO_getValue(pin) == 0) if (--cnt == 0) return 1;
		t=micros();  //register moment when sensor writing signal starts
		cnt = 10000;
		while(DigIO_getValue(pin) == 1) if (--cnt == 0) return 1;

    //[dht11 sensor writing signal lasts 40 ms]
    //--> if it passed at least 40 ms, it means bit is to be pull up
		if ((micros() - t) > 40) buf[idx] |= (1 << bit);
		if (bit == 0) {  // next byte?
			bit = 7;
			++idx;
		}
		else --bit;
	}

	//write values from buffer into log
  //(bits[1] and bits[3] are allways zero!)
	l->humidity=buf[0];
	l->temperature=buf[2];

  //checksum control
	if (buf[4] != (buf[0] + buf[2])) if (--cnt == 0) return 1;

	return 0;
}

ISR (TIMER0_OVF_vect) {
	++t0_ovflw_cnt;
}
