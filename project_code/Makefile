CC = gcc
CC_OPTS = -Wall -g --std=gnu99
LIBS = -lpthread

INCLUDES = serial_linux.h  client_protocol.h

OBJS = client_protocol.o  serial_linux.o

BINS = client.bin

.phony:	clean all

all: $(BINS)

%.o:	%.c $(INCLUDES)
	$(CC) $(CC_OPTS) -c  $<

%.bin:  %.o $(OBJS) $(INCLUDES)
	$(CC) $(CC_OPTS) -o $@ $< $(OBJS) $(LIBS)

clean:
	rm -rf $(OBJS) $(BINS) *~
