#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include "serial_linux.h"
#include "client_protocol.h"

typedef struct {
    int fd;
    int running;
} ARG_T;

void* display_routine(void* arg) {
  int ret, fd=((ARG_T*)arg)->fd;
  int* run=&((ARG_T*)arg)->running;
  char* buf=(char*)malloc(BUFFER_SIZE);
  uint16_t bytes_read;
  sleep(2);  //wait 2 sec before starting display functionality..
  while(*run) {
    memset(buf, 0, BUFFER_SIZE);
    bytes_read=0;
    while (*run) {
      ret=read(fd, buf+bytes_read, 1);
      //if (ret == -1 && errno == EAGAIN) continue;
      if (ret<0) {
        fprintf(stderr, "Error reading on descriptor: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
      }
      if (*(buf+bytes_read) == '\n') break;
      ++bytes_read;
    }
    printf("\n[From Sensor Logger]\n%s\n\n", buf);
  }
  printf("\n[Display Routine exiting..]\n\n");
  free(buf);
  pthread_exit(NULL);
}

int main(int argc, char** argv){
  /*
  char device[128];
  memset(device, 0, sizeof(device));
  printf("Please enter the name of the divice: [default: 'ttyACM0']");
  if (fgets(device, sizeof(device), stdin) != (char*)device) {
    fprintf(stderr, "Error reading from stdin, exiting..\n");
    exit(EXIT_FAILURE);
  }
  */
  //serial port initialization
  int fd=serial_open("/dev/ttyACM0");
  if (fd<0) {
    fprintf(stderr, "Failed opening the serial port!\n");
    exit(EXIT_FAILURE);
  }
  if (serial_set_interface_attribs(fd, 115200, 0) <0) {  //set serial port attributes [speed, parity]
    fprintf(stderr, "Failed setting serial port attributes: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  serial_set_blocking(fd, 1);  //set serial port to be blocking
  if (!fd) {
    fprintf(stderr, "Failed setting the attributes of the serial port!\n");
    exit(EXIT_FAILURE);
  }
  //initializzation tf the comunication protocol..
  struct PacketHandler* h=PacketHandler_init();
  //display routine thread launch..
  ARG_T* arg=(ARG_T*)malloc(sizeof(ARG_T));
  arg->fd=fd;
  arg->running=1;
  pthread_t t;
  if (pthread_create(&t, NULL, display_routine, (void*)arg) < 0) {
    fprintf(stderr, "Error creating the message display thread: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  //
  printf("Wellcome!\n");
  /*
  char pin[2];
  uint8_t  pin_number;
  while(1) {
    printf("Please choose the digital pin number where the sensor is connected: [1 - 14] [0 to skip]\n");
    scanf("%s", pin);
    pin_number=(uint8_t)atoi(pin);
    if (pin_number<0 || pin_number>14) {
      printf("Inserted value out of range, or not a number!\n  \
              Please retry..\n");
      continue;
    }
    else {
	  if (pin_number!=0) {
      --pin_number;
      //send pin number to the microcontroller
	  int ret=write(fd, &pin_number, sizeof(uint8_t));
	  if (ret<0) {
		fprintf(stderr, "Error writing on descriptor: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	  }
	  }
      break;
    }
  }
  */
  //
  char command;
  char seconds[128];
  while(1) {
    printf("Choose one of the following operations, by entering the corrispondent character:\n \
    -[a] --> to Request the logs registrated by the Sensor Logger till now.\n \
    -[b] --> to Set the period after which the Sensor Logger shall register a log.\n \
    -[q] --> to Quit.\n");
    //Read a line from stdin
    //memset(&command, 0, sizeof(char));  //clear buffer each time
    scanf("%s", &command);
    /*
    if (fgets(command, sizeof(command), stdin) != (char*)command) {
      fprintf(stderr, "Error reading from stdin, exiting..\n");
      exit(EXIT_FAILURE);
    }
    */
    if (command=='a') {
		    Request* req=(Request*)malloc(R_DIM);
        req->type=LogRequest;
        req->duration_s=0;
        //
        while(LoadPacket(h, req));
        FlushBuffer(fd, h);
        //
        printf("Log Request sent!\n\n");
        free(req);
    }
	  else if (command=='b') {
	    while(1) {
        memset(seconds, 0, sizeof(seconds));  //clear buffer each time
        printf("Insert the new period (seconds): [at least 10]");
        scanf("%s", seconds);
        /*
        if (fgets(seconds, sizeof(seconds), stdin) != (char*)seconds) {
				fprintf(stderr, "Error reading from stdin, exiting..\n");
				exit(EXIT_FAILURE);
			  */
        int x=atoi(seconds);
        if (x==0 || x<10) {
          printf("Inserted value is not a number! (or is 0.. or less then 10..)\n  \
					Please retry..\n");
          continue;
        }
        break;
      }
      Request* req=(Request*)malloc(R_DIM);
      req->type=SetTimer;
      req->duration_s=(uint16_t)atoi(seconds);
      //
      while(LoadPacket(h, req));
      FlushBuffer(fd, h);
      //
      printf("Timer Reset sent!\n\n");
      free(req);
    }
    else if (command=='q') {
      printf("Exiting..\n");
      arg->running=0;
      break;
    }
    else printf("Command not recognised! Please try again..\n");
    sleep(5);  //wait 5 seconds before asking again for command
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
  free(arg);
  printf("Bye Bye!\n");
  return 0;
}
