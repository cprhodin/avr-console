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
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "librb.h"

/*
 * Special characters recognized for translation and processing.
 */
#define CR      ('\r')          // carriage return
#define ERASE   ('\b')          // backspace
#define KILL    ('U' & ~0x40)   // ctrl-U
#define NL      ('\n')          // newline (line feed)
#define SPACE   (' ')           // space

/*
 * Transmit and receive ring-buffers.
 */
#ifndef TX_BUF_SIZE
#define TX_BUF_SIZE (32)
#endif
#ifndef RX_BUF_SIZE
#define RX_BUF_SIZE (32)
#endif
static uint8_t tx_buffer[TX_BUF_SIZE];
static uint8_t rx_buffer[RX_BUF_SIZE];
static struct ring_buffer tx_rb;
static struct ring_buffer rx_rb;

/*
 * Used in canonical mode to mark the beginning of the current line.
 */
static uint8_t * volatile line_start;

/*
 * Internal state variables used for I/O translation and processing.
 */
#define set_onlcr_state() (tx_rb.flags |= RB_SPARE0_MSK)
#define clr_onlcr_state() (tx_rb.flags &= ~RB_SPARE0_MSK)
#define is_onlcr_state() (tx_rb.flags & RB_SPARE0_MSK)
#define set_erase_state1() (tx_rb.flags |= RB_SPARE1_MSK)
#define set_erase_state2() (tx_rb.flags |= RB_SPARE2_MSK)
#define clr_erase_state() (tx_rb.flags &= ~(RB_SPARE1_MSK | RB_SPARE2_MSK))
#define is_erase_state1() (tx_rb.flags & RB_SPARE1_MSK)
#define is_erase_state2() (tx_rb.flags & RB_SPARE2_MSK)

/*
 * Terminal attributes.
 */
#define set_icrnl() (rx_rb.flags |= RB_SPARE0_MSK)      // translate received CR to NL
#define clr_icrnl() (rx_rb.flags &= ~RB_SPARE0_MSK)
#define is_icrnl() (rx_rb.flags & RB_SPARE0_MSK)
#define set_onlcr() (rx_rb.flags |= RB_SPARE1_MSK)      // translate transmitted NL to CR-NL
#define clr_onlcr() (rx_rb.flags &= ~RB_SPARE1_MSK)
#define is_onlcr() (rx_rb.flags & RB_SPARE1_MSK)
#define set_echo() (rx_rb.flags |= RB_SPARE2_MSK)       // echo received characters to transmitter
#define clr_echo() (rx_rb.flags &= ~RB_SPARE2_MSK)
#define is_echo() (rx_rb.flags & RB_SPARE2_MSK)
#define set_icanon() (rx_rb.flags |= RB_SPARE3_MSK)     // enable canonical mode processing
#define clr_icanon() (rx_rb.flags &= ~RB_SPARE3_MSK)
#define is_icanon() (rx_rb.flags & RB_SPARE3_MSK)
#define set_inonblock() (rx_rb.flags |= RB_SPARE4_MSK)  // block until operation is complete
#define clr_inonblock() (rx_rb.flags &= ~RB_SPARE4_MSK)
#define is_inonblock() (rx_rb.flags & RB_SPARE4_MSK)


static uint8_t erase_count;


/*
 * Enable transmitter.
 */
#define tx_enable() do {                                                       \
    UCSR0B = (UCSR0B & ~_BV(TXCIE0)) | (_BV(UDRIE0) | _BV(TXEN0));             \
} while (0)

/*
 * Enable transmit complete interrupt.
 */
#define tx_complete() do {                                                     \
    UCSR0B = (UCSR0B & ~_BV(UDRIE0)) | (_BV(TXCIE0) | _BV(TXEN0));             \
} while (0)

/*
 * Disable transmitter.
 */
#define tx_disable() do {                                                      \
    UCSR0B &= ~(_BV(UDRIE0) | _BV(TXCIE0) | _BV(TXEN0));                       \
} while (0)


/*
 * Enable receiver.
 */
#define rx_enable() do {                                                       \
    UCSR0B = UCSR0B | (_BV(RXEN0) | _BV(RXCIE0));                              \
} while (0)

/*
 * Disable receiver (interrupt).
 */
#define rx_disable() do {                                                      \
    UCSR0B = UCSR0B & ~_BV(RXCIE0);                                            \
} while (0)


/*
 * Tx complete interrupt handler
 */
ISR(USART_TX_vect)
{
    tx_disable();
}


/*
 * Tx data register empty interrupt handler
 */
