#include <unistd.h>
#include "client_protocol.h"
#include "buffer_utils.h"

static struct PacketHandler pack;

struct PacketHandler* PacketHandler_init(void) {
  PacketHandler* p=&pack;

  p->tx_start=0;
  p->tx_end=0;
  p->tx_size=0;

  return p;
}

static uint8_t getByte(PacketHandler* h){
  while(h->tx_size==0);  //untill there is nothing to read in the buffer
  uint8_t c=h->tx_buffer[h->tx_start];
  BUFFER_GET(h->tx, BUFFER_SIZE);
  return c;
}

void FlushBuffer(int fd, PacketHandler* h){
  while(h->tx_size){
    uint8_t c=getByte(h);
    write(fd, &c, 1);
  }
}

static void putByte(PacketHandler* h, uint8_t c){
  while (h->tx_size>=BUFFER_SIZE);  //until there is some space in the buffer
  h->tx_buffer[h->tx_end]=c;
  BUFFER_PUT(h->tx, BUFFER_SIZE);
}

int LoadPacket(PacketHandler* h, Request* r) {
  //check there is enough room in the buffer
  if ((BUFFER_SIZE-h->tx_size)<(R_DIM+3)) return 1;

  putByte(h, 0xAA);
  putByte(h, 0x55);
  h->tx_checksum=0;
  uint8_t size=R_DIM;
  uint8_t* buf=(uint8_t*)r;
  while(size){
    h->tx_checksum^=*buf;
    putByte(h,*buf);
    --size;
    ++buf;
  }
  putByte(h, h->tx_checksum);
  return 0;
}
