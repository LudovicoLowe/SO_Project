#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include "digio.h"
#include "buffer_utils.h"
#include "eeprom.h"
#include "uart.h"

struct UART* uart;
struct EEPROM_SPACE* eeprom;

void send(void* packet){
  in dim=sizeof(packet);
  for(int i=0; i<dim; ++i) UART_putByte(uart, (uint8_t)* packet);
}

int main (void) {
  // all pins as inputs with pull up resistors
  DigIO_init();
  //initalization of the eeprom
  eeprom=EEPROM_init();
  //initialization of the UART
  uart=UART_init("uart_0",115200);

  //
  uint8_t termo_pin=6;
  uint8_t start_pin=0;
  //
  char* log_data="fa 18° a giugno porco il dio";
  int size=strlen(log_data)+1;  //!
  char eeprom_buffer[size];
  //
  char tx_message[BUFFER_SIZE];
  char rx_message[BUFFER_SIZE];

	while(1) {
    //
    if (0) {
      //write logs in eeprom
      EEPROM_write(eeprom, log_data, size);
      _delay_ms(1000); //wait 1 sec

      //read logs in eeprom
      memset(eeprom_buffer, 0, size);  //clear the buffer where we read the message to, each time
      EEPROM_read(eeprom_buffer, eeprom, size);
      eeprom_buffer[size-1]=0;  //!
      _delay_ms(1000); //wait 1 sec
    }
    //
    if (0) {
      //copy log buffer into transfer buffer
      sprintf(tx_message, "msg: [%s]\n", eeprom_buffer);
      //send
      in dim=sizeof(tx_message);
      for(int i=0; i<dim; ++i) UART_putByte(uart, (uint8_t)* packet);
      _delay_ms(1000); //wait 1 sec

      //read request packet
      uint8_t x;
      int idx=0;
      while (1) {
        x= UART_getByte(uart);
        rx_message[idx]=c;
        ++idx;
        if (c=='\n' || c=='\r' || c==0) {
          rx_message[idx]=0;
          break;
        }
      }
      _delay_ms(1000); //wait 1 sec
    }
    //....
    _delay_ms(1000); //wait 1 sec
	}
}
