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
#ifndef _RB_H_
#define _RB_H_

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
 * ring-buffer state flags
 */
#define RB_CANTPUT 0x01
#define RB_CANTECHO 0x02
#define RB_CANTGET 0x04

/*
 * set ring-buffer state
 */
#define set_cantput(a) ((a)->flags |= RB_CANTPUT)
#define clr_cantput(a) ((a)->flags &= ~RB_CANTPUT)
#define set_cantecho(a) ((a)->flags |= RB_CANTECHO)
#define clr_cantecho(a) ((a)->flags &= ~RB_CANTECHO)
#define set_cantget(a) ((a)->flags |= RB_CANTGET)
#define clr_cantget(a) ((a)->flags &= ~RB_CANTGET)

/*
 * ring-buffer state tests
 */
#define rb_cantput(a) ((a)->flags & RB_CANTPUT)
#define rb_cantecho(a) ((a)->flags & RB_CANTECHO)
#define rb_cantget(a) ((a)->flags & RB_CANTGET)
#define rb_empty(a) (rb_cantecho(a) && rb_cantget(a))
#define rb_full(a) rb_cantput(a)

/*
 * clear ring-buffer
 */
#define rb_clear(a) do {                                                       \
    (a)->flags = RB_CANTECHO | RB_CANTGET;                                     \
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

#endif // _RING_BUFFER_H_
