// 16MHz + 9600 Baud
// Possible bit rates:
// http://bit.ly/1ND0wxf
#define F_CPU 16000000
#define BAUD 9600
#define BAUDRATE (((F_CPU)/(BAUD*16UL))-1)

// Initial wait until alarm will be armed: 10 Minutes
#define INITIAL_WAIT 600000

#include <avr/io.h>
