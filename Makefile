all: compile

compile: src/main.c
	avr-gcc -O2 -DF_CPU=16000000UL -mmcu=atmega328p -c src/main.c -o src/main.o -Wall -Werror
	avr-gcc -mmcu=atmega328p -o src/steep_beta.a src/main.o
	avr-objcopy -O ihex -R src/.eeprom src/steep_beta.a src/steep_beta.hex

flash: src/steep_beta.a
	avrdude -q -q -F -V -c arduino -p ATMEGA328P -P $(DEVNAME) -b 115200 -U flash:w:src/steep_beta.hex

clean:
	rm -rf src/*.a src/.eeprom src/*.hex src/*.o
