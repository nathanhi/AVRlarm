// 16MHz + 9600 Baud
// Possible bit rates:
// http://bit.ly/1ND0wxf
#define F_CPU 16000000
#define BAUD 9600
#define BAUDRATE (((F_CPU)/(BAUD*16UL))-1)

// Initial wait until alarm will be armed: 1 Minute
//#define INITIAL_WAIT 60000
#define INITIAL_WAIT 1000

// GSM specific settings
// If SIM has no PIN code,
// simply comment the next line
#define PINCODE "1234"

// IO specific settings
#define PORT_LED PORTB5
#define PORT_GSM_IGN PORTB4
#define PORT_ALARM_INDICATOR PORTB3

// Bool is nice :)
#define false 0
#define true 1
#define bool int

#include <avr/io.h>
