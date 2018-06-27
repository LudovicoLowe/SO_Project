#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdint.h>
#include "buffer_utils.h"
#include "packet_utils.h"
#include "io.h"
#include "eeprom.h"
#include "uart.h"
#include "timer.h"
#include "dht.h"

static struct LOG lo;
static struct Request req;

typedef struct timer_args{
    struct LOG* l;
    struct EEPROM_SPACE* eeprom;
    uint8_t N_LOG;
    uint8_t pin;
} timer_args;

static struct timer_args arg;

void timerFn(void* args){
  timer_args* a=(timer_args*)args;
  while(DHT_readSensor(a->l, a->pin));
  a->l->n = a->N_LOG;
  //write logs in eeprom
  char buf[LOG_DIM];
  memset(buf, 0, LOG_DIM);  //clear buffer
  memcpy((void*)buf, (void*)a->l,LOG_DIM);  //serialize
  while(a->eeprom->b_size==BUFFER_SIZE);  //wait untill there is space to write
  EEPROM_write((void*)buf, LOG_DIM, a->eeprom);
  ++a->eeprom->LOG_NUMBER;
  ++a->N_LOG;
}

int main (void) {
  //initialization of the UART
  struct UART* uart=UART_init(115200);
    //initalization of the eeprom
  struct EEPROM_SPACE* eeprom=EEPROM_init();
  //initialization of the structures needed for comunication
  struct LOG* l=&lo;
  struct Request* r=&req;
  //initialization of DHT
  uint8_t DHT_PIN=UART_getChar(uart); //get the dht pin number from client
  struct timer_args* ar=&arg;
  ar->l=l;
  ar->eeprom=eeprom;
  ar->pin=DHT_PIN;
  ar->N_LOG=0;
  timerFn((void*)ar);  //register an initial log
  //initialization of the timer with a duration time of 60000 ms (1 minute)
  struct Timer* timer=Timer_create(60000, timerFn, (void*)ar);
  Timer_start(timer);
  //
  uint16_t i, n;
  char eeprom_buffer[LOG_DIM];
  char r_buffer[R_DIM];
  while(1) {
    //read request from UART
    memset(r_buffer, 0, R_DIM);  //clear the buffer where we read the Request to, each time
    for (i=0; i<R_DIM; ++i) r_buffer[i]=UART_getChar(uart);
    memcpy((void*)r, (void*)r_buffer, R_DIM);  //deserialize
    if (r->type==SetTimer) {
		Timer_stop();
		Timer_create((uint16_t)(&req)->duration_s * 1000, timerFn, (void*)ar);
        Timer_start(timer);
    }
    else if (r->type==LogRequest) {
		while (eeprom->LOG_NUMBER==0);  //wait there is something to read
        n=eeprom->LOG_NUMBER;
        for(i=0; i<n; ++i) {
          //read a log from eeprom
          memset(eeprom_buffer, 0, LOG_DIM);  //clear the buffer where we read the log to, each time
          EEPROM_read((void*)eeprom_buffer, LOG_DIM, eeprom);
          --eeprom->LOG_NUMBER;
          for(int i=0; i<LOG_DIM; ++i) UART_putChar((uint8_t) eeprom_buffer[i], uart);
          _delay_ms(1000); //wait 1 sec
        }
    }
    _delay_ms(5000); //wait 5 sec
  }
}
