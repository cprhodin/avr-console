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
#ifndef _PINMAP_H_
#define _PINMAP_H_

#include "bits.h"

#include <stdint.h>

/*
 * ATmega328P pin definitions.
 *
 *  DEFINE_PIN(name,port,bit)
 */
#define ATmega328P_PINS                                                         \
    DEFINE_PIN(ATmega328P_PB0, PORTB, 0, ICP1, CLKO, PCINT0),
#define ATmega328P_PB1
#define ATmega328P_PB2
#define ATmega328P_PB3
#define ATmega328P_PB4
#define ATmega328P_PB5
#define ATmega328P_PB6
#define ATmega328P_PB7
#define ATmega328P_PC0
#define ATmega328P_PC1
#define ATmega328P_PC2
#define ATmega328P_PC3
#define ATmega328P_PC4
#define ATmega328P_PC5
#define ATmega328P_PC6
#define ATmega328P_PD0
#define ATmega328P_PD1
#define ATmega328P_PD2
#define ATmega328P_PD3
#define ATmega328P_PD4
#define ATmega328P_PD5
#define ATmega328P_PD6
#define ATmega328P_PD7
#define ATmega328P_ADC6
#define ATmega328P_ADC7



#define pin_t uint8_t
#define pinmap_t uint32_t

/* Port B */
#define PIN_PCINT7     (8+PORTB7)
#define PINMAP_PCINT7  _BVl(PIN_PCINT7)
#define PIN_PCINT6     (8+PORTB6)
#define PINMAP_PCINT6  _BVl(PIN_PCINT6)
#define PIN_SCK        (8+PORTB5)
#define PINMAP_SCK     _BVl(PIN_SCK)
#define PIN_PCINT5     (8+PORTB5)
#define PINMAP_PCINT5  _BVl(PIN_PCINT5)
#define PIN_MISO       (8+PORTB4)
#define PINMAP_MISO    _BVl(PIN_MISO)
#define PIN_PCINT4     (8+PORTB4)
#define PINMAP_PCINT4  _BVl(PIN_PCINT4)
#define PIN_MOSI       (8+PORTB3)
#define PINMAP_MOSI    _BVl(PIN_MOSI)
#define PIN_OC2A       (8+PORTB3)
#define PINMAP_OC2A    _BVl(PIN_OC2A)
#define PIN_PCINT3     (8+PORTB3)
#define PINMAP_PCINT3  _BVl(PIN_PCINT3)
#define PIN_SS         (8+PORTB2)
#define PINMAP_SS      _BVl(PIN_SS)
#define PIN_OC1B       (8+PORTB2)
#define PINMAP_OC1B    _BVl(PIN_OC1B)
#define PIN_PCINT2     (8+PORTB2)
#define PINMAP_PCINT2  _BVl(PIN_PCINT2)
#define PIN_OC1A       (8+PORTB1)
#define PINMAP_OC1A    _BVl(PIN_OC1A)
#define PIN_PCINT1     (8+PORTB1)
#define PINMAP_PCINT1  _BVl(PIN_PCINT1)
#define PIN_ICP1       (8+PORTB0)
#define PINMAP_ICP1    _BVl(PIN_ICP1)
#define PIN_CLKO       (8+PORTB0)
#define PINMAP_CLKO    _BVl(PIN_CLKO)
#define PIN_PCINT0     (8+PORTB0)
#define PINMAP_PCINT0  _BVl(PIN_PCINT0)


/* Port C */
#define PIN_PCINT14    (16+PORTC6)
#define PINMAP_PCINT14 _BVl(PIN_PCINT14)
#define PIN_ADC5       (16+PORTC5)
#define PINMAP_ADC5    _BVl(PIN_ADC5)
#define PIN_SCL        (16+PORTC5)
#define PINMAP_SCL     _BVl(PIN_SCL)
#define PIN_PCINT13    (16+PORTC5)
#define PINMAP_PCINT13 _BVl(PIN_PCINT13)
#define PIN_ADC4       (16+PORTC4)
#define PINMAP_ADC4    _BVl(PIN_ADC4)
#define PIN_SDA        (16+PORTC4)
#define PINMAP_SDA     _BVl(PIN_SDA)
#define PIN_PCINT12    (16+PORTC4)
#define PINMAP_PCINT12 _BVl(PIN_PCINT12)
#define PIN_ADC3       (16+PORTC3)
#define PINMAP_ADC3    _BVl(PIN_ADC3)
#define PIN_PCINT11    (16+PORTC3)
#define PINMAP_PCINT11 _BVl(PIN_PCINT11)
#define PIN_ADC2       (16+PORTC2)
#define PINMAP_ADC2    _BVl(PIN_ADC2)
#define PIN_PCINT10    (16+PORTC2)
#define PINMAP_PCINT10 _BVl(PIN_PCINT10)
#define PIN_ADC1       (16+PORTC1)
#define PINMAP_ADC1    _BVl(PIN_ADC1)
#define PIN_PCINT9     (16+PORTC1)
#define PINMAP_PCINT9  _BVl(PIN_PCINT9)
#define PIN_ADC0       (16+PORTC0)
#define PINMAP_ADC0    _BVl(PIN_ADC0)
#define PIN_PCINT8     (16+PORTC0)
#define PINMAP_PCINT8  _BVl(PIN_PCINT8)


