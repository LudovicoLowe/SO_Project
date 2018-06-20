#pragma once
#include <stdint.h>

struct UART;

// initializes a uart object
// returns 0 on failure
void UART_init(uint32_t baud);

// puts a character in the buffer of a uart
// if the buffer is full, the function waits until there is room in the buffer
void UART_putChar(uint8_t c);

// returns a character from the uart.
// if no character is available, the function waits
uint8_t UART_getChar(void);
