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

#include <avr/io.h>

#include "timers.h"

#if   TIMER0_PRESCALER == 1
#define TIMER0_CS 1
#elif TIMER0_PRESCALER == 8
#define TIMER0_CS 2
#elif TIMER0_PRESCALER == 64
#define TIMER0_CS 3
#elif TIMER0_PRESCALER == 256
#define TIMER0_CS 4
#elif TIMER0_PRESCALER == 1024
#define TIMER0_CS 5
#else
#error "Invalid timer 0 prescaler."
#endif

#if   TIMER1_PRESCALER == 1
#define TIMER1_CS 1
#elif TIMER1_PRESCALER == 8
#define TIMER1_CS 2
#elif TIMER1_PRESCALER == 64
#define TIMER1_CS 3
#elif TIMER1_PRESCALER == 256
#define TIMER1_CS 4
#elif TIMER1_PRESCALER == 1024
#define TIMER1_CS 5
#else
#error "Invalid timer 1 prescaler."
#endif

#if   TIMER2_PRESCALER == 1
#define TIMER2_CS 1
#elif TIMER2_PRESCALER == 8
#define TIMER2_CS 2
#elif TIMER2_PRESCALER == 32
#define TIMER2_CS 3
#elif TIMER2_PRESCALER == 64
#define TIMER2_CS 4
#elif TIMER2_PRESCALER == 128
#define TIMER2_CS 5
#elif TIMER2_PRESCALER == 256
#define TIMER2_CS 6
#elif TIMER2_PRESCALER == 1024
#define TIMER2_CS 7
#else
#error "Invalid timer 2 prescaler."
#endif

/*
 * run all the timers synchronized and at the same frequency
 *
 *  the counters operates continuously at clkIO/prescaler counting from 0 to 255
 *  for 8-bit and 0 to 65535 for 16-bit counters, each timer has two output
 *  compare units, timer 1 has one input capture unit
 *
 *    F_CPU / TIMERx_PRESCALER / 256
 *    16 MHz / 64 / 256 = 977. Hz
 *
 *    F_CPU / TIMERx_PRESCALER / 65536
 *    16 MHz / 64 / 65536 = 3.81 Hz
 *
 *  WGM0[2:0]=000b, mode 0, normal, TOP = 0xff
 *  WGM1[3:0]=0000b, mode 0, normal, TOP = 0xffff
 *  WGM2[2:0]=000b, mode 0, normal, TOP = 0xff
 *
 *  TIMERx_CS[2:0]=xxxb, value determined by TIMERx_PRESCALER
 *
 *  COMxA[1:0]=00b, normal port operation
 *  COMxB[1:0]=00b, normal port operation
 *
 *  TIMSKx=0x00, all interrupts disabled
 *
 *  TCNTx=0, start counters at 0
 */
void timers_init(void)
{
    /* disable and reset the prescalers */
    GTCCR = _BV(TSM) | _BV(PSRASY) | _BV(PSRSYNC);

    /* disable asynchronous operation */
    ASSR = 0x00;

    /* interrupts disabled */
    TIMSK0 = 0x00;
    TIMSK1 = 0x00;
    TIMSK2 = 0x00;

    /* configure all of the timers the same way */
    TCCR0A = 0x00;
    TCCR0B = TIMER0_CS;
    TCCR1A = 0x00;
    TCCR1B = TIMER1_CS;
    TCCR2A = 0x00;
    TCCR2B = TIMER2_CS;

    /* zero counter */
    TCNT0 = 0x00;
    TCNT1 = 0x0000;
    TCNT2 = 0x00;

    /* enable the prescalers */
    GTCCR = 0;
}
