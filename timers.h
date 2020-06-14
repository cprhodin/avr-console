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
#ifndef _TIMERS_H_
#define _TIMERS_H_

#ifdef TBTIMER
#include "timer.h"
#if   TBTIMER == 0
#define TIMER0_PRESCALER TBTIMER_PRESCALER
#elif TBTIMER == 1
#define TIMER1_PRESCALER TBTIMER_PRESCALER
#elif TBTIMER == 2
#define TIMER2_PRESCALER TBTIMER_PRESCALER
#else
#error "TBTIMER invalid; set to 0, 1, or 2."
#endif

#ifndef TIMER_PRESCALER
#define TIMER_PRESCALER (64UL)
#endif

#ifndef TIMER0_PRESCALER
#define TIMER0_PRESCALER TIMER_PRESCALER
#endif

#ifndef TIMER1_PRESCALER
#define TIMER1_PRESCALER TIMER_PRESCALER
#endif

#ifndef TIMER2_PRESCALER
#define TIMER2_PRESCALER TIMER_PRESCALER
#endif

#if   TIMER0_PRESCALER == 1UL
#define TIMER0_CS 1
#elif TIMER0_PRESCALER == 8UL
#define TIMER0_CS 2
#elif TIMER0_PRESCALER == 64UL
#define TIMER0_CS 3
#elif TIMER0_PRESCALER == 256UL
#define TIMER0_CS 4
#elif TIMER0_PRESCALER == 1024UL
#define TIMER0_CS 5
#else
#error "Invalid timer 0 prescaler."
#endif
#endif

#if   TIMER1_PRESCALER == 1UL
#define TIMER1_CS 1
#elif TIMER1_PRESCALER == 8UL
#define TIMER1_CS 2
#elif TIMER1_PRESCALER == 64UL
#define TIMER1_CS 3
#elif TIMER1_PRESCALER == 256UL
#define TIMER1_CS 4
#elif TIMER1_PRESCALER == 1024UL
#define TIMER1_CS 5
#else
#error "Invalid timer 1 prescaler."
#endif

#if   TIMER2_PRESCALER == 1UL
#define TIMER2_CS 1
#elif TIMER2_PRESCALER == 8UL
#define TIMER2_CS 2
#elif TIMER2_PRESCALER == 32UL
#define TIMER2_CS 3
#elif TIMER2_PRESCALER == 64UL
#define TIMER2_CS 4
#elif TIMER2_PRESCALER == 128UL
#define TIMER2_CS 5
#elif TIMER2_PRESCALER == 256UL
#define TIMER2_CS 6
#elif TIMER2_PRESCALER == 1024UL
#define TIMER2_CS 7
#else
#error "Invalid timer 2 prescaler."
#endif

#endif /* _TIMERS_H_ */
