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

/*
 * include for the explicitly sized types
 */
#include <stdint.h>

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

#ifndef BAUD
#define BAUD (250000UL)
#endif

/*
 * timebase configuration
 */

/* timebase timer 0, 1, or 2 (only 1 is extensively tested) */
#define TBTIMER 1

/* timebase prescaler is 1 so ADC trigger can hit 6400 Hz */
#define TBTIMER_PRESCALER (1UL)

/* use the A compare so B is available for the ADC trigger */
#define TBTIMER_COMP A

/* timebase counter size in bits, either 16 or 32 */
#define TBSIZE 32

/*
 * timer 0 is run in phase correct PWM mode for the LED brightness
 *
 *  16 MHz / 256 / 510 = 122.5 Hz
 */
#define TIMER0_PRESCALER (256UL)

/* LED interface pins */
#define LED_SS    PINMAP_SS
#define LED_DATA  PINMAP_MOSI
#define LED_CLOCK PINMAP_SCK
#define LED_ENA0  PINMAP_D3
#define LED_ENA1  PINMAP_D2
#define LED_ENA2  PINMAP_D4
#define LED_ENA3  PINMAP_D5
#define LED_BRT   PINMAP_D6

/*
 * autostart "Constructors"
 */
extern void timers_init(void) __attribute__((__constructor__));
extern void console_init(void) __attribute__((__constructor__));

/*
 * these functions never return
 */
extern void main(void) __attribute__((__noreturn__));

/* entry point for command line interpreter */
extern void cmdline(void) __attribute__((__noreturn__));

#endif /* _PROJECT_H_ */
