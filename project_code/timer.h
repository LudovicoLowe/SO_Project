#pragma once
#include <stdint.h>

struct Timer;

typedef void (*TimerFn)(void*);

// creates a timer that has a duration of ms milliseconds
// each duration_ms the function timer_fn will be called with arguments timer args
struct Timer* Timer_create(char* device, uint16_t duration_ms, TimerFn timer_fn, void* timer_args);

// stops and destroyes a timer
void Timer_destroy(struct Timer* timer);

// starts a timer
void Timer_start(struct Timer* timer);

// stops a timer
void Timer_stop(void);
