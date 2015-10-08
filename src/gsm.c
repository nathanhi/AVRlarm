#include <stdlib.h>
#include <string.h>

#include "gsm.h"
#include "uart.h"

int gsm_exec(char *c) {
    /* Executes given command and
     * returns return code of modem */
     // TODO: Implement output parsing..
     printf("%s\r\n", c);
     printf("[GSM]: Got: %s\r\n", uart_getmsg());
     return CODE_OK;
}

void gsm_init() {
    /* Initialize Siemens TC35 GSM Unit */
    printf("[GSM]: Initializing GSM modem..\r\n");
    printf("[GSM]: Reset modem to factory defaults..\r\n");
    if (gsm_exec("AT&F0") != CODE_OK) {
        printf("[GSM]: Failed to reset modem!\r\n");
    }

#ifdef DEBUG
    // English result codes instead of numeric for better readability
    printf("[GSM]: English result codes instead of numeric..\r\n");
    gsm_exec("ATV1");
#endif

    // Enter PIN-Code if defined
#ifdef PINCODE
    printf("[GSM]: Unlocking SIM card..\r\n");
    gsm_exec(strcat("AT+CPIN=", PINCODE));
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
    gsm_exec(strcat("AT+CFUN=", sleepstate));
}

void gsm_shutdown() {
    /* Shuts down GSM module */
    gsm_exec("AT^SMSO");
}
