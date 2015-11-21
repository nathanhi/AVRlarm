#include <stdlib.h>
#include <string.h>

#include "gsm.h"
#include "uart.h"
#include "io.h"
#include "morse.h"

#include <util/delay.h>

int _gsm_exec(char *c, char **retmsg) {
    /* Executes given command and
     * returns return code of modem */

    // Send message via UART
    printf("%s\r\n", c);

    // Receive answer
    *retmsg = uart_getmsg();
    const char *retval = *retmsg;

    // Parse output and return corresponding return code
    if (strcmp(retval, TXT_OK) == 0 || strcmp(retval, (const char*)CODE_OK+'0') == 0)
        // Match for CODE/TXT OK
        return CODE_OK;
    else if (strcmp(retval, TXT_CONNECT) == 0 || strcmp(retval, (const char*)CODE_CONNECT+'0') == 0)
        // Match for CODE/TXT CONNECT
        return CODE_CONNECT;
    else if (strcmp(retval, TXT_RING) == 0 || strcmp(retval, (const char*)CODE_RING+'0') == 0)
        // Match for CODE/TXT RING
        return CODE_RING;
    else if (strcmp(retval, TXT_NOCARRIER) == 0 || strcmp(retval, (const char*)CODE_NOCARRIER+'0') == 0)
        // Match for CODE/TXT NOCARRIER
        return CODE_NOCARRIER;
    else if (strcmp(retval, TXT_ERROR) == 0 || strcmp(retval, (const char*)CODE_ERROR+'0') == 0)
        // Match for CODE/TXT ERROR
        return CODE_ERROR;
    else if (strcmp(retval, TXT_NODIALTONE) == 0 || strcmp(retval, (const char*)CODE_NODIALTONE+'0') == 0)
        // Match for CODE/TXT NODIALTONE
        return CODE_NODIALTONE;
    else if (strcmp(retval, TXT_BUSY) == 0 || strcmp(retval, (const char*)CODE_BUSY+'0') == 0)
        // Match for CODE/TXT BUSY
        return CODE_BUSY;
    else if (strcmp(retval, TXT_CONNECT2400) == 0 || strcmp(retval, (const char*)CODE_CONNECT2400+'0') == 0)
        // Match for CODE/TXT CONNECT2400
        return CODE_CONNECT2400;
    else if (strcmp(retval, TXT_CONNECT2400RLP) == 0 || strcmp(retval, (const char*)CODE_CONNECT2400RLP+'0') == 0)
        // Match for CODE/TXT CONNECT2400RLP
        return CODE_CONNECT2400RLP;
    else if (strcmp(retval, TXT_CONNECT4800) == 0 || strcmp(retval, (const char*)CODE_CONNECT4800+'0') == 0)
        // Match for CODE/TXT CONNECT4800
        return CODE_CONNECT4800;
    else if (strcmp(retval, TXT_CONNECT4800RLP) == 0 || strcmp(retval, (const char*)CODE_CONNECT4800RLP+'0') == 0)
        // Match for CODE/TXT CONNECT4800RLP
        return CODE_CONNECT4800RLP;
    else if (strcmp(retval, TXT_CONNECT9600) == 0 || strcmp(retval, (const char*)CODE_CONNECT9600+'0') == 0)
        // Match for CODE/TXT CONNECT9600
        return CODE_CONNECT9600;
    else if (strcmp(retval, TXT_CONNECT9600RLP) == 0 || strcmp(retval, (const char*)CODE_CONNECT9600RLP+'0') == 0)
        // Match for CODE/TXT CONNECT9600RLP
        return CODE_CONNECT9600RLP;
    else if (strcmp(retval, TXT_CONNECT14400) == 0 || strcmp(retval, (const char*)CODE_CONNECT14400+'0') == 0)
        // Match for CODE/TXT CONNECT14400
        return CODE_CONNECT14400;
    else if (strcmp(retval, TXT_CONNECT14400RLP) == 0 || strcmp(retval, (const char*)CODE_CONNECT14400RLP+'0') == 0)
        // Match for CODE/TXT CONNECT14400RLP
        return CODE_CONNECT14400RLP;
    else
        // Return CODE_ERROR if output cannot be parsed
        return CODE_ERROR;
}

int gsm_exec(char *c, bool abortonerror) {
    /* Executes given GSM commands and
     * aborts on error if specified
     */
    char *retmsg = NULL;
    int retval = _gsm_exec(c, &retmsg);
    if (retval == CODE_ERROR && abortonerror) {
        // If error is received
        char tmp_retval[10];
        itoa(retval, tmp_retval, 10);
        while (true) {
            // Loop endlessly and morse error message
            // First part: Source module; GSM
            morse(PORT_LED, "GSM");
            _delay_ms(5*morse_dit_length);

            // Second part: Command
            morse(PORT_LED, c);
            _delay_ms(5*morse_dit_length);

            // Third part: Error message
            morse(PORT_LED, retmsg);
            _delay_ms(5*morse_dit_length);

            // Fourth part: Return code
            morse(PORT_LED, tmp_retval);

            // Wait 10x before beginning again
            _delay_ms(10*morse_dit_length);
        }
    }
    return retval;
}

void gsm_init() {
    /* Initialize Siemens TC35 GSM Unit */
#ifdef DEBUG
    printf("[GSM]: Initializing GSM modem..\r\n");
    printf("[GSM]: Activating hardware..\r\n");
#endif

    /* Set ignition to HIGH for 2.5
     * seconds to activate the modem
     */
    io_set_port_state(PORT_GSM_IGN, IO_PORT_HIGH);
    _delay_ms(2500);
    io_set_port_state(PORT_GSM_IGN, IO_PORT_LOW);

    // Reset all modem settings
#ifdef DEBUG
    printf("[GSM]: Reset modem to factory defaults..\r\n");
#endif
    gsm_exec("AT&F0", true);
    }

#ifdef DEBUG
    // English result codes instead of numeric for better readability
    printf("[GSM]: English result codes instead of numeric..\r\n");
    gsm_exec("ATV1", true);
#else
    // Numeric result codes instead of English..
    gsm_exec("ATV0", true);
#endif

    // Enter PIN-Code if defined
#ifdef PINCODE
#ifdef DEBUG
    printf("[GSM]: Unlocking SIM card..\r\n");
#endif
    gsm_exec(strcat("AT+CPIN=", PINCODE), true);
#endif

    printf("[GSM]: Disabling powersave mode..\r\n");
    gsm_powersave(false);
}

void gsm_powersave(bool sleep) {
    /* Toggles powersave mode.
     * If sleep is true, powersave mode is set
     * If sleep is false, powersave mode is unset
     */
    char sleepstate[21];
    itoa(!sleep, sleepstate, 10);
    gsm_exec(strcat("AT+CFUN=", sleepstate), true);
}

void gsm_shutdown() {
    /* Shuts down GSM module */
    gsm_exec("AT^SMSO", true);
}

bool gsm_send_sms(char *msg, char *number) {
    /* Sends an SMS to the given number */
    // Set modem to text mode
    if (gsm_exec("AT+CMGF=1", true) != CODE_OK)
        return false;
    
    // Specify phone number
    printf("AT+CMGS=%s\r\n", number);
    
    // Put message in body
    printf("%s\r\n", msg);
    
    // End message with Ctrl+Z
    if (gsm_exec("\x1A", true) != CODE_OK)
        return false;
    
    return true;
}
