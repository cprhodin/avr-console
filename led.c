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

#include "bits.h"
#include "pinmap.h"

#define SEG_a 0x40
#define SEG_b 0x20
#define SEG_c 0x10
#define SEG_d 0x08
#define SEG_e 0x04
#define SEG_f 0x02
#define SEG_g 0x01

uint8_t digits[] = {
    /* 0 */ SEG_a | SEG_b | SEG_c | SEG_d | SEG_e | SEG_f        ,
    /* 1 */         SEG_b | SEG_c                                ,
    /* 2 */ SEG_a | SEG_b |         SEG_d | SEG_e |         SEG_g,
    /* 3 */ SEG_a | SEG_b | SEG_c | SEG_d |                 SEG_g,
    /* 4 */         SEG_b | SEG_c |                 SEG_f | SEG_g,
    /* 5 */ SEG_a |         SEG_c | SEG_d |         SEG_f | SEG_g,
    /* 6 */ SEG_a |         SEG_c | SEG_d | SEG_e | SEG_f | SEG_g,
    /* 7 */ SEG_a | SEG_b | SEG_c                                ,
    /* 8 */ SEG_a | SEG_b | SEG_c | SEG_d | SEG_e | SEG_f | SEG_g,
    /* 9 */ SEG_a | SEG_b | SEG_c | SEG_d |         SEG_f | SEG_g,
};

#define SPI_DISABLED (_BV(MSTR) | _BV(SPR1))
#define SPI_ENABLED (SPI_DISABLED | _BV(SPE))
#define spi_enable() do {SPCR = SPI_ENABLED;} while (0)
#define spi_disable() do {SPCR = SPI_DISABLED;} while (0)

void led_send(uint16_t data)
{
    data |= 0x8000;

    while (!test_bit(&SPSR, SPIF));
    SPDR = data >> 8;
    while (!test_bit(&SPSR, SPIF));
    SPDR = data;
    while (!test_bit(&SPSR, SPIF));
    SPDR = 0;
    while (!test_bit(&SPSR, SPIF));
    SPDR = 0;
    while (!test_bit(&SPSR, SPIF));
    SPDR = 0;
}

void led_module_update(uint8_t module, uint16_t data)
{
    uint32_t enable = 0;

    switch (module) {
    case 0:
        enable = LED_ENA0;
        break;
    case 1:
        enable = LED_ENA1;
        break;
    case 2:
        enable = LED_ENA2;
        break;
    case 3:
        enable = LED_ENA3;
        break;
    }

    pinmap_clear(enable);
    led_send(data);
    pinmap_set(LED_ENA0 | LED_ENA1 | LED_ENA2 | LED_ENA3);
}

void led_write_number(uint8_t line, uint16_t number)
{
    char buffer[6];
    uint8_t i;

    sprintf(buffer, "%5d", number);

    for (i = 0; i < 5; i++) {
        if (buffer[i] == ' ')
            buffer[i] = 0x00;
        else
            buffer[i] = digits[buffer[i] & 0x0f];
    }

    line <<= 1;

    led_module_update(line, (buffer[1] << 8) | buffer[2]);
    led_module_update(line + 1, (buffer[3] << 8) | buffer[4]);
}

void led_init(void)
{
    /* initialize the LED pins */
    pinmap_set(LED_SS | LED_ENA0 | LED_ENA1 | LED_ENA2 | LED_ENA3);
    pinmap_clear(LED_DATA | LED_CLOCK);
    pinmap_dir(0, LED_SS | LED_DATA | LED_CLOCK | LED_BRT |
                  LED_ENA0 | LED_ENA1 | LED_ENA2 | LED_ENA3);

    /* initialize LED brightness timer at 50% */
    TCCR0A = _BV(COM0A1) | _BV(COM0A0) | _BV(WGM00);
    OCR0A = 128;

    /* initialize the SPI port */
    spi_disable();
    SPSR = _BV(SPI2X);
    spi_enable();

    SPDR = 0x00;

    /* flush out any erroneous start bits from the LED */
    pinmap_clear(LED_ENA0 | LED_ENA1 | LED_ENA2 | LED_ENA3);
    led_send(0);
    led_send(0x7f7f);
    pinmap_set(LED_ENA0 | LED_ENA1 | LED_ENA2 | LED_ENA3);

    led_write_number(0, 1111);
    led_write_number(1, 2222);
}
