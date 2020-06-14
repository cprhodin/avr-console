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
#ifndef _TIMER_H_
#define _TIMER_H_

/*
 * timebase counter size in bits
 */
#ifndef TBSIZE
#error "TBSIZE undefined, set to 16 or 32."
#endif

/*
 * timebase timer and compare register
 */
#ifndef TBTIMER
#error "TBTIMER undefined; set to 0, 1, or 2."
#endif

#ifndef TBTIMER_COMP
/* default to the lower priority B compare */
#define TBTIMER_COMP B
#endif

/*
 * types and constants to support the timebase counter
 */
#if   TBSIZE == 16
#define tbtick_t uint16_t
#define tbtick_st int16_t
#elif TBSIZE == 32
#define tbtick_t uint32_t
#define tbtick_st int32_t
#else
#error "Invalid TBSIZE, must be 16 or 32."
#endif

#define TIMEBASE_MAX_LATENCY (1L<<(TBSIZE-1))
#define TIMEBASE_MAX_DELAY (TIMEBASE_MAX_LATENCY-2)

/*
 * types and constants to support the timebase timer
 */
#if   TBTIMER == 0 || TBTIMER == 2
#define TBTSIZE 8
#define TBTIMER_MOD (256UL)
#define tbtimer_t uint8_t
#define tbtimer_st int8_t
#elif TBTIMER == 1
#define TBTSIZE 16
#define TBTIMER_MOD (65536UL)
#define tbtimer_t uint16_t
#define tbtimer_st int16_t
#endif

#define TBTIMER_MAX_LATENCY (TBTIMER_MOD / 2)
#define TBTIMER_MAX_DELAY (TBTIMER_MAX_LATENCY-2)

#define _TBJOIN2(a,b) a##b
#define _TBJOIN3(a,b,c) a##b##c
#define _TBJOIN5(a,b,c,d,e) a##b##c##d##e

#define _TBTCNT(a,b) _TBJOIN2(TCNT,a)
#define _TBTCNTL(a,b) _TBJOIN3(TCNT,a,L)
#define _TBTCNTH(a,b) _TBJOIN3(TCNT,a,H)
#define _TBOCR(a,b) _TBJOIN3(OCR,a,b)
#define _TBTIFR(a,b) _TBJOIN2(TIFR,a)
#define _TBTIMSK(a,b) _TBJOIN2(TIMSK,a)
#define _TBOCF(a,b) _TBJOIN3(OCF,a,b)
#define _TBOCIE(a,b) _TBJOIN3(OCIE,a,b)
#define _TBTIMER_COMP_vect(a,b) _TBJOIN5(TIMER,a,_COMP,b,_vect)

#define TBTCNT _TBTCNT(TBTIMER,TBTIMER_COMP)
#define TBTCNTL _TBTCNTL(TBTIMER,TBTIMER_COMP)
#define TBTCNTH _TBTCNTH(TBTIMER,TBTIMER_COMP)
#define TBOCR _TBOCR(TBTIMER,TBTIMER_COMP)
#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
#define TBTIFR _TBTIFR(TBTIMER,TBTIMER_COMP)
#define TBTIMSK _TBTIMSK(TBTIMER,TBTIMER_COMP)
#else
#define TBTIFR TIFR
#define TBTIMSK TIMSK
#endif
#define TBOCF _TBOCF(TBTIMER,TBTIMER_COMP)
#define TBOCIE _TBOCIE(TBTIMER,TBTIMER_COMP)
#define TBTIMER_COMP_vect _TBTIMER_COMP_vect(TBTIMER,TBTIMER_COMP)

/*
 * timer event
 */
struct timer_event {
    struct timer_event * next;
    tbtick_t tbtick;
    int8_t (* handler)(struct timer_event * this_timer_event);
};

#define TIMER_EVENT_INIT(name,handler) { &name, 0, handler }
#define TIMER_EVENT(name,handler)                                              \
        int8_t handler(struct timer_event * this_timer_event);                 \
        struct timer_event name = TIMER_EVENT_INIT(name,handler)

#define init_timer_event(a,b,c)                                                \
    do {                                                                       \
        (a)->next = (a);                                                       \
        (a)->tbtick = (b);                                                     \
        (a)->handler = (c);                                                    \
    } while (0)

#if (F_CPU == ((F_CPU / 1000UL) * 1000UL))
#define KCLOCKS_PER_MS (F_CPU / 1000UL)
#define TBTICKS_FROM_MS(m) ((tbtick_t) UDIV_CEILING(KCLOCKS_PER_MS * (m),      \
                                                    TBTIMER_PRESCALER))
#else
#error "Insert timer math here."
#endif

#if (F_CPU == ((F_CPU / 1000000UL) * 1000000UL))
#define MCLOCKS_PER_US (F_CPU / 1000000UL)
#define TBTICKS_FROM_US(u) ((tbtick_t) UDIV_CEILING(MCLOCKS_PER_US * (u),      \
                                                    TBTIMER_PRESCALER))
//#define US_FROM_TBTICKS(t) ((tbtick_t) UDIV_CEILING(TBTIMER_PRESCALER * (t),
//                                                    MCLOCKS_PER_US))
#define US_FROM_TBTICKS(t) ((TBTIMER_PRESCALER / MCLOCKS_PER_US) * (t))
#else
#error "Insert timer math here."
#endif

#define timer_is_expired(a) (((volatile struct timer_event *) (a))->next == (a))
#define timer_is_active(a) (!timer_is_expired(a))

/*
 * timebase api
 */
extern tbtick_t timebase_update(void);
extern void timebase_init(void);
extern tbtimer_t timebase_get(void);
extern void timebase_delay(tbtimer_st tbticks);

/*
 * timer event api
 */
extern void schedule_timer_event(struct timer_event * this_timer_event, struct timer_event * ref_timer_event);
extern void cancel_timer_event(struct timer_event * this_timer_event);
extern void timer_delay(tbtick_st ticks);

#endif /* _TIMER_H_ */
