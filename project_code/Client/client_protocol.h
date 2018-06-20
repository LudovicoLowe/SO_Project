#pragma once

typedef struct LOG{
  int n;
  int temperature;
  int humidity;
} LOG;
#define LOG_DIM=sizeof(LOG)

typedef enum {
  REQ=0x1,
  ANS=0x2,
} Type;

typedef enum {
  SetTimer=0x3,
  LogRequest=0x4,
} ReqType;

typedef struct {
  Type type;
  ReqType req_type;
  int duration_s;
} Request;
#define R_DIM=sizeof(Request)

typedef struct {
  Type type;
  LOG log;
} Answer;
#define A_DIM=sizeof(Answer)

// converts a well formed packet into a string in dest
void Packet_serialize(char* dest, const void* h);

// returns a newly allocated packet read from the buffer
Type* Packet_deserialize(const char* buffer);