#include <string.h>
#include <stdlib.h>
#include "client_protocol.h"

// converts a packet into a (preallocated) buffer
void Packet_serialize(char* dest, const void* h){
  const Type* t=(Type*) h;
  switch(t->type) {
    case Req:
    {
      memcpy(dest, h, sizeof(Request));
    }
    case Ans:
    {
      memcpy(dest, h, sizeof(Answere));
    }
  }
}

//reads a packet from a preallocated buffer
 Type* Packet_deserialize(const char* buffer){
  const Type* t=(Type*) buffer;
  switch(t->type) {
    case Req:
    {
      Request* req=(Request*)malloc(sizeof(Request));
      memcpy(req, buffer, sizeof(Request));
      return (Type*)req;
    }
    case Ans:
    {
      Answere* ans=(Answere*)malloc(sizeof(Answere));
      memcpy(ans, buffer, sizeof(Answere));
      return (Type*)ans;
    }
  }
}