ISR(USART_UDRE_vect)
{
    uint8_t c;

    if (erase_count) {
        /*
         * (ECHOE) Echo error correcting ERASE.
         */
        if (!is_erase_state1()) {
            set_erase_state1();
            UDR0 = ERASE;
            return;
        }

        if (!is_erase_state2()) {
            set_erase_state2();
            UDR0 = SPACE;
            return;
        }

        erase_count--;
        clr_erase_state();
        c = ERASE;
    }
    else if (is_onlcr_state()) {
        /*
         * (ONLCR) Complete NL to CR-NL expansion.
         */
        clr_onlcr_state();
        c = NL;
    }
    else {
        /*
         * Get next echo or output byte.
         */
        if (rb_echo(&rx_rb, &c) < 0) rb_get(&tx_rb, &c);

        if (is_onlcr() && (c == NL)) {
            /*
             * (ONLCR) Start NL to CR-NL expansion.
             */
            set_onlcr_state();
            UDR0 = CR;
            return;
        }
    }

    /*
     * Put byte to USART.
     */
    UDR0 = c;

    if (rb_cantget(&tx_rb) && rb_cantecho(&rx_rb) &&
        !erase_count && !is_onlcr_state()) {
        tx_complete();
    }
}


/*
 * Rx complete interrupt handler
 */
ISR(USART_RX_vect)
{
    uint8_t c;

    /*
     * Get byte from USART data register.
     */
    c = UDR0;

    /*
     * If enabled translate a received carriage-return (CR) to a newline (NL).
     */
    if (is_icrnl() && (c == CR)) c = NL;

    /*
     * Canonical Mode Input Processing
     */
    if (is_icanon()) {
        switch (c) {
        case ERASE:
            /*
             * Handle the ERASE character if the current line is not empty.
             */
            if ((rx_rb.put != line_start) && rb_erase(&rx_rb) && is_echo()) {
                erase_count++;
                tx_enable();
            }

            /*
             * ERASE character is discarded.
             */
            return;
        case KILL:
            /*
             * Handle the KILL character if the current line is not empty.
             * rb_kill returns 0 if echo is not enabled.
             */
            erase_count += rb_kill(&rx_rb, line_start);

            if (erase_count) tx_enable();

            /*
             * KILL character is discarded.
             */
            return;
        }
    }

    /*
     * Buffer incoming character.
     */
    if (!(is_echo() ? rb_put_echo(&rx_rb, &c) : rb_put(&rx_rb, &c)) &&
        is_icanon() && (c != NL)) {
        /*
         * in canonical mode the last character in the buffer must be a newline.
         */
        rb_erase(&rx_rb);
    }
    else if (is_echo()) tx_enable();

    /*
     * A newline means the end of the current line and and the beginning of
     * a new current line.
     */
    if (c == NL) line_start = rx_rb.put;

    if (rb_full(&rx_rb)) rx_disable();
}


/*
 * putchar
 */
static int console_putchar(char c, struct __file * stream)
{
    for (;;) {
        cli();

        if (rb_put(&tx_rb, (uint8_t *) &c) >= 0) break;

        /* Wait for an interrupt before trying again. */
        SMCR = SLEEP_MODE_IDLE | _BV(SE);
        sei();
        sleep_cpu();
        SMCR = SLEEP_MODE_IDLE;
    }

    tx_enable();
    sei();

    return 0;
}


/*
 * getchar
 */
static int console_getchar(struct __file * stream)
{
    char c;

    for (;;) {
        cli();

        /*
         * In canonical mode wait for the current line to be complete or the
         * buffer to be full, which is an error condition and should never
         * happen.
         */
        if (!is_icanon() || (rx_rb.get != line_start) || rb_full(&rx_rb))
            if (rb_get(&rx_rb, (uint8_t *) &c) >= 0) break;

        if (is_inonblock()) {
            sei();
            return _FDEV_EOF;
        }

        /* Wait for an interrupt before trying again. */
        SMCR = SLEEP_MODE_IDLE | _BV(SE);
        sei();
        sleep_cpu();
        SMCR = SLEEP_MODE_IDLE;
    }

    rx_enable();
    sei();

    return c;
}


static FILE console = FDEV_SETUP_STREAM(console_putchar, console_getchar,
                                        _FDEV_SETUP_RW);

/*
 * Initialize console interface. Must be called with interrupts disabled.
 */
void console_init(void)
{
    /*
     * Initialize the transmit and receive ring buffers.
     */
    rb_init(&tx_rb, tx_buffer, sizeof(tx_buffer));
    rb_init(&rx_rb, rx_buffer, sizeof(rx_buffer));

    /*
     * Current line is empty.
     */
    line_start = rx_rb.put; /* line_start also equals echo and get */

    /*
     * State variables for processing.
     */
    erase_count = 0;
    clr_erase_state();
    clr_onlcr_state();

    /*
     * Default attributes.
     */
    set_icrnl();
    set_onlcr();
    set_echo();
    set_icanon();
    clr_inonblock();

    /*
     * Attach standard I/O to this console.
     */
    stdin = &console;
    stdout = &console;
    stderr = &console;

    /*
     * Set baud rate.  This value is calculated from the symbol BAUD.
     */
    UBRR0 = UBRR_VALUE;

    /*
     * Set frame format; 8 data bits, 2 stop bits.
     */
    UCSR0A = 0;
    UCSR0B = 0;
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01) | _BV(USBS0);

    /*
     * Turn on the receiver, the transmitter remains off until needed.
     */
    rx_enable();
}
