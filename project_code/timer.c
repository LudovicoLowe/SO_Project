#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "timer.h"

// here we hook to timer 5 which is not used in any of the primary arduino pins
// its 16 bit and allows us a decent resolution


typedef struct{
  uint16_t duration_ms;
  TimerFn fn;
} Timer;

static Timer timer;

// creates a timer that has a duration of ms milliseconds
// each duration_ms the function timer_fn will be called with arguments timer args
struct Timer* Timer_create(uint16_t duration_ms, TimerFn timer_fn){
  //memset(&timer, 0, sizeof(Timer));
  Timer* t=&timer;
  timer->duration_ms=duration_ms;
  timer->fn=timer_fn;
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
  uint16_t ocrval=(uint16_t)(15.62*timer->duration_ms);
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
  while ((*timer.fn)() == 0);
}
