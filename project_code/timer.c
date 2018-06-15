#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "timer.h"

// here we hook to timer 5 which is not used in any of the primary arduino pins
// its 16 bit and allows us a decent resolution


typedef struct Timer{
  uint16_t duration_ms;
  TimerFn fn;
  void* args;
} Timer;

static Timer timer;

// creates a timer that has a duration of ms milliseconds
// each duration_ms the function timer_fn will be called with arguments timer args
Timer* Timer_create(uint16_t duration_ms, TimerFn timer_fn, void* timer_args){
  memset(&timer, 0, sizeof(timer));
  Timer* t=&timer;
  timer->duration_ms=duration_ms;
  timer->fn=timer_fn;
  timer->args=timer_args;
  return t;
}

// stops and destroyes a timer
void Timer_destroy(struct Timer* t){
  Timer_stop();
  cli();
  memset(t, 0, sizeof(Timer));
  sei();
}

// starts a timer
void Timer_start(struct Timer* t){
  cli();
  uint16_t ocrval=(uint16_t)(15.62*t->duration_ms);
  TCCR5A = 0;
  TCCR5B = 0;
  OCR5A = ocrval;
  TCCR5B |= (1 << WGM52) | (1 << CS50) | (1 << CS52);
  TIMSK5 |= (1 << OCIE5A);
  sei();
}

// stops a timer
void Timer_stop(){
  cli();
  TIMSK5 &= ~(1 << OCIE5A);
  sei();
}

ISR(TIMER5_COMPA_vect) {
  TCNT5 = 0;
  (*timer.fn)(timer.args);
}
