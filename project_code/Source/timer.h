#pragma once
#include <stdint.h>

typedef int (*TimerFn)(void);

typedef struct{
  uint16_t duration_ms;
  TimerFn fn;
} Timer;

// creates a timer that has a duration of ms milliseconds
// each duration_ms the function timer_fn will be called with arguments timer args
struct Timer* Timer_create(uint16_t duration_ms, TimerFn timer_fn;

// stops and destroyes a timer
void Timer_destroy(struct Timer* timer);

// starts a timer
void Timer_start(struct Timer* timer);

// stops a timer
void Timer_stop(void);
