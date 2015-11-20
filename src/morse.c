#include "morse.h"
#include "io.h"

#include <util/delay.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

void morse_short(int port) {
    /* Morses just a short dit
     * and pauses for a dit.
     */
    io_set_port_state(port, IO_PORT_HIGH);
    _delay_ms(morse_dit_length);
    io_set_port_state(port, IO_PORT_LOW);
    _delay_ms(morse_dit_length);
}

void morse_long(int port) {
    /* Morses a long dah for 3*dit ms
     * and pauses for a dit
     */
    io_set_port_state(port, IO_PORT_HIGH);
    _delay_ms(3*morse_dit_length);
    io_set_port_state(port, IO_PORT_LOW);
    _delay_ms(3*morse_dit_length);
}

void morse_letter(int port, char ltr) {
    /* Morses exactly one letter */
    
    // Pull down given port first
    io_set_port_state(port, IO_PORT_LOW);
    
    switch(toupper(ltr)) {
        case 'A':
            // A: ._
            morse_short(port);
            morse_long(port);
            break;
        case 'B':
            // B: _...
            morse_long(port);
            morse_short(port);
            morse_short(port);
            morse_short(port);
            break;
        case 'C':
            // C: _._.
            morse_long(port);
            morse_short(port);
            morse_long(port);
            morse_short(port);
            break;
        case 'D':
            // D: _..
            morse_long(port);
            morse_short(port);
            morse_short(port);
            break;
        case 'E':
            // E: .
            morse_short(port);
            break;
        case 'F':
            // F: .._.
            morse_short(port);
            morse_short(port);
            morse_long(port);
            morse_short(port);
            break;
        case 'G':
            // G: ___.
            morse_long(port);
            morse_long(port);
            morse_long(port);
            morse_short(port);
            break;
        case 'H':
            // H: ....
            morse_short(port);
            morse_short(port);
            morse_short(port);
            morse_short(port);
            break;
        case 'I':
            // I: ..
            morse_short(port);
            morse_short(port);
            break;
        case 'J':
            // J: .___
            morse_short(port);
            morse_long(port);
            morse_long(port);
            morse_long(port);
            break;
        case 'K':
            // K: _._
            morse_long(port);
            morse_short(port);
            morse_long(port);
            break;
        case 'L':
            // L: ._..
            morse_short(port);
            morse_long(port);
            morse_short(port);
            morse_short(port);
            break;
        case 'M':
            // M: __
            morse_long(port);
            morse_long(port);
            break;
        case 'N':
            // N: _.
            morse_long(port);
            morse_short(port);
            break;
        case 'O':
            // O: ___
            morse_long(port);
            morse_long(port);
            morse_long(port);
            break;
        case 'P':
            // P: .__.
            morse_short(port);
            morse_long(port);
            morse_long(port);
            morse_short(port);
            break;
        case 'Q':
            // Q: __._
            morse_long(port);
            morse_long(port);
            morse_short(port);
            morse_long(port);
            break;
        case 'R':
            // R: ._.
            morse_short(port);
            morse_long(port);
            morse_short(port);
            break;
        case 'S':
            // S: ...
            morse_short(port);
            morse_short(port);
            morse_short(port);
            break;
        case 'T':
            // T: _
            morse_long(port);
            break;
        case 'U':
            // U: .._
            morse_short(port);
            morse_short(port);
            morse_long(port);
            break;
        case 'V':
            // V: ..._
            morse_short(port);
            morse_short(port);
            morse_short(port);
            morse_long(port);
            break;
        case 'W':
            // W: .__
            morse_short(port);
            morse_long(port);
            morse_long(port);
            break;
        case 'X':
            // X: _.._
            morse_long(port);
            morse_short(port);
            morse_short(port);
            morse_long(port);
            break;
        case 'Y':
            // Y: _.__
            morse_long(port);
            morse_short(port);
            morse_long(port);
            morse_long(port);
            break;
        case 'Z':
            // Z: __..
            morse_long(port);
            morse_long(port);
            morse_short(port);
            morse_short(port);
            break;
        case '0':
            // 0: _____
            morse_long(port);
            morse_long(port);
            morse_long(port);
            morse_long(port);
            morse_long(port);
            break;
        case '1':
            // 1: .____
            morse_short(port);
            morse_long(port);
            morse_long(port);
            morse_long(port);
            morse_long(port);
            break;
        case '2':
            // 2: ..___
            morse_short(port);
            morse_short(port);
            morse_long(port);
            morse_long(port);
            morse_long(port);
            break;
        case '3':
            // 3: ...__
            morse_short(port);
            morse_short(port);
            morse_short(port);
            morse_long(port);
            morse_long(port);
            break;
        case '4':
            // 4: ...._
            morse_short(port);
            morse_short(port);
            morse_short(port);
            morse_short(port);
            morse_long(port);
            break;
        case '5':
            // 5: .....
            morse_short(port);
            morse_short(port);
            morse_short(port);
            morse_short(port);
            morse_short(port);
            break;
        case '6':
            // 6: _....
            morse_long(port);
            morse_short(port);
            morse_short(port);
            morse_short(port);
            morse_short(port);
            break;
        case '7':
            // 7: __...
            morse_long(port);
            morse_long(port);
            morse_short(port);
            morse_short(port);
            morse_short(port);
            break;
        case '8':
            // 8: ___..
            morse_long(port);
            morse_long(port);
            morse_long(port);
            morse_short(port);
            morse_short(port);
            break;
        case '9':
            // 9: ____.
            morse_long(port);
            morse_long(port);
            morse_long(port);
            morse_long(port);
            morse_short(port);
            break;
        case '.':
            // .: ._._._
            morse_short(port);
            morse_long(port);
            morse_short(port);
            morse_long(port);
            morse_short(port);
            morse_long(port);
            break;
        case ',':
            // ,: __..__
            morse_long(port);
            morse_long(port);
            morse_short(port);
            morse_long(port);
            morse_long(port);
            break;
        case ':':
            // :: ___...
            morse_long(port);
            morse_long(port);
            morse_long(port);
            morse_short(port);
            morse_short(port);
            morse_short(port);
            break;
        case ';':
            // ;: _._._.
            morse_long(port);
            morse_short(port);
            morse_long(port);
            morse_short(port);
            morse_long(port);
            morse_short(port);
            break;
        case '?':
            // ?: ..__..
            morse_short(port);
            morse_short(port);
            morse_long(port);
            morse_long(port);
            morse_short(port);
            morse_short(port);
            break;
        case '-':
            // -: _...._
            morse_long(port);
            morse_short(port);
            morse_short(port);
            morse_short(port);
            morse_short(port);
            morse_long(port);
            break;
        case '_':
            // _: ..__._
            morse_short(port);
            morse_short(port);
            morse_long(port);
            morse_long(port);
            morse_short(port);
            morse_long(port);
            break;
        case '(':
            // (: _.__.
            morse_long(port);
            morse_short(port);
            morse_long(port);
            morse_long(port);
            morse_short(port);
            break;
        case ')':
            // ): _.__._
            morse_long(port);
            morse_short(port);
            morse_long(port);
            morse_long(port);
            morse_short(port);
            morse_long(port);
            break;
        case '\'':
            // ': .____.
            morse_short(port);
            morse_long(port);
            morse_long(port);
            morse_long(port);
            morse_long(port);
            morse_short(port);
            break;
        case '=':
            // =: _..._
            morse_long(port);
            morse_short(port);
            morse_short(port);
            morse_short(port);
            morse_long(port);
            break;
        case '+':
            // +: ._._.
            morse_short(port);
            morse_long(port);
            morse_short(port);
            morse_long(port);
            morse_short(port);
            break;
        case '/':
            // /: _.._.
            morse_long(port);
            morse_short(port);
            morse_short(port);
            morse_long(port);
            morse_short(port);
            break;
        case '@':
            // @: .__._.
            morse_short(port);
            morse_long(port);
            morse_long(port);
            morse_short(port);
            morse_long(port);
            morse_short(port);
            break;
        }
    _delay_ms(morse_dit_length);
}

void morse(int port, char *msg) {
    /* Morses a series of characters */
    for (int i = 0; i < strlen(msg); i++) {
        printf("%c", msg[i]);
        morse_letter(port, msg[i]);
    }
    _delay_ms(3*morse_dit_length);
}
