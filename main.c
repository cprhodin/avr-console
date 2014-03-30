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
#include "project.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>

extern void timer1_init(void);
extern void tick_init(void);
extern void console_init(void);
extern void cmdline(void);


void main(void) __attribute__((OS_main));
void main(void)
{
    /*
     * initialize
     */
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        timer1_init();
//        timebase_init();
//        tick_init();

        console_init();
    }
    /* interrupts are enabled */

    /*
     * run command line interface
     */
    cmdline();
}
