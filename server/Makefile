# Reference: https://makefiletutorial.com/#getting-started
CC ?= gcc
CFLAGS ?= -Wall -Werror

all: aesdsocket

aesdsocket: aesdsocket.c
	$(CC) $(CFLAGS) -o $@ $<
clean:
	rm -f aesdsocket *.o

