#include <string.h>
#include <stdlib.h>
#include "client_protocol.h"

// converts a packet into a (preallocated) buffer
void Packet_serialize(char* dest, const void* h){
  const Type* t=(Type*) h;
  switch(t->type) {
    case REQ:
    {
      memcpy(dest, h, sizeof(Request));
    }
    case ANS:
    {
      memcpy(dest, h, sizeof(Answer));
    }
  }
}

//reads a packet from a preallocated buffer
Type* Packet_deserialize(const char* buffer){
  const Type* t=(Type*) buffer;
  switch(t->type) {
    case REQ:
    {
      Request* req=(Request*)malloc(R_DIM);
      memcpy(req, buffer, R_DIM);
      return (Type*)req;
    }
    case ANS:
    {
      Answer* ans=(Answer*)malloc(A_DIM);
      memcpy(ans, buffer, A_DIM);
      return (Type*)ans;
    }
  }
}
