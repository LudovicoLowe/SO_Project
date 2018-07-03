#pragma once
#include <stdint.h>

#define BUFFER_SIZE 256

typedef struct LOG{
  uint8_t n;
  uint8_t temperature;
  uint8_t humidity;
} LOG;

typedef enum Type{
  SetTimer = 0x0,
  LogRequest = 0x1,
} Type;

typedef struct Request{
  Type type;
  uint16_t duration_s;
} Request;

#define LOG_DIM sizeof(LOG)
#define R_DIM sizeof(Request)

typedef struct PacketHandler{
  uint8_t tx_buffer[BUFFER_SIZE];

  uint8_t tx_start;
  uint8_t tx_end;
  uint8_t tx_size;

  uint8_t tx_checksum;
} PacketHandler;

struct PacketHandler* PacketHandler_init(void);

void FlushBuffer(int fd, PacketHandler* h);

int LoadPacket(PacketHandler* h, Request* r);
