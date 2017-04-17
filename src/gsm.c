#include "gsm.h"
#include "uart.h"
#include "io.h"
#include "morse.h"

#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

int _gsm_exec(char *c, char **retmsg, bool autoeol) {
    /* Executes given command and
     * returns return code of modem */

     // Clean UART receive buffer first
     uart_clearbuf(GSM_UART);

    // Send message via UART
    char buf[255] = { '\0' };
    if (autoeol)
        snprintf(buf, 255, "%s\n", c);
    else
        snprintf(buf, 255, "%s", c);

    // Send command
    uart_sendmsg(GSM_UART, buf, 255);

    // Receive until correct data is retrieved
    while (true) {
        // This will, of course, deadlock if we don't receive the answer
        // we're waiting for.
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
    }
}

int gsm_exec(char *c, bool abortonerror, bool autoeol) {
    /* Executes given GSM commands and
     * aborts on error if specified
     */
    char *retmsg = NULL;
#ifdef DEBUG
    char buf[255] = { '\0' };
    snprintf(buf, 255, "[GSM]: Executing command '%s'...\t", c);
    uart_sendmsg(DBG_UART, buf, 255);
#endif
    int retval = _gsm_exec(c, &retmsg, autoeol);
#ifdef DEBUG
    memset(&buf[0], '\0', 255);
    snprintf(buf, 255, "[%s]\n", retmsg);
    uart_sendmsg(DBG_UART, buf, 255);
#endif
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
    free(retmsg);
    return retval;
}

void gsm_toggle_igt() {
    /* Set ignition to LOW for 100ms
     * and HIGH for 1000ms to activate the modem
     * as described in
     * "3.3.1.1 Turn on GSM engine using the ignition line IGT (Power on)"
     */
    io_set_port_state(PORT_GSM_IGN, IO_PORT_LOW);
    _delay_ms(100);  // From the manual, pull down for 100ms
    io_set_port_state(PORT_GSM_IGN, IO_PORT_HIGH);
    _delay_ms(100);  // From the manual, pull and pull up for 1000ms
    io_set_port_state(PORT_GSM_IGN, IO_PORT_LOW);
}

void gsm_init() {
    /* Initialize Siemens TC35 GSM Unit */
    // Wait until modem firmware has finished booting
    _delay_ms(INITIAL_MODEM_WAIT);

    // Initialize UART
    uart_init(GSM_UART);

    uart_sendmsg(DBG_UART, "[GSM]: Initializing GSM modem..\n", -1);
    uart_sendmsg(DBG_UART, "[GSM]: Activating hardware..\n", -1);

    // Send escape to abort running tasks
    uart_putchar(GSM_UART, '\3');  // Ctrl-C
    uart_putchar(GSM_UART, '\24'); // Ctrl-X
    uart_putchar(GSM_UART, '\27'); // Escape

    // Send short pulse to GSM modem
    gsm_toggle_igt();

    // Enable echo
    gsm_exec("ATE1", true, true);

    // Reset all modem settings
    uart_sendmsg(DBG_UART, "[GSM]: Reset modem to factory defaults..\n", -1);
    gsm_exec("AT&F", true, true);

    // Enhanced return message format (<CR><LF>message<CR><LF>)
    gsm_exec("ATV1", true, true);

#ifdef DEBUG
    // Enable extended error reporting
    uart_sendmsg(DBG_UART, "[GSM]: Enable extended error codes...\n", -1);
    gsm_exec("AT+CMEE=2", true, true);
#endif

    // Enter PIN-Code if defined
#ifdef PINCODE
    uart_sendmsg(DBG_UART, "[GSM]: Unlocking SIM card..\n", -1);
    gsm_exec(strcat("AT+CPIN=", PINCODE), false, true);
#endif

    uart_sendmsg(DBG_UART, "[GSM]: Setting default character " \
                           "set to GSM..\n", -1);
    gsm_exec("AT+CSCS=\"GSM\"", true, true);
}

void _gsm_send_sms(char *msg, char *number) {
    /* Sends an SMS to the given number */
    // Set modem to text mode
    char buf[1024] = { '\0' };
    gsm_exec("AT+CMGF=1", true, true);

    snprintf(buf, 1024, "[GSM]: Sending SMS with %i characters to '%s'.\n", strlen(msg), number);
    uart_sendmsg(DBG_UART, buf, 1024);

    // Switch to text mode for given number
    memset(&buf, '\0', 1024);
    snprintf(buf, 1024, "AT+CMGS=\"%s\"\n", number);
    uart_sendmsg(GSM_UART, buf, 1024);
    uart_clearbuf(GSM_UART);

    // Write message to buffer
    memset(&buf, '\0', 1024);
    snprintf(buf, 1024, "%s", msg);
    uart_sendmsg(GSM_UART, buf, 1024);
    uart_clearbuf(GSM_UART);

    // Send SMS
    gsm_exec("\26", true, true);  // Ctrl-Z

    // Clear buffer, just to make sure
    uart_clearbuf(GSM_UART);
}

void gsm_send_sms(char *msg, char *numbers) {
    char *pt;
    pt = strtok(numbers, ",");
    while (pt != NULL) {
        // Send SMS to every number specified
        _gsm_send_sms(msg, pt);
        pt = strtok (NULL, ",");
    }
}
