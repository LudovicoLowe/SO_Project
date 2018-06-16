#include <stdio.h>
#include <string.h>
#include "serial_linux.h"
#include "client_protocol.h"

typedef struct LOG{
  int temperature;
  int humidity;
} LOG;
static int LOG_SIZE=sizeof(LOG);

typedef enum {
  SetTimer=0x1,
  LogRequest=0x2,
  LogSent=0x3
} Type;

typedef struct {
  Type type;
  LOG log;
  //timer set up
} Packet;

int main(int argc, char** argv){
  if (argc < 1) {
    printf("%s <device name>\n", argv[0]);
    exit(0);
  }
  const char* device=argv[1];
  //serial port initialization
  int fd=serial_open(device);
  if (fd<0) {
    printf("Failed\n");
    exit(-1);
  }
  if (serial_set_interface_attribs(fd, 115200, 0) <0) return 0;  //set serial port attributes [speed, parity]
  serial_set_blocking(fd, 1);  //set serial port to be blocking
  if (!fd) {
    printf("Failed\n");
    exit(-1);
  }

  //....
}
