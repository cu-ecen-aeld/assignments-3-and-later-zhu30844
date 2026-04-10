# Reffence: https://makefiletutorial.com/#getting-started
NATIVE_CC := gcc
FINAL_CC := $(CROSS_COMPILE)$(NATIVE_CC)
writer:
	$(FINAL_CC) -Wall -Werror -o writer writer.c
clean:
	rm -f writer *.o

