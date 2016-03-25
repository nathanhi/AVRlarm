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
#define INITIAL_WAIT 60000
// Wait for this amount of milliseconds until
// sending an alarm message
#define ALARM_WAIT 30000
// Time (ms) to wait to resume operation after alarm
// If commented, alarm system will remain in alarm state
// forever. Default is 30 minutes.
#define ALARM_RESUME 1800000

// Initialization message that will be sent as soon as
// the system is armed. Comment it if no message should
// be sent.
#define ARMMSG "Alarm system is fully functional and armed."


// Message that will be sent in the case of an alarm
#define ALARMMSG "Intrusion detected!"

// GSM specific settings
// If SIM has PIN code, simply
// uncomment the next line
//#define PINCODE "1234"

// Target number for alarm notifications
#define TGT_NUM "+491234567890"

// IO specific settings
#define PORT_LED PORTB7             // Pin 13
#define PORT_GSM_IGN PORTB6         // Pin 12
#define PORT_ALARM_INDICATOR PORTB5 // Pin 11

// Bool is nice :)
#define false 0
#define true 1
#define bool int

// Setting for morse code
// Dit is 120ms long (10 WpM)
// Dah is 3x Dit.
#define morse_dit_length 120

#include <avr/io.h>
