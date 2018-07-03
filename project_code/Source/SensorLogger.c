#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "buffer_utils.h"
#include "packet_utils.h"
#include "io.h"
#include "eeprom.h"
#include "uart.h"
#include "timer.h"
#include "dht.h"

typedef struct timer_args{
    struct EEPROM_SPACE* eeprom;
    uint8_t N_LOG;  //progressive identificative number of logs
    uint8_t pin;
} timer_args;

static struct timer_args arg;

void answer_send(char* msg, struct UART* uart){
  int len = strlen(msg);
	for(int i=0; i<len; ++i, ++msg) UART_putChar((uint8_t)*msg, uart);
  _delay_ms(1000); //wait 1 sec
}

void timerFn(void* args){
  timer_args* a=(timer_args*)args;
  LOG l;
  (&l)->n = a->N_LOG;
  while(DHT_readSensor(&l, a->pin));
  //write logs in eeprom
  uint8_t buf[LOG_DIM];
  memset(buf, 0, LOG_DIM);  //clear buffer
  memcpy((void*)buf, (void*)&l, LOG_DIM);  //serialize
  if (EEPROM_write((void*)buf, LOG_DIM, a->eeprom)) {
    ++a->eeprom->LOG_NUMBER;
  /*
  char mesg[BUFFER_SIZE];
  memset(mesg, 0, BUFFER_SIZE);
  sprintf(mesg, "MSG: Timer proc.. registrating log %d!\n", a->N_LOG);
  answer_send(mesg, a->uart);
  */
    ++a->N_LOG;
  }
}

int main (void){
  //initialization of the UART
  struct UART* uart=UART_init(115200);
  answer_send("MSG: UART initialized!\n", uart);
  //initalization of the eeprom
  struct EEPROM_SPACE* eeprom=EEPROM_init();
  answer_send("MSG: EEPROM space initialized!\n", uart);
  //initialization of DHT
  //uint8_t DHT_PIN=UART_getChar(uart); //get the dht pin number from client

  uint8_t DHT_PIN=8;
  struct timer_args* ar=&arg;
  ar->eeprom=eeprom;
  ar->pin=DHT_PIN;
  ar->N_LOG=0;
  answer_send("MSG: Going to register an initial log into the EEPROM..\n", uart);
  timerFn((void*)ar);  //register an initial log
  answer_send("MSG: Done. LOG 0 registrated..\n", uart);

  //initialization of the timer with a duration time of 60000 ms (1 minute)
  struct Timer* timer=Timer_create(60000, timerFn, (void*)ar);
  Timer_start(timer);
  answer_send("MSG: Timer started with a period of 1 min..\n", uart);
  //
  uint8_t r_buffer[BUFFER_SIZE];
  uint8_t eeprom_buffer[BUFFER_SIZE];
  uint16_t i, n;
  while(1) {
    answer_send("MSG: Now waiting for request..\n", uart);
    Request req;
    memset(r_buffer, 0, BUFFER_SIZE);  //clear the buffer where we read the Request to, each time
    for (i=0; i<R_DIM; ++i) {
      uint8_t c=UART_getChar(uart);
      r_buffer[i]=c;
    }
    memcpy((void*)&req, (void*)r_buffer, R_DIM);  //deserialize
    if ((&req)->type==SetTimer) {
      answer_send("MSG: Request to reset the log-writing period of the timer received..\n", uart);
      Timer_stop();
      Timer_create((uint16_t)(&req)->duration_s * 1000, timerFn, (void*)ar);
      Timer_start(timer);
      answer_send("MSG: Done. New period is: k seconds\n", uart);
    }
    else if ((&req)->type==LogRequest) {
      answer_send("MSG: Request to send the sequence of the log registrated till now received..\n", uart);
      if (eeprom->LOG_NUMBER==0) answer_send("MSG: no log found.. Going to skip..\n", uart);
      else {
      n=eeprom->LOG_NUMBER;
      memset(mesg, 0, BUFFER_SIZE);
      sprintf(mesg, "MSG: %d logs found.. Going to send 1 log per second..\n", n);
      answer_send(mesg, uart);
      for(i=0; i<n; ++i) {
        //read a log from eeprom
        memset(eeprom_buffer, 0, BUFFER_SIZE);  //clear the buffer where we read the log to, each time
        if (EEPROM_read((void*)eeprom_buffer, LOG_DIM, eeprom)) {
          --eeprom->LOG_NUMBER;
          LOG l;
          memcpy((void*)&l, (void*)eeprom_buffer, LOG_DIM);
          memset(mesg, 0, BUFFER_SIZE);
          sprintf(mesg, "LOG: %d Temperature: %d Humidity: %d\n", (&l)->n, (&l)->temperature, (&l)->humidity);
          answer_send(mesg, uart);
        }
        _delay_ms(1000); //wait 1 sec
      }
      answer_send("MSG: Done. Senging sequence terminated successfully..\n", uart);
    }
    answer_send("MSG: In loop.. waiting 5 sec..\n", uart);
    _delay_ms(5000); //wait 5 sec
  }
}
