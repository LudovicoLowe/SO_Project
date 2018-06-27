#include <string.h>
#include <stdlib.h>
#include "client_protocol.h"

// converts a packet into a (preallocated) buffer
void request_serialize(char* dest, const Request* r){
    memset(dest, 0, R_DIM);
    memcpy(dest, r, R_DIM);
}

//reads a packet from a preallocated buffer
LOG* answer_deserialize(const char* buffer){
    LOG* ans=(LOG*)malloc(LOG_DIM);
    memset(ans, 0, LOG_DIM);
    memcpy(ans, buffer, LOG_DIM);
    return ans;
}
