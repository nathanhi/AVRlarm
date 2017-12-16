AVRlarm
=======

[![Build Status](https://img.shields.io/travis/nathanhi/AVRlarm.svg?maxAge=2592000&style=flat-square&branch=master)](https://travis-ci.org/nathanhi/AVRlarm)
[![Coverity Scan Build Status](https://img.shields.io/coverity/scan/9073.svg?maxAge=2592000&style=flat-square)](https://scan.coverity.com/projects/nathanhi-avrlarm)
[![MIT License](https://img.shields.io/github/license/nathanhi/AVRlarm.svg?maxAge=2592000&style=flat-square)](https://github.com/nathanhi/AVRlarm/blob/HEAD/LICENSE)

AVRlarm is an AVR alarm system designed
for 8-Bit Atmel CPUs (ATmega2560) that
monitors specified I/O ports and offers
GSM (SMS) notification.

Hardware dependencies
---------------------

AVRlarm has been proven to work with
various GSM modems like for example
the Siemens TC35, SIM800 and SIM900.

Building
--------

AVRlarm requires an installed avr-gcc compiler to be built.
To build and link the sources, you simply have to execute

    $ make

This will generate a .hex file, ready to be uploaded to
your AVRs EEPROM.

Installing
----------

AVRlarm can be directly installed to the target via Make
if avrdude is in PATH:

    $ make flash

This assumes per default that your device is available on
/dev/ttyUSB0. To override the flash target, simply override
the DEVNAME variable, like so:

    $ DEVNAME=/dev/ttyUSB1 make flash
