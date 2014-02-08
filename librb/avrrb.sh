avr-gcc -Wall -O2 -std=c99 -mmcu=atmega328p -D__AVR_ATmega328P__ -DF_CPU=16000000 -c *.c
avr-ar cru librb.a *.o
cp librb.a librb.h ..
