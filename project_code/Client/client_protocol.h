#pragma once

typedef struct LOG{
  int n;
  int temperature;
  int humidity;
} LOG;

typedef enum {
  Req=0x1,
  Ans=0x2,
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
#define R_DIM=sizeof(Request);

typedef struct {
  Type type;
  LOG log;
} Answere;
#define A_DIM=sizeof(Request);

// converts a well formed packet into a string in dest
void Packet_serialize(char* dest, const void* h);

// returns a newly allocated packet read from the buffer
Type* Packet_deserialize(const char* buffer);
