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
#ifndef _LIBRB_H_
#define _LIBRB_H_

#include <avr/io.h>

/*
 * ring-buffer control stucture
 */
struct ring_buffer {
    uint8_t * start;
    uint8_t * limit;
    uint8_t * volatile put;
    uint8_t * volatile echo;
    uint8_t * volatile get;

    uint8_t flags;
};

/*
 * ring-buffer state flag bits
 */
enum {
    RB_CANTPUT_MSK = _BV(0),
    RB_CANTECHO_MSK = _BV(1),
    RB_CANTGET_MSK = _BV(2),
    RB_SPARE0_MSK = _BV(3),
    RB_SPARE1_MSK = _BV(4),
    RB_SPARE2_MSK = _BV(5),
    RB_SPARE3_MSK = _BV(6),
    RB_SPARE4_MSK = _BV(7),
};

/*
 * ring-buffer state
 */
#define set_cantput(a) ((a)->flags |= RB_CANTPUT_MSK)
#define clr_cantput(a) ((a)->flags &= ~RB_CANTPUT_MSK)
#define rb_cantput(a) ((a)->flags & RB_CANTPUT_MSK)
#define set_cantecho(a) ((a)->flags |= RB_CANTECHO_MSK)
#define clr_cantecho(a) ((a)->flags &= ~RB_CANTECHO_MSK)
#define rb_cantecho(a) ((a)->flags & RB_CANTECHO_MSK)
#define set_cantget(a) ((a)->flags |= RB_CANTGET_MSK)
#define clr_cantget(a) ((a)->flags &= ~RB_CANTGET_MSK)
#define rb_cantget(a) ((a)->flags & RB_CANTGET_MSK)
#define rb_empty(a) (rb_cantecho(a) && rb_cantget(a))
#define rb_full(a) rb_cantput(a)

/*
 * clear ring-buffer
 */
#define rb_clear(a) do {                                                       \
    (a)->flags &= ~(RB_CANTECHO_MSK | RB_CANTGET_MSK);                         \
    (a)->get = (a)->echo = (a)->put = (a)->start;                              \
} while (0)

/*
 * increment and wrap buffer pointer
 */
#define rb_inc_ptr(a,b) do {                                                   \
    b++;                                                                       \
    if (b == (a)->limit) b = (a)->start;                                       \
} while (0)

/*
 * (un)wrap and decrement buffer pointer
 */
#define rb_dec_ptr(a,b) do {                                                   \
    if (b == (a)->start) b = (a)->limit;                                       \
    b--;                                                                       \
} while (0)

/*
 * ring-buffer control/access functions
 */
extern void rb_init(struct ring_buffer * const rb, uint8_t * const p, size_t s);
extern int8_t rb_put(struct ring_buffer * const rb, volatile uint8_t const * const b);
extern int8_t rb_put_echo(struct ring_buffer * const rb, volatile uint8_t const * const b);
extern int8_t rb_unput(struct ring_buffer * const rb, volatile uint8_t * const b);
extern int8_t rb_echo(struct ring_buffer * const rb, volatile uint8_t * const b);
extern int8_t rb_get(struct ring_buffer * const rb, volatile uint8_t * const b);
extern uint8_t rb_erase(struct ring_buffer * const rb);
extern uint8_t rb_kill(struct ring_buffer * const rb, uint8_t * p);

#endif /* _LIBRB_H_ */
