// 16MHz + 9600 Baud
// Possible bit rates:
// http://bit.ly/1ND0wxf
#define F_CPU 16000000
#define BAUD 9600
#define BAUDRATE (((F_CPU)/(BAUD*16UL))-1)

// Specifies the scan interval to check
// for irregularities in milliseconds
#define SCAN_INTERVAL 500

// Initial wait until alarm will be armed: 1 Minute
//#define INITIAL_WAIT 60000
#define INITIAL_WAIT 1000

// GSM specific settings
// If SIM has PIN code, simply
// uncomment the next line
//#define PINCODE "1234"

// Target number for alarm notifications
#define TGT_NUM "+491234567890"

// IO specific settings
#define PORT_LED PORTB5
#define PORT_GSM_IGN PORTB4
#define PORT_ALARM_INDICATOR PORTB3

// Bool is nice :)
#define false 0
#define true 1
#define bool int

// Enable debugging output?
//#define DEBUG

// Setting for morse code
// Dit is 120ms long (10 WpM)
// Dah is 3x Dit.
#define morse_dit_length 120

#include <avr/io.h>
