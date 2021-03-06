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
#ifndef _PROJECT_H_
#define _PROJECT_H_

#include "pinmap.h"

#define UDIV_FLOOR(a,b) ((a) / (b))
#define UDIV_ROUND(a,b) (((a) + ((b) / 2)) / (b))
#define UDIV_CEILING(a,b) (((a) + (b) - 1) / (b))

#define min(a,b) ({              \
        __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
       _a < _b ? _a : _b;        \
})

#define max(a,b) ({              \
        __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
       _a > _b ? _a : _b;        \
})

/* lo <= n <= hi */
#define limit_range(lo,n,hi) min(max(lo,(n)),hi)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))


/*
 * console interface
 */
#ifndef BAUD
#define BAUD (9600L)
#endif

#define TX_BUF_SIZE (128)


/*
 * Project pin assignments.
 */

/* Tick pin */
#define SPEAKER_OUT  PINMAP_D4

/* ILITEK LCD controller */

// MISO MOSI SCK SS + A0 RESET

#define ILITEK_SCK   PINMAP_SCK
#define ILITEK_SI    PINMAP_MOSI
#define ILITEK_CS    PINMAP_SS
#define ILITEK_RES   PINMAP_D6
#define ILITEK_A0    PINMAP_D7

#define X_ORIGIN 0
#define X_SIZE 240
#define Y_ORIGIN 32
#define Y_SIZE 320

#define ADC_RANGE 1024
#define ADCREF_MV 5000

/*
 * timebase timer prescaler
 */
#define TBTIMER_PRESCALER (64UL)

/*
 * timebase timer, 0, 1 or 2 (only 1 is extensively tested)
 */
#define TBTIMER 1

/*
 * use the A compare so B can be used to trigger the ADC
 */
#define TBTIMER_COMP A

/*
 * timebase counter size in bits, either 16 or 32
 */
#define TBSIZE 32

/*
 * initialize console before main
 */
extern void console_init(void) __attribute__((__constructor__));

/*
 * initialize timers before main
 */
extern void timers_init(void) __attribute__((__constructor__));

/*
 * these functions never return
 */
extern void main(void) __attribute__((__noreturn__));
extern void cmdline(void) __attribute__((__noreturn__));

#endif /* _PROJECT_H_ */
