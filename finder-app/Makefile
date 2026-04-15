# Reffence: https://makefiletutorial.com/#getting-started
CC ?= gcc
CFLAGS ?= -Wall -Werror
all: writer

writer: writer.c
	$(CC) $(CFLAGS) -o $@ $<
clean:
	rm -f writer *.o