/* Port D */
#define PIN_D7         (24+PORTD7)
#define PINMAP_D7      _BVl(PIN_D7)
#define PIN_AIN1       (24+PORTD7)
#define PINMAP_AIN1    _BVl(PIN_AIN1)
#define PIN_PCINT23    (24+PORTD7)
#define PINMAP_PCINT23 _BVl(PIN_PCINT23)
#define PIN_D6         (24+PORTD6)
#define PINMAP_D6      _BVl(PIN_D6)
#define PIN_AIN0       (24+PORTD6)
#define PINMAP_AIN0    _BVl(PIN_AIN0)
#define PIN_OC0A       (24+PORTD6)
#define PINMAP_OC0A    _BVl(PIN_OC0A)
#define PIN_PCINT22    (24+PORTD6)
#define PINMAP_PCINT22 _BVl(PIN_PCINT22)
#define PIN_D5         (24+PORTD5)
#define PINMAP_D5      _BVl(PIN_D5)
#define PIN_T1         (24+PORTD5)
#define PINMAP_T1      _BVl(PIN_T1)
#define PIN_OC0B       (24+PORTD5)
#define PINMAP_OC0B    _BVl(PIN_OC0B)
#define PIN_PCINT21    (24+PORTD5)
#define PINMAP_PCINT21 _BVl(PIN_PCINT21)
#define PIN_D4         (24+PORTD4)
#define PINMAP_D4      _BVl(PIN_D4)
#define PIN_XCK        (24+PORTD4)
#define PINMAP_XCK     _BVl(PIN_XCK)
#define PIN_T0         (24+PORTD4)
#define PINMAP_T0      _BVl(PIN_T0)
#define PIN_PCINT20    (24+PORTD4)
#define PINMAP_PCINT20 _BVl(PIN_PCINT20)
#define PIN_D3         (24+PORTD3)
#define PINMAP_D3      _BVl(PIN_D3)
#define PIN_INT1       (24+PORTD3)
#define PINMAP_INT1    _BVl(PIN_INT1)
#define PIN_OC2B       (24+PORTD3)
#define PINMAP_OC2B    _BVl(PIN_OC2B)
#define PIN_PCINT19    (24+PORTD3)
#define PINMAP_PCINT19 _BVl(PIN_PCINT19)
#define PIN_D2         (24+PORTD2)
#define PINMAP_D2      _BVl(PIN_D2)
#define PIN_INT0       (24+PORTD2)
#define PINMAP_INT0    _BVl(PIN_INT0)
#define PIN_PCINT18    (24+PORTD2)
#define PINMAP_PCINT18 _BVl(PIN_PCINT18)
#define PIN_D1         (24+PORTD1)
#define PINMAP_D1      _BVl(PIN_D1)
#define PIN_TXD        (24+PORTD1)
#define PINMAP_TXD     _BVl(PIN_TXD)
#define PIN_PCINT17    (24+PORTD1)
#define PINMAP_PCINT17 _BVl(PIN_PCINT17)
#define PIN_D0         (24+PORTD0)
#define PINMAP_D0      _BVl(PIN_D0)
#define PIN_RXD        (24+PORTD0)
#define PINMAP_RXD     _BVl(PIN_RXD)
#define PIN_PCINT16    (24+PORTD0)
#define PINMAP_PCINT16 _BVl(PIN_PCINT16)


static __inline void pin_set(pin_t pin)
{
         if (pin >= 24) bit_set(&PORTD, pin - 24);
    else if (pin >= 16) bit_set(&PORTC, pin - 16);
    else if (pin >=  8) bit_set(&PORTB, pin -  8);
}

