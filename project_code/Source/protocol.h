#pragma once
#include <stdint.h>

struct LOG, struct Request, struct Answer;
enum Type;

// initializes the structures needed for comunication protocol
void protocol_init(struct LOG* lo, struct Request* re, struct Answer* an);

// converts an answer into a preallocated buffer
void answer_serialize(char* dest, const struct Answer* a);

// converts an answer into a preallocated buffer
void request_deserialize(const char* buffer, struct Request* r);
