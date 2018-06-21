#include <string.h>
#include <stdlib.h>
#include "client_protocol.h"

// converts a packet into a (preallocated) buffer
void request_serialize(char* dest, const Request* r){
    memset(dest, 0, R_DIM);
    memcpy(dest, r, R_DIM);
}

//reads a packet from a preallocated buffer
Answer* answer_deserialize(const char* buffer){
    Answer* ans=(Answer*)malloc(A_DIM);
    memset(ans, 0, A_DIM);
    memcpy(ans, buffer, A_DIM);
    return ans;
}
