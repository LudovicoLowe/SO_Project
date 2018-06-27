#pragma once
#include <stdint.h>

typedef void (*TimerFn)(void*);

struct Timer;

// creates a timer that has a duration of ms milliseconds
// each duration_ms the function timer_fn will be called with arguments timer args
struct Timer* Timer_create(uint16_t duration_ms, TimerFn timer_fn, void* args);

// starts a timer
void Timer_start(struct Timer* timer);

// stops a timer
void Timer_stop(void);
