avr-console
===========

Serial I/O with stdio like canonical processing.

avr-gcc -Wall -O2 -std=c99 -mmcu=atmega328p -D__AVR_ATmega328P__ -DF_CPU=16000000 -o avr-console *.c librb.a
avr-objdump -d avr-console > avr-console.dump
avrdude -p atmega328p -U avr-console
