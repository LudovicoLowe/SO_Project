#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "serial_linux.h"
#include "client_protocol.h"

int RUNNING=1;

int main(int argc, char** argv){
  if (argc < 1) {
    printf("%s <device name>\n", argv[0]);
    exit(0);
  }
  const char* device=argv[1];
  //serial port initialization
  int fd=serial_open(device);
  if (fd<0) {
    fprintf(stderr, "Failed opening the serial port!\n");
    exit(EXIT_FAILURE);
  }
  if (serial_set_interface_attribs(fd, 115200, 0) <0) return 0;  //set serial port attributes [speed, parity]
  serial_set_blocking(fd, 1);  //set serial port to be blocking
  if (!fd) {
    fprintf(stderr, "Failed setting the attributes of the serial port!\n");
    exit(EXIT_FAILURE);
  }
  //display routine thread launch..
  pthread_t t;
  if (pthread_create(&t, NULL, display_routine, fd) < 0) {
    fprintf(stderr, "Error creating the message display thread: %s\n" strerror(errno));
    exit(EXIT_FAILURE);
  }
  //
  printf("Wellcome!\n");
  char command;
  char req_buffer[R_DIM];
  char buf[128];
  while(1) {
    printf("Choose one of the following operations, by sending the corrispondent character:\n
            -[a] --> to Request the logs registrated by the Sensor Logger till now.\n
            -[b] --> to Set the period after which the Sensor Logger shall register a log.\n
            -[q] --> to Quit.\n");
    //Read a line from stdin
    if (fgets(&command, sizeof(char), stdin) != (char*)&command) {
      fprintf(stderr, "Error reading from stdin, exiting..\n");
      exit(EXIT_FAILURE);
    }
    switch(buf) {
      case 'a':
      {
        Request* req=(Request*)malloc(R_DIM);
        req->type=LogRequest;
        Packet_serialize(req_buffer, req);
        request_send(fd, req_buffer);
        printf("Log Request sent!\n");
        free(req);
      }
      case 'b':
      {
        printf("Insert the new period (seconds): ");
        memset(buf, 0, sizeof(buf));  //clear buffer each time
        if (fgets(buf, sizeof(char), stdin) != (char*)buf) {
          fprintf(stderr, "Error reading from stdin, exiting..\n");
          exit(EXIT_FAILURE);
        }
        Request* req=(Request*)malloc(R_DIM);
        req->type=LogRequest;
        req->duration_s=atoi(buf);
        Packet_serialize(req_buffer, req);
        request_send(fd, req_buffer);
        printf("Timer Reset sent!\n");
        free(req);
      }
      case 'q':
      {
        printf("Exiting..\n");
        RUNNING=0;
        break;
      }
      case default: printf("Command not recognised! Please try again..\n");
    }
  }
  //join on the display routine thread
  if (pthread_join(t, NULL) < 0) {
    fprintf(stderr, "Error joining the message display thread: %s\n" strerror(errno));
    exit(EXIT_FAILURE);
  }
  //exit operations
  if (close(fd) < 0) {
    fprintf(stderr, "Error closing the descriptor: %s\n" strerror(errno));
    exit(EXIT_FAILURE);
  }
  printf("Bye Bye!\n");
  return 0;
}

void request_send(int fd, char* buf) {
  int ret, bytes_sent=0;
  while (bytes_sent<R_DIM) {
    ret=write(fd, buf+bytes_sent, 1);
    if (ret == -1 && errno==EINTR) continue;
    else if (ret<0) {
      fprintf(stderr, "Error writing on descriptor: %s\n" strerror(errno));
      exit(EXIT_FAILURE);
    }
    bytes_sent+=ret;
  }
}

void display_routine(int fd) {
  int ret, bytes_read;
  char buf[A_DIM];
  while(RUNNING) {
    bytes_read=0;
    while (bytes_read<A_DIM) {
      ret=read(fd, buf+bytes_read, 1);
      if (ret == -1 && errno == EINTR) continue;
      else if (ret<0) {
        fprintf(stderr, "Error reading on descriptor: %s\n" strerror(errno));
        exit(EXIT_FAILURE);
      }
      else if (ret==0) {
        fprintf(stderr, "Connection closed: %s\n" strerror(errno));
        exit(EXIT_FAILURE);
      }
      bytes_read+=ret;
    }
    Answer* a=(Answer*)malloc(A_DIM);
    a=(Answer*)Packet_deserialize(buf);
    printf("[From Sensor Logger]\n
            LOG: %d Temperature: %d Humidity: %d\n",
            a->log->n, a->log->temperature, a->log->humidity);
    free(a);
  }
  pthread_exit(NULL);
}
