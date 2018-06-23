#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include "serial_linux.h"
#include "client_protocol.h"

int RUNNING=1;

typedef struct {
    int fd;
} ARG_T;

void request_send(const int fd, const char* buf) {
  int ret;
  uint8_t bytes_sent=0;
  while (bytes_sent<R_DIM) {
    ret=write(fd, buf+bytes_sent, sizeof(uint8_t));
    if (ret == -1 && errno==EINTR) continue;
    else if (ret<0) {
      fprintf(stderr, "Error writing on descriptor: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    ++bytes_sent;
  }
}

void* display_routine(void* arg) {
  int ret, fd=((ARG_T*)arg)->fd;
  uint8_t bytes_read;
  char buf[A_DIM];
  while(RUNNING) {
    bytes_read=0;
    while (bytes_read<A_DIM) {
      ret=read(fd, buf+bytes_read, sizeof(uint8_t));
      if (ret == -1 && errno == EINTR) continue;
      else if (ret<0) {
        fprintf(stderr, "Error reading on descriptor: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
      }
      else if (ret==0) {
        fprintf(stderr, "Connection closed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
      }
      ++bytes_read;
    }
    Answer* a=answer_deserialize(buf);
    printf("[From Sensor Logger]\n  \
            LOG: %d Temperature: %d Humidity: %d\n",
            (int)((&(a->l))->n), (int)((&(a->l))->temperature), (int)((&(a->l))->humidity));
    free(a);
  }
  pthread_exit(NULL);
}

int main(int argc, char** argv){
  char device[128];
  memset(device, 0, sizeof(device));
  printf("Please enter the name of the divice: ");
  if (fgets(device, sizeof(device), stdin) != (char*)device) {
    fprintf(stderr, "Error reading from stdin, exiting..\n");
    exit(EXIT_FAILURE);
  }
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
  ARG_T* arg=(ARG_T*)malloc(sizeof(ARG_T));
  arg->fd=fd;
  pthread_t t;
  if (pthread_create(&t, NULL, display_routine, (void*)arg) < 0) {
    fprintf(stderr, "Error creating the message display thread: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  //
  printf("Wellcome!\n");
  char pin[2];
  uint8_t  pin_number;
  while(1) {
    printf("Please choose the digital pin number where the sensor is connected: [1 to 14]\n");
    if (fgets(pin, 2*sizeof(char), stdin) != (char*)pin) {
      fprintf(stderr, "Error reading from stdin, exiting..\n");
      exit(EXIT_FAILURE);
    }
    pin_number=(uint8_t)atoi(pin);
    if (pin_number<1 || pin_number>14) {
      printf("Inserted value out of range, or not a number!\n  \
              Please retry..\n");
      continue;
    }
    else {
      --pin_number;
      break;
    }
  }
  //send pin number to the microcontroller
  int ret;
  while(1) {
    ret=write(fd, &pin_number, sizeof(uint8_t));
    if (ret == -1 && errno==EINTR) continue;
    else if (ret<0) {
      fprintf(stderr, "Error writing on descriptor: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    else break;
  }
  //
  char command;
  char req_buffer[R_DIM];
  char seconds[128];
  while(1) {
    printf("Choose one of the following operations, by entering the corrispondent character:\n  \
            -[a] --> to Request the logs registrated by the Sensor Logger till now.\n  \
            -[b] --> to Set the period after which the Sensor Logger shall register a log.\n  \
            -[q] --> to Quit.\n");
    //Read a line from stdin
    if (fgets(&command, sizeof(char), stdin) != (char*)&command) {
      fprintf(stderr, "Error reading from stdin, exiting..\n");
      exit(EXIT_FAILURE);
    }
    switch(command) {
      case 'a':
      {
        Request* req=(Request*)malloc(R_DIM);
        req->type=LogRequest;
        request_serialize(req_buffer, req);
        request_send(fd, req_buffer);
        printf("Log Request sent!\n");
        free(req);
      }
      case 'b':
      {
        while(1) {
          printf("Insert the new period (seconds): ");
          memset(seconds, 0, sizeof(seconds));  //clear buffer each time
          if (fgets(seconds, sizeof(seconds), stdin) != (char*)seconds) {
            fprintf(stderr, "Error reading from stdin, exiting..\n");
            exit(EXIT_FAILURE);
          }
          if (atoi(seconds)==0) {
            printf("Inserted value is not a number! (or is 0..)\n  \
                    Please retry..\n");
            continue;
          }
          else break;
        }
        Request* req=(Request*)malloc(R_DIM);
        req->type=SetTimer;
        req->duration_s=(uint16_t)atoi(seconds);
        request_serialize(req_buffer, req);
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
      default: printf("Command not recognised! Please try again..\n");
    }
  }
  //join on the display routine thread
  if (pthread_join(t, NULL) < 0) {
    fprintf(stderr, "Error joining the message display thread: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  //exit operations
  if (close(fd) < 0) {
    fprintf(stderr, "Error closing the descriptor: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  printf("Bye Bye!\n");
  return 0;
}
