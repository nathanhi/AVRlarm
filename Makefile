CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS += -O2 -Wall -Werror

all: compile
debug: CFLAGS += -DDEBUG -g
debug: all

src/main.o: src/main.c
	$(CC) -mmcu=atmega2560 -c src/main.c -o src/main.o $(CFLAGS)

src/uart.o: src/uart.c
	$(CC) -std=c99 -mmcu=atmega2560 -c src/uart.c -o src/uart.o $(CFLAGS)

src/gsm.o: src/gsm.c
	$(CC) -mmcu=atmega2560 -c src/gsm.c -o src/gsm.o $(CFLAGS)

src/io.o: src/io.c
	$(CC) -mmcu=atmega2560 -c src/io.c -o src/io.o $(CFLAGS)

src/morse.o: src/morse.c
	$(CC) -std=c99 -mmcu=atmega2560 -c src/morse.c -o src/morse.o $(CFLAGS)

src/timer.o: src/timer.c
	$(CC) -std=c99 -mmcu=atmega2560 -c src/timer.c -o src/timer.o $(CFLAGS)

compile: src/uart.o src/gsm.o src/io.o src/morse.o src/timer.o src/main.o
	$(CC) -Wl,-u,vfprintf -lprintf_flt -lm -mmcu=atmega2560 -o src/steep_beta.a src/uart.o src/gsm.o src/io.o src/morse.o src/timer.o src/main.o
	$(OBJCOPY) -O ihex -R src/.eeprom src/steep_beta.a src/steep_beta.hex

flash: compile
ifndef DEVNAME
	$(warning DEVNAME is not set. Assuming target on /dev/ttyUSB0.)
	$(eval DEVNAME=/dev/ttyUSB0)
endif
	avrdude -q -q -F -V -patmega2560 -cwiring -P$(DEVNAME) -b115200 -D -Uflash:w:src/steep_beta.hex

clean:
	rm -rf src/*.a src/.eeprom src/*.hex src/*.o
