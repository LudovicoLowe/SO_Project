#pragma once

typedef struct LOG{
  int n;
  int temperature;
  int humidity;
} LOG;

typedef enum {
  SetTimer=0x0,
  LogRequest=0x1
} Type;

typedef struct {
  Type type;
  int duration_s;
} Request;

typedef struct {
  LOG l;
} Answer;

#define LOG_DIM sizeof(LOG)
#define R_DIM sizeof(Request)
#define A_DIM sizeof(Answer)

// converts a well formed packet into a string in dest
void request_serialize(char* dest, const Request* r);

// returns a newly allocated packet read from the buffer
Answer* answer_deserialize(const char* buffer);
