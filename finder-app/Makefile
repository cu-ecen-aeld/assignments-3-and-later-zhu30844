# Reffence: https://makefiletutorial.com/#getting-started
TOOLCHAIN_PATH_PREFIX := /opt/toolchains/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/bin/
NATIVE_CC := gcc
FINAL_CC := $(if $(CROSS_COMPILE), $(TOOLCHAIN_PATH_PREFIX)$(CROSS_COMPILE)$(NATIVE_CC), $(NATIVE_CC))
writer:
	$(FINAL_CC) -Wall -Werror -o writer writer.c
clean:
	rm -f writer *.o

