#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "io.h"
#include "buffer_utils.h"
#include "eeprom.h"
#include "uart.h"
#include "timer.h"
#include "protocol.h"
#include "dht.h"

struct EEPROM_SPACE* eeprom_s;
struct Timer* timer;
struct LOG* l;
struct Request* req;
struct Answer* ans;

int main (void) {
  //initalization of the eeprom
  eeprom_s=EEPROM_init();
  //initialization of the UART
  UART_init((uint32_t)115200);
  //initialization of comunication protocol
  protocol_init(l, req, ans);
  //initialization of DHT
  DHT_setup(UART_getChar()); //get the dht pin number from client
  while (timerFn() == 0);  //register an initial log
  //initialization of the timer with a duration time of 60000 ms (1 minute)
  timer=Timer_create((uint16_t) 60000, timerFn);
  Timer_start(timer);
  //
  int i;
  uint8_t n;
  char eeprom_buffer[LOG_DIM];
  char r_buffer[R_DIM];
  char a_buffer[A_DIM];
  while(1) {
    //read request from UART
    memset(r_buffer, 0, R_DIM);  //clear the buffer where we read the Request to, each time
    for (i=0; i<R_DIM; ++i) r_buffer[i]=UART_getChar();
    memset(req, 0, R_DIM);
    request_deserialize(r_buffer, &req);
    switch(req->type) {
      case SetTimer:
      {
        Timer_destroy(timer);
        timer=Timer_create(req->duration_s * 1000, timerFn);
        Timer_start(timer);
      }
      case LogRequest:
      {
        n=(eeprom_s->size / LOG_DIM);
        for(i=0; i<n; ++i) {
          //read logs from eeprom
          memset(eeprom_buffer, 0, LOG_DIM);  //clear the buffer where we read the log to, each time
          EEPROM_read((void*)eeprom_buffer, LOG_DIM);
          memset(ans, 0, A_DIM);
          memcpy(&(ans->l), eeprom_buffer, LOG_DIM);
          answer_send(a_buffer);
          _delay_ms(1000); //wait 1 sec
        }
      }
    }
    _delay_ms(5000); //wait 5 sec
  }
}

void answer_send(char* a){
  memset(a, 0, A_DIM);
  answer_serialize(a, ans);
  for(int i=0; i<A_DIM; ++i) UART_putChar((uint8_t) a[i]);
}

int timerFn(){
  memset(l, 0, LOG_DIM);
  DHT_ERROR_t err=*((DHT_ERROR_t*)DHT_readSensor(&l));
  switch (err) {
    case DHT_TIMEOUT: return 0;
    case DHT_CHECKSUM_ERROR: return 0;
    case DHT_OK:
    {
      l->n=eeprom_s->LOG_NUMBER;
      ++(eeprom_s->LOG_NUMBER);
      //write logs in eeprom
      EEPROM_write((void*)l, LOG_DIM);
      return 1;
    }
  }
}
