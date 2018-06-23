#include <string.h>
#include <stdlib.h>
#include "protocol.h"

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

static LOG l, Request r, Answer a;

// initializes the structures needed for comunication protocol
void protocol_init(struct LOG* lo, struct Request* re, struct Answer* an){
    lo=&l;
    re=&r;
    an=&a;
}

// converts an answer into a preallocated buffer
void answer_serialize(char* dest, const struct Answer* a){
  memset(dest, 0, A_DIM);
  memcpy(dest, a, A_DIM);
}

//reads a request from a preallocated buffer
void request_deserialize(const char* buffer, struct Request* r){
  memset(r, 0, R_DIM);
  memcpy(r, buffer, R_DIM);
}
