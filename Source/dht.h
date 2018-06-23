#pragma once
#include <stdint.h>
#include "protocol.h"

enum DHT_ERROR_t;

//#define clockCyclesPerMicrosecond ( F_CPU / 1000000L )
#define CCxMS 16

//sets the pin where is connected the dht
void DHT_setup(uint8_t pin);

//returns the milliseconds since device started
unsigned long micros(void);

//reads humidity and temperature values read from the sensor into a log
void* DHT_readSensor(void* l);
