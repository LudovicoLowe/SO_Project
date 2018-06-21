#include <string.h>
#include <stdlib.h>
#include "protocol.h"

// converts a packet into a (preallocated) buffer
void answer_serialize(char* dest, const struct Answer* a){
  memset(dest, 0, A_DIM);
  memcpy(dest, h, A_DIM);
}

//reads a packet from a preallocated buffer
void request_deserialize(const char* buffer, struct Request* r){
  memset(r, 0, R_DIM);
  memcpy(r, buffer, R_DIM);
}
