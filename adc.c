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
#include <util/atomic.h>

#include "bits.h"
#include "pinmap.h"

/* GPIOR00: new sample ready flag */
/* GPIOR01: restart */
uint16_t adc_ch6;
uint16_t adc_ch7;

static int8_t conversion_count;
static uint16_t adcw_ch6;
static uint16_t adcw_ch7;

ISR(TIMER1_COMPB_vect)
{
    uint16_t ocr1b;
    uint16_t admux;
    uint16_t adcw;

    /*
     * reset compare register to trigger in another 2500 cycles
     */
    ocr1b = OCR1B;
    OCR1B = ocr1b + 2500;

    /*
     * read the previous conversion result, update the count
     */
    adcw = ADCW;
    conversion_count++;

    /*
     * the current mux value applies to the conversion that was just
     * started by timer 1 compare B trigger
     */
    admux = ADMUX;

    /*
     * handle initialization and restart by discarding initial results
     *
     *  conversion_count  <  0: conversion result discarded
     *  conversion_count ==  0: start oversampling
     *  conversion_count  < 64: collect conversion results
     *  conversion_count >= 64: post-process and signal
     */
    if      (conversion_count == 0) {
        adcw_ch6 = 0;
        adcw_ch7 = 0;
        set_bit(&GPIOR0, GPIOR01);
    }
    else if (conversion_count > 0) {
        /*
         * accumulate ADC result
         */
        if (admux & 0x1)
            adcw_ch6 += adcw;
        else
            adcw_ch7 += adcw;

        /*
         * post results
         */
        if (conversion_count >= 64) {
            conversion_count = 0;

            adc_ch6 = adcw_ch6;
            adcw_ch6 = 0;

            adc_ch7 = adcw_ch7;
            adcw_ch7 = 0;

            set_bit(&GPIOR0, GPIOR00);
        }
    }

    /*
     * a minimum of 1 ADC clock or 128 CPU clocks must elapse before
     * changing the input multiplexer for the next conversion
     */
    while ((TCNT1 - ocr1b) < 128);

    /*
     * switch between input 6 and 7
     */
    ADMUX = admux ^ 0x01;
}

void adc_restart(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // MUX0  0, 0 // channel 6
        // MUX1  1, 1 //
        // MUX2  2, 1 //
        // MUX3  3, 0 //
        // rsvd  4, 0 //
        // ADLAR 5, 0 // right adjusted
        // REFS0 6, 1 // AVCC reference
        // REFS1 7, 0 //
        ADMUX = 0x46;

        conversion_count = -2;

        clear_bit(&GPIOR0, GPIOR00);
        clear_bit(&GPIOR0, GPIOR01);
    }
}

void adc_init(void)
{
    // ADC power on
    clear_bit(&PRR, PRADC);

    // ADPS0 0, 1 // divide by 128
    // ADPS1 1, 1 //
    // ADPS2 2, 1 //
    // ADIE  3, 0 //
    // ADIF  4, 1 //
    // ADATE 5, 1 //
    // ADSC  6, 0 //
    // ADEN  7, 1 // enable
    ADCSRA = 0xb7;

    // ADTS0 0, 1 // Timer/Counter1 Compare Match B
    // ADTS1 1, 0 //
    // ADTS2 2, 1 //
    // rsvd  3, 0 //
    // rsvd  4, 0 //
    // rsvd  5, 0 //
    // ACME  6, 0 // comparator mux
    // rsvd  7, 0 //
    ADCSRB = 0x05;

    // disable digital functionality on ADC0-ADC5
    // ADC0D 0, 1 //
    // ADC1D 1, 1 //
    // ADC2D 2, 1 //
    // ADC3D 3, 1 //
    // ADC4D 4, 1 //
    // ADC5D 5, 1 //
    // rsvd  6, 0 //
    // rsvd  7, 0 //
    pinmap_set_did(PINMAP_ADC5 | PINMAP_ADC4 | PINMAP_ADC3 |
                   PINMAP_ADC2 | PINMAP_ADC1 | PINMAP_ADC0);
    pinmap_clear(PINMAP_ADC5 | PINMAP_ADC4 | PINMAP_ADC3 |
                 PINMAP_ADC2 | PINMAP_ADC1 | PINMAP_ADC0);
    pinmap_dir(PINMAP_ADC5 | PINMAP_ADC4 | PINMAP_ADC3 |
               PINMAP_ADC2 | PINMAP_ADC1 | PINMAP_ADC0, 0);

    // start the trigger timer and enable interrupt
    OCR1B = 2500;
    TIFR1 |= _BV(OCF1B);
    TIMSK1 |= _BV(OCIE1B);

    adc_restart();
}
