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
struct Answer ans;

void answer_send(void){
  char a[A_DIM];
  answer_serialize(a, &ans)
  int dim=A_DIM;
  int i;
  for(i=0; i<dim; ++i) UART_putChar(uart, (uint8_t) a[i]);
}

void timerFn(void* args){
  struct LOG* log;
  log->n=eeprom->LOG_NUMBER;
  log->temperature= *(args->temperature);
  log->humidity= *(args->humidity);
  ++(eeprom->LOG_NUMBER);
  //write logs in eeprom
  EEPROM_write(&log, sizeof(LOG));
}

int main (void) {
  // all pins as inputs with pull up resistors
  DigIO_init();
  //initalization of the eeprom
  eeprom=EEPROM_init();
  //initialization of the UART
  uart=UART_init(115200);
  //initialization of the timer with a duration time of 60000 ms (1 minute)
  //read logs.. and put them in args
  timer=Timer_create(60000, timerFn, (void*) &args);
  Timer_start(timer);
  //
  int i, n;
  char eeprom_buffer[sizeof(LOG)];
  char r_buffer[R_DIM];
	while(1) {
    //read request from UART
    memset(r_buffer, 0, R_DIM);  //clear the buffer where we read the Request to, each time
    for (i=0; i<R_DIM; ++i) re_buffer[i]=UART_getChar(uart);
    request_deserialize(r_buffer, &req);
    switch(req->req_type) {
      case SetTimer:
      {
        Timer_destroy(timer);
        timer=Timer_create((unit16_t)(req->duration_s * 1000), timerFn, (void*) &args);
        Timer_start(timer);
      }
      case LogRequest:
      {
        n=(eeprom->size / sizeof(LOG);
        for(i=0; i<n); ++i) {
          //read logs from eeprom
          memset(eeprom_buffer, 0, sizeof(LOG));  //clear the buffer where we read the log to, each time
          EEPROM_read(eeprom_buffer, sizeof(LOG));
          memset(&ans, 0, A_DIM);
          ans->type=Ans;
          memcpy(&(ans->log), eeprom_buffer, sizeof(LOG))
          answer_send();
          _delay_ms(1000); //wait 1 sec
        }
      }
    }
    _delay_ms(5000); //wait 5 sec
	}
}