static __inline void pin_set_ddr(pin_t pin)
{
         if (pin >= 24) bit_set(&DDRD, pin - 24);
    else if (pin >= 16) bit_set(&DDRC, pin - 16);
    else if (pin >=  8) bit_set(&DDRB, pin -  8);
}

static __inline void pin_set_pin(pin_t pin)
{
         if (pin >= 24) bit_set(&PIND, pin - 24);
    else if (pin >= 16) bit_set(&PINC, pin - 16);
    else if (pin >=  8) bit_set(&PINB, pin -  8);
}

static __inline void pin_clear(pin_t pin)
{
         if (pin >= 24) bit_clear(&PORTD, pin - 24);
    else if (pin >= 16) bit_clear(&PORTC, pin - 16);
    else if (pin >=  8) bit_clear(&PORTB, pin -  8);
}

static __inline void pin_clear_ddr(pin_t pin)
{
         if (pin >= 24) bit_clear(&DDRD, pin - 24);
    else if (pin >= 16) bit_clear(&DDRC, pin - 16);
    else if (pin >=  8) bit_clear(&DDRB, pin -  8);
}

static __inline void pin_clear_pin(pin_t pin)
{
         if (pin >= 24) bit_clear(&PIND, pin - 24);
    else if (pin >= 16) bit_clear(&PINC, pin - 16);
    else if (pin >=  8) bit_clear(&PINB, pin -  8);
}

static __inline void pinmap_set(pinmap_t pinmap)
{
    if (pinmap & (0xffUL << 24)) bitmask_set(&PORTD, pinmap >> 24);
    if (pinmap & (0xffUL << 16)) bitmask_set(&PORTC, pinmap >> 16);
    if (pinmap & (0xffUL <<  8)) bitmask_set(&PORTB, pinmap >>  8);
}

static __inline void pinmap_set_ddr(pinmap_t pinmap)
{
    if (pinmap & (0xffUL << 24)) bitmask_set(&DDRD, pinmap >> 24);
    if (pinmap & (0xffUL << 16)) bitmask_set(&DDRC, pinmap >> 16);
    if (pinmap & (0xffUL <<  8)) bitmask_set(&DDRB, pinmap >>  8);
}

static __inline void pinmap_set_pin(pinmap_t pinmap)
{
    if (pinmap & (0xffUL << 24)) bitmask_set(&PIND, pinmap >> 24);
    if (pinmap & (0xffUL << 16)) bitmask_set(&PINC, pinmap >> 16);
    if (pinmap & (0xffUL <<  8)) bitmask_set(&PINB, pinmap >>  8);
}

static __inline void pinmap_set_did(pinmap_t pinmap)
{
    if (pinmap & (0x3fUL << 16)) bitmask_set(&DIDR0, (pinmap >> 16) & 0x3f);
}

static __inline void pinmap_clear(pinmap_t pinmap)
{
    if (pinmap & (0xffUL << 24)) bitmask_clear(&PORTD, pinmap >> 24);
    if (pinmap & (0xffUL << 16)) bitmask_clear(&PORTC, pinmap >> 16);
    if (pinmap & (0xffUL <<  8)) bitmask_clear(&PORTB, pinmap >>  8);
}

static __inline void pinmap_clear_ddr(pinmap_t pinmap)
{
    if (pinmap & (0xffUL << 24)) bitmask_clear(&DDRD, pinmap >> 24);
    if (pinmap & (0xffUL << 16)) bitmask_clear(&DDRC, pinmap >> 16);
    if (pinmap & (0xffUL <<  8)) bitmask_clear(&DDRB, pinmap >>  8);
}

static __inline void pinmap_clear_pin(pinmap_t pinmap)
{
    if (pinmap & (0xffUL << 24)) bitmask_clear(&PIND, pinmap >> 24);
    if (pinmap & (0xffUL << 16)) bitmask_clear(&PINC, pinmap >> 16);
    if (pinmap & (0xffUL <<  8)) bitmask_clear(&PINB, pinmap >>  8);
}

static __inline void pinmap_clear_did(pinmap_t pinmap)
{
    if (pinmap & (0x3fUL << 16)) bitmask_clear(&DIDR0, (pinmap >> 16) & 0x3f);
}

static __inline void pinmap_dir(pinmap_t inmap, pinmap_t outmap)
{
    if (inmap) pinmap_clear_ddr(inmap);
    if (outmap) pinmap_set_ddr(outmap);
}

#endif /* _PINMAP_H_ */
