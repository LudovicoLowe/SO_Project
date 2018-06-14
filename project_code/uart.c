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

typedef struct {
  int tx_buffer[BUFFER_SIZE];
  volatile uint8_t tx_start;
  volatile uint8_t tx_end;
  volatile uint8_t tx_size;

  int rx_buffer[BUFFER_SIZE];
  volatile uint8_t rx_start;
  volatile uint8_t rx_end;
  volatile uint8_t rx_size;

  int baud;
  int uart_num; // hardware uart;
} UART;

static UART uart_0;

struct UART* UART_init(const char* device __attribute__((unused)), uint32_t baud) {
  UART* uart=&uart_0;
  uart->uart_num=0;

  switch(baud){
  case 57600: setBaud57600(); break;
  case 115200: setBaud115200(); break;
  default: return 0;
  }

  uart->tx_start=0;
  uart->tx_end=0;
  uart->tx_size=0;
  uart->rx_start=0;
  uart->rx_end=0;
  uart->rx_size=0;

  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);   /* Enable RX and TX */
  sei();
  return &uart_0;
}

// returns the free space in the buffer
int UART_rxbufferSize(struct UART* uart __attribute__((unused))) {
  return BUFFER_SIZE;
}

// returns the free occupied space in the buffer
int  UART_txBufferSize(struct UART* uart __attribute__((unused))) {
  return BUFFER_SIZE;
}

// number of chars available in rx buffer
int UART_rxBufferAvailable(UART* uart) {
  return uart->rx_size;
}

// number of chars available in rx buffer
int UART_txBufferAvailable(UART* uart) {
  return uart->tx_size;
}

// number of bytes left in rx buffer
int UART_rxBufferLeft(UART* uart){
  return BUFFER_SIZE - uart->rx_size;
}

// number of bytes left in tx buffer
int UART_txBufferLeft(UART* uart){
  return BUFFER_SIZE - uart->tx_size;
}

void UART_putChar(struct UART* uart, uint8_t c) {
  while (uart->tx_size>=BUFFER_SIZE);  //until there is some space in the buffer
  ATOMIC_BLOCK(ATOMIC_FORCEON){
    uart->tx_buffer[uart->tx_end]=c;
    BUFFER_PUT(uart->tx, BUFFER_SIZE);
  }
  UCSR0B |= _BV(UDRIE0); // enable transmit interrupt
}

uint8_t UART_getChar(struct UART* uart){
  while(uart->rx_size==0);  //untill there is nothing to read in the buffer
  uint8_t c;
  ATOMIC_BLOCK(ATOMIC_FORCEON){
    c=uart->rx_buffer[uart->rx_start];
    BUFFER_GET(uart->rx, BUFFER_SIZE);
  }
  return c;
}


ISR(USART0_RX_vect) {
  uint8_t c=UDR0;
  if (uart_0.rx_size<BUFFER_SIZE){
    uart_0.rx_buffer[uart_0.rx_end] = c;
    BUFFER_PUT(uart_0.rx, BUFFER_SIZE);
  }
}

ISR(USART0_UDRE_vect){
  if (! uart_0.tx_size){
    UCSR0B &= ~_BV(UDRIE0);
  } else {
    UDR0 = uart_0.tx_buffer[uart_0.tx_start];
    BUFFER_GET(uart_0.tx, BUFFER_SIZE);
  }
}
