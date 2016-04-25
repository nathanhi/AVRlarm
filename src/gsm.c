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
    char buf[255];
    snprintf(buf, 255, "%s\n", c);
    uart_sendmsg(GSM_UART, buf);

    // Receive answer; discard first answer (just a newline)
    uart_getmsg(GSM_UART);
    *retmsg = uart_getmsg(GSM_UART);
    const char *retval = *retmsg;

    // Parse output and return corresponding return code
    // We have to use magic numbers instead of the defines (CODE_...) here
    // because casting all these ints to proper strings would 
    // create too much of an overhead
    if (strcmp(retval, TXT_OK) == 0 || strcmp(retval, "0") == 0)
        // Match for CODE/TXT OK
        return CODE_OK;
    else if (strcmp(retval, TXT_CONNECT) == 0 || strcmp(retval, "1") == 0)
        // Match for CODE/TXT CONNECT
        return CODE_CONNECT;
    else if (strcmp(retval, TXT_RING) == 0 || strcmp(retval, "2") == 0)
        // Match for CODE/TXT RING
        return CODE_RING;
    else if (strcmp(retval, TXT_NOCARRIER) == 0 || strcmp(retval, "3") == 0)
        // Match for CODE/TXT NOCARRIER
        return CODE_NOCARRIER;
    else if (strcmp(retval, TXT_ERROR) == 0 || strcmp(retval, "4") == 0)
        // Match for CODE/TXT ERROR
        return CODE_ERROR;
    else if (strcmp(retval, TXT_NODIALTONE) == 0 || strcmp(retval, "6") == 0)
        // Match for CODE/TXT NODIALTONE
        return CODE_NODIALTONE;
    else if (strcmp(retval, TXT_BUSY) == 0 || strcmp(retval, "7") == 0)
        // Match for CODE/TXT BUSY
        return CODE_BUSY;
    else if (strcmp(retval, TXT_CONNECT2400) == 0 || strcmp(retval, "10") == 0)
        // Match for CODE/TXT CONNECT2400
        return CODE_CONNECT2400;
    else if (strcmp(retval, TXT_CONNECT2400RLP) == 0 || strcmp(retval, "47") == 0)
        // Match for CODE/TXT CONNECT2400RLP
        return CODE_CONNECT2400RLP;
    else if (strcmp(retval, TXT_CONNECT4800) == 0 || strcmp(retval, "30") == 0)
        // Match for CODE/TXT CONNECT4800
        return CODE_CONNECT4800;
    else if (strcmp(retval, TXT_CONNECT4800RLP) == 0 || strcmp(retval, "48") == 0)
        // Match for CODE/TXT CONNECT4800RLP
        return CODE_CONNECT4800RLP;
    else if (strcmp(retval, TXT_CONNECT9600) == 0 || strcmp(retval, "32") == 0)
        // Match for CODE/TXT CONNECT9600
        return CODE_CONNECT9600;
    else if (strcmp(retval, TXT_CONNECT9600RLP) == 0 || strcmp(retval, "49") == 0)
        // Match for CODE/TXT CONNECT9600RLP
        return CODE_CONNECT9600RLP;
    else if (strcmp(retval, TXT_CONNECT14400) == 0 || strcmp(retval, "33") == 0)
        // Match for CODE/TXT CONNECT14400
        return CODE_CONNECT14400;
    else if (strcmp(retval, TXT_CONNECT14400RLP) == 0 || strcmp(retval, "50") == 0)
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
#ifdef DEBUG
    char buf[255];
    snprintf(buf, 255, "[GSM]: Executing command '%s'...\t", c);
    uart_sendmsg(DBG_UART, buf);
#endif
    int retval = _gsm_exec(c, &retmsg);
    if (retval == CODE_ERROR && abortonerror) {
        // If error is received
        char tmp_retval[10];
#ifdef DEBUG
        snprintf(buf, 255, "[%s]\n", retmsg);
        uart_sendmsg(DBG_UART, buf);
#endif
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
#ifdef DEBUG
    else {
        uart_sendmsg(DBG_UART, "[OK]\n");
    }
#endif
    return retval;
}

void gsm_init() {
    /* Initialize Siemens TC35 GSM Unit */
    // Initialize UART
    uart_init(GSM_UART);

    uart_sendmsg(DBG_UART, "[GSM]: Initializing GSM modem..\n");
    uart_sendmsg(DBG_UART, "[GSM]: Activating hardware..\n");

    // Send escape to abort running tasks
    uart_sendmsg(GSM_UART, "\27\n");

    /* Set ignition to HIGH for 2.5
     * seconds to activate the modem
     */
    io_set_port_state(PORT_GSM_IGN, IO_PORT_HIGH);
    _delay_ms(2500);
    io_set_port_state(PORT_GSM_IGN, IO_PORT_LOW);

    // Send escape to abort running tasks
    uart_sendmsg(GSM_UART, "\27");

    // Disable echo
    gsm_exec("ATE0", true);

    // Reset all modem settings
    uart_sendmsg(DBG_UART, "[GSM]: Reset modem to factory defaults..\n");
    gsm_exec("AT&F0", true);

#ifdef DEBUG
    // English result codes instead of numeric for better readability
    uart_sendmsg(DBG_UART, "[GSM]: English result codes instead of numeric..\n");
    gsm_exec("ATV1", true);
#else
    // Numeric result codes instead of English..
    gsm_exec("ATV0", true);
#endif

    // Enter PIN-Code if defined
#ifdef PINCODE
    uart_sendmsg(DBG_UART, "[GSM]: Unlocking SIM card..\n");
    gsm_exec(strcat("AT+CPIN=", PINCODE), true);
#endif

    uart_sendmsg(DBG_UART, "[GSM]: Disabling powersave mode..\n");
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

bool _gsm_send_sms(char *msg, char *number) {
    /* Sends an SMS to the given number */
    // Set modem to text mode
    char buf[1024] = { '\0' };
    if (gsm_exec("AT+CMGF=1", true) != CODE_OK)
        return false;

    snprintf(buf, 1024, "[GSM]: Sending SMS with %i characters to '%s'.\n", strlen(msg), number);
    uart_sendmsg(DBG_UART, buf);
    
    // Specify phone number
    snprintf(buf, 1024, "AT+CMGS=%s\n", number);
    uart_sendmsg(GSM_UART, buf);
    
    // Put message in body
    snprintf(buf, 1024, "%s", msg);
    uart_sendmsg(GSM_UART, buf);
    // Receive '>'
    //uart_sendmsg(DBG_UART, uart_getmsg(GSM_UART));
    uart_sendmsg(GSM_UART, "\x1A");
    uart_sendmsg(DBG_UART, uart_getmsg(GSM_UART));

    //if (gsm_exec("\x1A", true) != CODE_OK)
    //    return false;
    
    return true;
}

bool gsm_send_sms(char *msg, char *numbers) {
    bool status = true;
    char *pt;
    pt = strtok(numbers, ",");
    while (pt != NULL) {
        // Send SMS to every number specified
        if (!_gsm_send_sms(msg, pt)) {
            // If send failed, fail overall status
            status = false;
        }
        pt = strtok (NULL, ",");
    }
    return status;
}
