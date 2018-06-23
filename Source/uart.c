#include "uart.h"
#include "buffer_utils.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/atomic.h>

void setBaud57600(void) {
#define BAUD 57600
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

#ifdef USE_2X
  UCSR0A |= (1<<U2X0);
#endif
#undef BAUD
}

void setBaud115200(void) {
#define BAUD 115200
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

#ifdef USE_2X
  UCSR0A |= (1<<U2X0);
#endif
#undef BAUD
}

static UART uart;

void UART_init(uint32_t baud) {
  UART* u=&uart;

  switch(baud){
  case 57600:
    {
      setBaud57600();
      u->baud=57600;
      break;
  case 115200:
    {
      setBaud115200();
      u->baud=115200;
      break;
    }
  default: return 0;
  }

  u->tx_start=0;
  u->tx_end=0;
  u->tx_size=0;
  u->rx_start=0;
  u->rx_end=0;
  u->rx_size=0;

  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);   /* Enable RX and TX */
  sei();
}

void UART_putChar(uint8_t c) {
  while (uart->tx_size>=BUFFER_SIZE);  //until there is some space in the buffer
  ATOMIC_BLOCK(ATOMIC_FORCEON){
    uart->tx_buffer[uart->tx_end]=c;
    BUFFER_PUT(uart->tx, BUFFER_SIZE);
  }
  UCSR0B |= _BV(UDRIE0); // enable transmit interrupt
}

uint8_t UART_getChar(void){
  while(uart->rx_size==0);  //untill there is nothing to read in the buffer
  uint8_t c;
  ATOMIC_BLOCK(ATOMIC_FORCEON){
    c=uart->rx_buffer[uart->rx_start];
    BUFFER_GET(uart->rx, BUFFER_SIZE);
  }
  return c;
}


ISR(USART_RX_vect) {
  uint8_t c=UDR0;
  if (uart.rx_size<BUFFER_SIZE){
    uart.rx_buffer[uart.rx_end] = c;
    BUFFER_PUT(uart.rx, BUFFER_SIZE);
  }
}

ISR(USART_UDRE_vect){
  if (! uart.tx_size){
    UCSR0B &= ~_BV(UDRIE0);
  } else {
    UDR0 = uart.tx_buffer[uart.tx_start];
    BUFFER_GET(uart.tx, BUFFER_SIZE);
  }
}
