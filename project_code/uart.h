#pragma once
#include <stdint.h>

struct UART;

// initializes a uart object
// returns 0 on failure
struct UART*  UART_init(const char* device, uint32_t baud);

// puts a character in the buffer of a uart
// if the buffer is full, the function waits until there is room in the buffer
void UART_putChar(struct UART* uart, uint8_t c);

// returns a character from the uart.
// if no character is available, the function waits
uint8_t UART_getChar(struct UART* uart);

// returns the size of the rx buffer
int  UART_rxBufferSize(struct UART* uart);

// returns the size of the tx buffer
int  UART_txBufferSize(struct UART* uart);

// returns the number of characters to be read fron the rx buffer
int UART_rxBufferAvailable(struct UART* uart);

// returns the number of characters to be read fron the tx buffer
int UART_txBufferAvailable(struct UART* uart);

// returns the number of available characters in the rx buffer
int UART_rxBufferLeft(struct UART* uart);

// returns the number of available characters in the tx buffer
int UART_txBufferLeft(struct UART* uart);