#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include "digio.h"
#include "buffer_utils.h"
#include "eeprom.h"
#include "uart.h"
#include "timer.h"
#include "protocol.h"

struct UART* uart;
struct EEPROM_SPACE* eeprom;
struct Timer* timer;
struct Request req;
int R_DIM=sizeof(Request);
struct Answer ans;
int A_DIM=sizeof(Answer);

void send(void){
  char a[sizeof(Answer)];
  Packet_serialize(a, ans)
  int dim=sizeof(Answer);
  int i;
  for(i=0; i<dim; ++i) UART_putChar(uart, (uint8_t) a[i]);
}

void timerFn(void* args){
  struct LOG* log;
  log->n=eeprom->LOG_NUMBER;
  log->temperature= *(args->temperature);
  log->humidity= *(args->humidity);
  log->msg="LOG 0: fa 18° a giugno porco il dio";
  ++(eeprom->LOG_NUMBER);
  //write logs in eeprom
  EEPROM_write(eeprom, &log, sizeof(LOG));
}

int main (void) {
  // all pins as inputs with pull up resistors
  DigIO_init();
  //initalization of the eeprom
  eeprom=EEPROM_init();
  //initialization of the UART
  uart=UART_init("uart_0",115200);
  //initialization of the timer with a duration time of 60000 ms (1 minute)
  //read logs.. and put them in arargs
  timer=Timer_create(60000, timerFn, (void*) &args);
  Timer_start(timer);
  //
  uint8_t termo_pin=6;
  uint8_t start_pin=0;
  //
  int i, n=(eeprom->size / sizeof(LOG);
  char eeprom_buffer[sizeof(LOG)];
  char re[sizeof(Request)];
	while(1) {
    //read request from UART
    memset(re, 0, sizeof(Request));  //clear the buffer where we read the Request to, each time
    for (i=0; i<sizeof(Request); ++i) re[idx]=UART_getChar(uart);
    memset(&req, 0, sizeof(Request));
    Request_deserialize(re, &req);
    switch(req->req_type) {
      case SetTimer:
      {
        //...
      }
      case LogRequest:
      {
        for(i=0; i<n); ++i) {
          //read logs from eeprom
          memset(eeprom_buffer, 0, sizeof(LOG));  //clear the buffer where we read the log to, each time
          EEPROM_read(eeprom_buffer, eeprom, sizeof(LOG));
          memset(&ans, 0, sizeof(Answer));
          ans->type=Ans;
          ans->log=(LOG)(*eeprom_buffer);
          send();
          _delay_ms(2000); //wait 2 sec
        }
      }
    }
    _delay_ms(5000); //wait 5 sec
	}
}
