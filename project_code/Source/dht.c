#include <avr/io.h>
#include <avr/interrupt.h>
#include "dht.h"
#include "pins.h"
#include "io.h"

typedef enum DHT_ERROR_t{
  DHT_OK=0x0,
  DHT_TIMEOUT=0x1,
  DHT_CHECKSUM_ERROR=0x2
} DHT_ERROR_t;

static uint8_t DHT_PIN;

void DHT_setup(uint8_t pin) {
  DHT_PIN=pin;
}

unsigned long micros() {
	unsigned long m;
	uint8_t oldSREG = SREG, t;

	cli();
	m = timer0_overflow_count;
#if defined(TCNT0)
	t = TCNT0;
#elif defined(TCNT0L)
	t = TCNT0L;
#else
	#error TIMER 0 not defined
#endif


#ifdef TIFR0
	if ((TIFR0 & _BV(TOV0)) && (t & 255))
		m++;
#else
	if ((TIFR & _BV(TOV0)) && (t & 255))
		m++;
#endif

	SREG = oldSREG;

	return ((m << 8) + t) * (64 / CCxMS);
}

void* DHT_readSensor(struct LOG* l) {
    DHT_ERROR_t error;

	uint8_t buf[5];  // buffer to recieve
	uint8_t bit = 7;  // byte = 8 bit
	uint8_t idx = 0;

	//empty buffer
  memset(buf, 0, sizeof(buf));

	//request
  DigIO_setDirection(DHT_PIN, 1);
  DigIO_setValue(DHT_PIN, 0);
	_delay_ms(18);
	DigIO_setValue(DHT_PIN, 1);
	_delay_ms(40);
	DigIO_setDirection(DHT_PIN, 0);

	//acknowledge or timeout
	unsigned int cnt = 10000;
	while(DigIO_getValue(DHT_PIN) == 0) {if (--cnt == 0) {error=DHT_TIMEOUT; return (void*)&error;}}
	cnt = 10000;
	while(DigIO_getValue(DHT_PIN) == 1) {if (--cnt == 0) {error=DHT_TIMEOUT; return (void*)&error;}}

	//read output or timeout
  unsigned long t;
	for (int i=0; i<40; ++i) {  //data = 5 bytes = 40 bytes
		loopCnt = 10000;
		while(DigIO_getValue(DHT_PIN) == 0) {if (--cnt == 0) {error=DHT_TIMEOUT; return (void*)&error;}}
		t=micros();  //register moment when sensor writing signal starts
		loopCnt = 10000;
		while(DigIO_getValue(DHT_PIN) == 1) {if (--cnt == 0) {error=DHT_TIMEOUT; return (void*)&error;}}

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
	if (buf[4] != (buf[0] + buf[2])) {error=DHT_CHECKSUM_ERROR; return (void*)&error;}

    error=DHT_OK;
	return (void*)&error;
}
