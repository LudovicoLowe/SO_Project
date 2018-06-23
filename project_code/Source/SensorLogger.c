#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdint.h>
#include "packet_utils.h"
#include "io.h"
#include "eeprom.h"
#include "uart.h"
#include "timer.h"
#include "dht.h"

static struct LOG lo;
static struct Request req;
static struct Answer ans;

typedef struct timer_args{
    struct LOG* l;
    struct EEPROM_SPACE* eeprom;
    uint8_t pin;
} timer_args;

static struct timer_args arg;

void answer_send(struct Answer* a, char* a_buf, struct UART* uart){
  memset(a_buf, 0, A_DIM);  //clear buffer each time
  memcpy((void*)a, a_buf, A_DIM);  //serialize
  for(int i=0; i<A_DIM; ++i) UART_putChar((uint8_t) a_buf[i], uart);
}

int timerFn(void* args){
  timer_args* a=(timer_args*)args;
  int err=DHT_readSensor(a->l, a->pin);
  if (err) {
      a->l->n = a->eeprom->LOG_NUMBER;
      ++a->eeprom->LOG_NUMBER;
      //write logs in eeprom
      char buf[LOG_DIM];
      memset(buf, 0, A_DIM);  //clear buffer
      memcpy((void*)a->l, buf, LOG_DIM);  //serialize
      EEPROM_write((void*)buf, LOG_DIM, a->eeprom);
  }
  return err;
}

int main (void) {
  //initalization of the eeprom
  struct EEPROM_SPACE* eeprom=EEPROM_init();
  //initialization of the UART
  struct UART* uart=UART_init((uint32_t)115200);
  //initialization of the structures needed for comunication
  struct LOG* l=&lo;
  struct Answer* a=&ans;
  struct Request* r=&req;
  //initialization of DHT
  uint8_t DHT_PIN=UART_getChar(uart); //get the dht pin number from client
  struct timer_args* ar=&arg;
  ar->l=l;
  ar->eeprom=eeprom;
  ar->pin=DHT_PIN;
  while (timerFn((void*)ar) == 0);  //register an initial log
  //initialization of the timer with a duration time of 60000 ms (1 minute)
  struct Timer* timer=Timer_create((uint16_t) 60000, timerFn, (void*)ar);
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
    for (i=0; i<R_DIM; ++i) r_buffer[i]=UART_getChar(uart);
    memcpy((void*)r, r_buffer, R_DIM);  //deserialize
    switch(r->type) {
      case SetTimer:
      {
        Timer_stop();
        Timer_create((uint16_t)(&req)->duration_s * 1000, timerFn, (void*)ar);
        Timer_start(timer);
      }
      case LogRequest:
      {
        n=(eeprom->b_size / (uint8_t)LOG_DIM);
        for(i=0; i<n; ++i) {
          //read logs from eeprom
          memset(eeprom_buffer, 0, LOG_DIM);  //clear the buffer where we read the log to, each time
          EEPROM_read((void*)eeprom_buffer, LOG_DIM, eeprom);
          memcpy((void*)&a->l, eeprom_buffer, LOG_DIM);
          answer_send(a, a_buffer, uart);
          _delay_ms(1000); //wait 1 sec
        }
      }
    }
    _delay_ms(5000); //wait 5 sec
  }
}
