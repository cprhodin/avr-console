avr-console-commandline
=======================

Description:

AVR Console with Ragel command interpreter and three commands implemented.

Target device(s):

    ATmega328P
    Arduino Nano

Toolchain:

    gcc version 4.7.2 (AVR_8_bit_GNU_Toolchain_3.4.2_939)
    avr-libc-1.8.0

Programming tool:

    AVRISP mkII

How to build (Linux 64-bit system):

    $cd librb
    $make
    $cd ..
    $make
    $make burn # Flash an Arduino Nano with avrdude.

How to run (Linux 64-bit system):

    The default baud rate is 250K.  The baud rate is set by the preprocessor variable "BAUD".

    $setserial /dev/ttyUSB0 spd_cust divisor 96 # make 38.4K into 250K
    $minicom -D /dev/ttyUSB0 -b 38400

    Type "help" for a list of commands.
