#include <string.h>
#include <stdlib.h>
#include "protocol.h"

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
void Request_deserialize(const char* buffer, struct Request* r){
  memcpy(r, buffer, sizeof(Request));
}
