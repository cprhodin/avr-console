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

#include "tick.h"
#include "timer.h"
#include "pinmap.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <stdint.h>

/* tick timer period, default 1 second */
static uint32_t tick_period = TBTICKS_FROM_MS(1000);


TIMER_EVENT(tick_off_event, tick_off_handler);

static int8_t tick_off_handler(struct timer_event * this_timer_event)
{
    /* output high, OFF */
    pinmap_set(SPEAKER_OUT);

    /* don't reschedule this timer */
    return 0;
}


TIMER_EVENT(tick_timer_event, tick_timer_handler);

static int8_t tick_timer_handler(struct timer_event * this_timer_event)
{
    /* output low, ON */
    pinmap_clear(SPEAKER_OUT);

    /* schedule the output off timer for 2 ms */
    tick_off_event.tbtick = TBTICKS_FROM_MS(2);
    schedule_timer_event(&tick_off_event, this_timer_event);

    /* advance this timer one period */
    this_timer_event->tbtick += tick_period;

    /* reschedule this timer */
    return 1;
}


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
    pinmap_set(SPEAKER_OUT);
    pinmap_dir(0, SPEAKER_OUT);

    /* start tick timer */
    tick_enable(1);
}
