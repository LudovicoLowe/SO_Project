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
  //timer set up
} Request;

typedef struct {
  Type type;
  LOG log;
} Answere;

// converts a well formed packet into a string in dest
void Packet_serialize(char* dest, const void* h);

// returns a newly allocated packet read from the buffer
void Request_deserialize(const char* buffer, struct Request* r);
