#ifndef CONFIG_H
#define CONFIG_H

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
// Initial wait time for modem firmware to boot: 10 seconds
#define INITIAL_MODEM_WAIT 10000
// Time to wait until notification is sent: Immediate
#define ALARM_WAIT 0
// Time to wait until resuming operation after alarm: 1 Minute
// Won't resume operation if unset
#define ALARM_RESUME 60000

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
// Multiple numbers can be specified comma separated
#define TGT_NUMS "+491,+4912,+49123,+491234,+4912345,+49123456,+491234567,+4912345678"

// Target number for informal notifications
// Multiple numbers can be specified comma separated
#define INFO_NUMS "+491,+4912,+49123"

// IO specific settings
#define PORT_LED PORTB7             // Pin 13
#define PORT_GSM_IGN PORTB6         // Pin 12
#define PORT_ALARM_INDICATOR PORTB5 // Pin 11

// Setting for morse code
// Dit is 120ms long (10 WpM)
// Dah is 3x Dit.
#define morse_dit_length 120

#include <avr/io.h>

#endif /* CONFIG_H */
