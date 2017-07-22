CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS += -std=c99 -O2 -Wall -Werror

BUILD_DEPS = src/uart.o \
             src/gsm.o \
             src/io.o \
             src/morse.o \
             src/timer.o \
             src/ringbuf.o \
             src/main.o

ifeq ($(filter debug,$(MAKECMDGOALS)),debug)
CFLAGS += -DDEBUG -g

# Add 'd' to .o files in case of debug build
DEBUGEXT = d
BUILD_DEPS := $(subst .o,$(DEBUGEXT).o,$(BUILD_DEPS))
endif

all: src/steep_beta.hex
debug: all

src/%$(DEBUGEXT).o: src/%.c
	$(CC) -mmcu=atmega2560 $(CFLAGS) -c $< -o $@

src/steep_beta.a: $(BUILD_DEPS)
	$(CC) -Wall -Werror -Wl,-u,vfprintf -lprintf_flt -lm -mmcu=atmega2560 -o $@ $^

src/steep_beta.hex: src/steep_beta.a
	$(OBJCOPY) -O ihex -R .eeprom $< $@

.PHONY: flash
flash: src/steep_beta.hex
ifndef DEVNAME
	$(warning DEVNAME is not set. Assuming target on /dev/ttyUSB0.)
	$(eval DEVNAME=/dev/ttyUSB0)
endif
	avrdude -q -q -F -V -patmega2560 -cwiring -P$(DEVNAME) -b115200 -D -Uflash:w:$<

.PHONY: clean
clean:
	rm -rf src/*.a src/*.hex src/*.o
