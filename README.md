Steep Beta
==========

Steep Beta is a currently in-development
DYI AVR alarm system for 8-Bit Atmel CPUs.

Hardware dependencies
---------------------

Steep Beta currently requires a Siemens TC35 GSM modem to
function correctly. Later plans are to make specific functions
optional.

Building
--------

Steep Beta requires an installed avr-gcc compiler to be built.
To build and link the sources, you simply have to execute

    $ make

This will generate a .hex file, ready to be uploaded to
your AVRs EEPROM.

Installing
----------

Steep Beta can be directly installed to the target via Make
if avrdude is in PATH:

    $ make flash

This assumes per default that your device is available on
/dev/ttyACM0. To override the flash target, simply override
the DEVNAME variable, like so:

    $ DEVNAME=/dev/ttyACM1 make flash
