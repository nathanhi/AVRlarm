all: compile

src/main.o: src/main.c
	avr-gcc -O2 -mmcu=atmega328p -c src/main.c -o src/main.o -Wall -Werror

src/uart.o: src/uart.c
	avr-gcc -O2 -mmcu=atmega328p -c src/uart.c -o src/uart.o -Wall -Werror

compile: src/uart.o src/main.o
	avr-gcc -mmcu=atmega328p -o src/steep_beta.a src/uart.o src/main.o
	avr-objcopy -O ihex -R src/.eeprom src/steep_beta.a src/steep_beta.hex

flash: compile
ifndef DEVNAME
	$(warning DEVNAME is not set. Assuming target on /dev/ttyACM0.)
	$(eval DEVNAME=/dev/ttyACM0)
endif
	avrdude -q -q -F -V -c arduino -p ATMEGA328P -P $(DEVNAME) -b 115200 -U flash:w:src/steep_beta.hex

clean:
	rm -rf src/*.a src/.eeprom src/*.hex src/*.o
