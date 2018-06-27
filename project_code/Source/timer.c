#include <avr/io.h>
#include <avr/interrupt.h>
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
struct Timer* Timer_create(uint16_t duration_ms, TimerFn timer_fn, void* args){
  Timer* t=&timer;
  t->duration_ms=duration_ms;
  t->fn=timer_fn;
  t->args=args;
  return t;
}

// starts a timer
void Timer_start(struct Timer* t){
  cli();
  uint16_t ocrval=(uint16_t)(15.62*t->duration_ms);
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = ocrval;
  TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

// stops a timer
void Timer_stop(){
  cli();
  TIMSK1 &= ~(1 << OCIE1A);
  sei();
}

ISR(TIMER1_COMPA_vect) {
  TCNT1 = 0;
  (*timer.fn)(timer.args);
}
