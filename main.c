/*
 * Copyright 2013, 2014 Chris Rhodin <chris@notav8.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * avr-gcc -Wall -O2 -std=c99 -mmcu=atmega328p -D__AVR_ATmega328P__ -DF_CPU=16000000 -o avr-console *.c librb.a
 * avr-objdump -d avr-console > avr-console.dump
 */

#include "project.h"

#include <stdio.h>
#include <util/atomic.h>

extern void console_init(void);


int main(void)
{
    /*
     * initialize
     */
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        console_init();
    }
    /* interrupts are enabled */

    printf("\n\nUp, up and away!\n\n");

    for (;;) {
        printf("Command: ");

        for (;;) {
            int c;

            if ((c = getchar()) != EOF) putchar(c);

            if (c == '\n') break;
        }
    }
}

