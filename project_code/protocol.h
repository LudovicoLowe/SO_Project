#pragma once

typedef struct LOG{
  unit8_t n;
  unit8_t temperature;
  unit8_t humidity;
} LOG;
#define LOG_DIM=sizeof(LOG)

typedef enum {
  REQ=0x0,
  ANS=0x1
} Type;

typedef enum {
  SetTimer=0x0,
  LogRequest=0x1
} ReqType;

typedef struct {
  Type type;
  ReqType req_type;
  unit16_t duration_s;
} Request;
#define R_DIM=sizeof(Request)

typedef struct {
  Type type;
  LOG log;
} Answer;
#define A_DIM=sizeof(Answer)

// converts a well formed packet into a string in dest
void Answer_serialize(char* dest, const struct Answer* a);

// returns a newly allocated packet read from the buffer
void Request_deserialize(const char* buffer, struct Request* r);
