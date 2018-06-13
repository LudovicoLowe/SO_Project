#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "digio.h"
#include "eeprom.h"

int main (void) {
  // all pins as inputs with pull up resistors
  DigIO_init();
  //initalization of the eeprom
  EEPROM_init();

  char* log_data="fa 18° a giugno porco il dio";
  int size=strlen(log_data)+1;  //!
  char eeprom_buffer[size];

  uint8_t termo_pin=6;
  uint8_t start_pin=0;
	while(1) {
    if (0) {
      EEPROM_write(0, log_data, size);
      _delay_ms(1000); //wait 1 sec

      memset(eeprom_buffer, 0, size);  //clear the buffer where we read the message to, each time
      EEPROM_read(eeprom_buffer, 0, size);
      eeprom_buffer[size-1]=0;  //!
      _delay_ms(1000); //wait 1 sec
      //wrap it into a nice thing (a standard 128 buffer..)
      char tx_message[128];
      sprintf(tx_message, "buffer string:[%s]\n", eeprom_buffer);
      //..uart send protocol function..
    }
    //....
    _delay_ms(1000); //wait 1 sec
	}
}
