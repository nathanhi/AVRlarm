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

all: src/steep_beta$(DEBUGEXT).hex
debug: all

.INTERMEDIATE: src/version.h
src/version.h:
	echo "#define STEEP_BETA_COMMIT \"$(shell git rev-parse --short HEAD || echo unknown commit)\"" > $@
	echo "#define STEEP_BETA_VERSION \"0.1\"" >> $@
	echo "#define STEEP_BETA_BUILDDATE \"$(shell date -I || echo unknown build date)\"" >> $@

src/%$(DEBUGEXT).o: src/%.c src/version.h
	$(CC) -mmcu=atmega2560 $(CFLAGS) -c $< -o $@

src/steep_beta$(DEBUGEXT).a: $(BUILD_DEPS)
	$(CC) -Wall -Werror -Wl,-u,vfprintf -lprintf_flt -lm -mmcu=atmega2560 -o $@ $^

src/steep_beta$(DEBUGEXT).hex: src/steep_beta$(DEBUGEXT).a
	$(OBJCOPY) -O ihex -R .eeprom $< $@

.PHONY: flash
flash: src/steep_beta$(DEBUGEXT).hex
ifndef DEVNAME
	$(warning DEVNAME is not set. Assuming target on /dev/ttyUSB0.)
	$(eval DEVNAME=/dev/ttyUSB0)
endif
	avrdude -q -q -F -V -patmega2560 -cwiring -P$(DEVNAME) -b115200 -D -Uflash:w:$<

.PHONY: clean
clean:
	rm -rf src/*.a src/*.hex src/*.o src/version.h
