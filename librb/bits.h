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
#ifndef _BITS_H_
#define _BITS_H_

#define _BVl(a) (1UL<<(a))

#define set_bits_cond(a,b,c)  set_field_bits((a),(b),(c)?(b):0)

static __inline uint8_t test_bits(uint8_t volatile * const address,
                                  uint8_t test_data)
{
    return *address & test_data;
}

static __inline uint8_t test_bit(uint8_t volatile * const address, uint8_t bit)
{
    return *address & _BV(bit);
}

static __inline uint8_t set_bits(uint8_t volatile * const address,
                                 uint8_t set_data)
{
    uint8_t initial_value;

    initial_value = *address;
    *address = initial_value | set_data;

    return initial_value;
}

static __inline uint8_t set_bit(uint8_t volatile * const address, uint8_t bit)
{
    return set_bits(address, _BV(bit));
}

static __inline uint8_t clear_bits(uint8_t volatile * const address,
                                   uint8_t clear_data)
{
    uint8_t initial_value;

    initial_value = *address;
    *address = initial_value & ~clear_data;

    return initial_value;
}

static __inline uint8_t clear_bit(uint8_t volatile * const address, uint8_t bit)
{
    return clear_bits(address, _BV(bit));
}

static __inline uint8_t set_clear_bits(uint8_t volatile * const address,
                                       uint8_t set_data, uint8_t clear_data)
{
    uint8_t initial_value;

    initial_value = *address;
    *address = (initial_value & ~clear_data) | set_data;

    return initial_value;
}

/*
 * Read an eight bit location and replace the bits defined by "field_mask" with
 * the bits from "value".  Returns the initial value stored at location.  This
 * API is used to update a field and save the initial value for later
 * restoration.
 */
static __inline uint8_t set_field_bits(uint8_t volatile * const address,
                                       uint8_t field_mask, uint8_t data)
{
    uint8_t initial_value;

    initial_value = *address;
    *address = (initial_value & ~field_mask) | (data & field_mask);

    return initial_value;
}

#endif /* _BITS_H_ */
