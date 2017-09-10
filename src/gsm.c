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
        snprintf(buf, 255, "%s\r\n", c);
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

        if (retval[0] == '\0')
            // Ignore empty strings
            continue;

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
        // AT+CPIN?
        else if (strcmp(retval, "+CPIN: READY") == 0)
            // Match for "READY" on CPIN requests
            return CODE_OK;
        else if (strcmp(retval, "+CPIN: SIM PIN") == 0 || strcmp(retval, "+CPIN: SIM PIN2") == 0||
                 strcmp(retval, "+CPIN: SIM PUK") == 0 || strcmp(retval, "+CPIN: SIM PUK2") == 0)
             return CODE_CPIN_REQUIRED;
    }
}

int gsm_exec(char *c, bool abortonerror, bool autoeol) {
    /* Executes given GSM commands and
     * aborts on error if specified
     */
    char *retmsg = NULL;
#ifdef DEBUG
    char buf[255] = { '\0' };
    snprintf(buf, 255, "[GSM] Executing command '%s'...\t", c);
    uart_sendmsg(DBG_UART, buf, 255);
#endif
    int retval = _gsm_exec(c, &retmsg, autoeol);
    uart_clearbuf(GSM_UART);
#ifdef DEBUG
    memset(&buf[0], '\0', 255);
    snprintf(buf, 255, "[%s]\r\n", retmsg);
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

void gsm_init() {
    /* Initialize Siemens TC35 GSM Unit */
    // Wait until modem firmware has finished booting
    _delay_ms(INITIAL_MODEM_WAIT);

    // Initialize UART
    uart_init(GSM_UART);

    uart_sendmsg(DBG_UART, "[GSM] Initializing GSM modem..\r\n", -1);
    uart_sendmsg(DBG_UART, "[GSM] Activating hardware..\r\n", -1);

    // Send escape to abort running tasks
    uart_putchar(GSM_UART, '\3');  // Ctrl-C
    uart_putchar(GSM_UART, '\24'); // Ctrl-X
    uart_putchar(GSM_UART, '\27'); // Escape

    // Disable echo
    uart_sendmsg(GSM_UART, "ATE1\r\n", -1);
    uart_clearbuf(GSM_UART);

    // Reset all modem settings
    uart_sendmsg(DBG_UART, "[GSM] Reset modem to factory defaults..\r\n", -1);
    gsm_exec("AT&F", true, true);

    // Enhanced return message format (<CR><LF>message<CR><LF>)
    gsm_exec("ATV1", true, true);

#ifdef DEBUG
    // Enable extended error reporting
    uart_sendmsg(DBG_UART, "[GSM] Enable extended error codes...\r\n", -1);
    gsm_exec("AT+CMEE=2", true, true);
#endif

    // Enter PIN-Code if defined and required
    if (gsm_exec("AT+CPIN?", false, true) != 0) {
#ifdef PINCODE
        uart_sendmsg(DBG_UART, "[GSM] Unlocking SIM card..\r\n", -1);
        gsm_exec(strcat("AT+CPIN=", PINCODE), true, true);
#else
        uart_sendmsg(DBG_UART, "[GSM] PIN or PUK required, but not configured!\r\n", -1);
        while (true) {
            // Loop endlessly and morse error message
            // First part: Source module; GSM
            morse(PORT_LED, "GSM");
            _delay_ms(5*morse_dit_length);
            morse(PORT_LED, "PIN");
            _delay_ms(10*morse_dit_length);
        }
#endif
    }

    uart_sendmsg(DBG_UART, "[GSM] Setting default character " \
                           "set to GSM..\r\n", -1);
    gsm_exec("AT+CSCS=\"GSM\"", true, true);
}

bool _gsm_send_sms(char *msg, char *number) {
    /* Sends an SMS to the given number */
    // Set modem to text mode
    char buf[1024] = { '\0' };
    gsm_exec("AT+CMGF=1", true, true);

    snprintf(buf, 1024, "[GSM] Sending SMS with %i characters to '%s'.\r\n", strlen(msg), number);
    uart_sendmsg(DBG_UART, buf, 1024);

    // Switch to text mode for given number
    memset(&buf, '\0', 1024);
    snprintf(buf, 1024, "AT+CMGS=\"%s\"\r\n", number);
    uart_sendmsg(GSM_UART, buf, 1024);
    _delay_ms(500);
    uart_clearbuf(GSM_UART);

    // Write message to buffer
    memset(&buf, '\0', 1024);
    snprintf(buf, 1024, "%s", msg);
    _delay_ms(500);
    uart_sendmsg(GSM_UART, buf, 1024);

    // Send message
    return (gsm_exec("\032", false, true) == 0);
}

void gsm_send_sms(char *msg, char *numbers) {
    char *pt;
    pt = strtok(numbers, ",");
    while (pt != NULL) {
        // Try to send SMS to every number specified
        if (!_gsm_send_sms(msg, pt)) {
            uart_sendmsg(DBG_UART, "[GSM] Failed to send SMS! Trying again.\r\n", -1);
            // Retry for configured amount on failure
            for (int i=1; i<=SMS_RETRY_COUNT; i++) {
                // Reinitialise modem
                _delay_ms(1000);
                gsm_init();
                gsm_exec("AT+CHUP", false, true);  // Hang up existing calls

                if (_gsm_send_sms(msg, pt))
                    break;
            }
        }
        pt = strtok (NULL, ",");
    }
}
