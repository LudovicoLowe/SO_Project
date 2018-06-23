#pragma once

typedef struct LOG{
  uint8_t n;
  uint8_t temperature;
  uint8_t humidity;
} LOG;

typedef enum Type{
  SetTimer = 0x0,
  LogRequest = 0x1
} Type;

typedef struct Request{
  Type type;
  uint16_t duration_s;
} Request;

typedef struct Answer{
  struct LOG l;
} Answer;

#define LOG_DIM sizeof(LOG)
#define R_DIM sizeof(Request)
#define A_DIM sizeof(Answer)
