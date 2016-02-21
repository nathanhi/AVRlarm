all: compile

src/main.o: src/main.c
	avr-gcc -O2 -mmcu=atmega2560 -c src/main.c -o src/main.o -Wall -Werror

src/uart.o: src/uart.c
	avr-gcc -O2 -mmcu=atmega2560 -c src/uart.c -o src/uart.o -Wall -Werror

src/gsm.o: src/gsm.c
	avr-gcc -O2 -mmcu=atmega2560 -c src/gsm.c -o src/gsm.o -Wall -Werror

src/io.o: src/io.c
	avr-gcc -O2 -mmcu=atmega2560 -c src/io.c -o src/io.o -Wall -Werror

src/morse.o: src/morse.c
	avr-gcc -std=c99 -O2 -mmcu=atmega2560 -c src/morse.c -o src/morse.o -Wall -Werror

compile: src/uart.o src/gsm.o src/io.o src/morse.o src/main.o
	avr-gcc -mmcu=atmega2560 -o src/steep_beta.a src/uart.o src/gsm.o src/io.o src/morse.o src/main.o
	avr-objcopy -O ihex -R src/.eeprom src/steep_beta.a src/steep_beta.hex

flash: compile
ifndef DEVNAME
	$(warning DEVNAME is not set. Assuming target on /dev/ttyUSB0.)
	$(eval DEVNAME=/dev/ttyUSB0)
endif
	avrdude -q -q -F -V -patmega2560 -cwiring -P$(DEVNAME) -b115200 -D -Uflash:w:src/steep_beta.hex

clean:
	rm -rf src/*.a src/.eeprom src/*.hex src/*.o
