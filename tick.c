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
#include <avr/io.h>
#include <util/atomic.h>

#include "timer.h"
#include "tick.h"

static uint32_t tick_period;


static int8_t tick_off_handler(struct timer_event * this_timer_event)
{
    /* output high, OFF */
    PORTD |= _BV(PORTD4);

    /* don't reschedule this timer */
    return 0;
}

static struct timer_event tick_off_event = {
    .next = &tick_off_event,
    .handler = tick_off_handler,
};


static int8_t tick_timer_handler(struct timer_event * this_timer_event)
{
    /* output low, ON */
    PORTD &= ~_BV(PORTD4);

    /* schedule the output off timer for 1 ms */
    tick_off_event.tbtick = TBTICKS_FROM_MS(2);
    schedule_timer_event(&tick_off_event, this_timer_event);

    /* advance this timer one second */
    this_timer_event->tbtick += tick_period;

    /* reschedule this timer */
    return 1;
}

static struct timer_event tick_timer_event =
{
    .next = &tick_timer_event,
    .handler = tick_timer_handler,
};


void tick_set_period(uint32_t period)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        tick_period = period;
    }
}


void tick_enable(uint8_t enable)
{
    if (timer_is_expired(&tick_timer_event)) {
        if (enable) {
            tick_timer_event.tbtick = tick_period;
            schedule_timer_event(&tick_timer_event, NULL);
        }
    }
    else {
        if (!enable) {
            cancel_timer_event(&tick_timer_event);
        }
    }
}


void tick_init(void)
{
    /* initialize speaker output pin */
    PORTD |= _BV(PORTD4);
    DDRD |= _BV(DDD4);

    /* start tick timer */
    tick_set_period(TBTICKS_FROM_MS(1000));
    tick_enable(1);
}
