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

#ifndef _MATHOPS_H_
#define _MATHOPS_H_

/*
 * unsigned divide, 32-bit / 16-bit -> 16-bit
 */
static inline uint16_t _divu(uint32_t dividend, uint16_t divisor)
{
    uint8_t count = 17;

    __asm__ __volatile__ (
        "               rjmp            2f                                   \n"

        "1:             brcs            3f                                   \n"

        "2:             cp              %C[dividend], %A[divisor]            \n"
        "               cpc             %D[dividend], %B[divisor]            \n"
        "               brcs            4f                                   \n"
        "3:             sub             %C[dividend], %A[divisor]            \n"
        "               sbc             %D[dividend], %B[divisor]            \n"
        "               clc                                                  \n"

        "4:             rol             %A[dividend]                         \n"
        "               rol             %B[dividend]                         \n"
        "               rol             %C[dividend]                         \n"
        "               rol             %D[dividend]                         \n"

        "               dec             %[count]                             \n"
        "               brne            1b                                   \n"

        "               com             %A[dividend]                         \n"
        "               com             %B[dividend]                         \n"
        : [dividend] "=&r" (dividend),
          [count] "=&r" (count)
        : "0" (dividend),
          "1" (count),
          [divisor] "r" (divisor)
        : "cc"
    );

    return (uint16_t) dividend;
}

/*
 * unsigned divide, 64(32)-bit / 32-bit -> 32-bit
 *
 *  dividend is implicitly multiplied by 2^32
 */
static inline uint32_t _ummd32(uint32_t dividend, uint32_t divisor)
{
    uint32_t quotient;
    uint8_t count = 33;

    __asm__ __volatile__ (
        "               rjmp            2f                                   \n"

        "1:             brcs            3f                                   \n"

        "2:             cp              %A[dividend], %A[divisor]            \n"
        "               cpc             %B[dividend], %B[divisor]            \n"
        "               cpc             %C[dividend], %C[divisor]            \n"
        "               cpc             %D[dividend], %D[divisor]            \n"
        "               brcs            4f                                   \n"
        "3:             sub             %A[dividend], %A[divisor]            \n"
        "               sbc             %B[dividend], %B[divisor]            \n"
        "               sbc             %C[dividend], %C[divisor]            \n"
        "               sbc             %D[dividend], %D[divisor]            \n"
        "               clc                                                  \n"

        "4:             rol             %A[quotient]                         \n"
        "               rol             %B[quotient]                         \n"
        "               rol             %C[quotient]                         \n"
        "               rol             %D[quotient]                         \n"
        "               lsl             %A[dividend]                         \n"
        "               rol             %B[dividend]                         \n"
        "               rol             %C[dividend]                         \n"
        "               rol             %D[dividend]                         \n"

        "               dec             %[count]                             \n"
        "               brne            1b                                   \n"

        "               com             %A[quotient]                         \n"
        "               com             %B[quotient]                         \n"
        "               com             %C[quotient]                         \n"
        "               com             %D[quotient]                         \n"
        : [dividend] "=&r" (dividend),
          [count] "=&r" (count),
          [quotient] "=&r" (quotient)
        : "0" (dividend),
          "1" (count),
          [divisor] "r" (divisor)
        : "cc"
    );

    return quotient;
}


/*
 * unsigned multiply, 16-bit x 16-bit -> 32-bit
 */
static inline uint32_t _mulu(uint16_t multiplier, uint16_t multiplicand)
{
    uint32_t product;

    __asm__ __volatile__ (
        /*
         */
        "               mul            %A[multiplier], %A[multiplicand]      \n"
        "               movw           %A[product],    r0                    \n"

        /*
         */
        "               mul            %B[multiplier], %B[multiplicand]      \n"
        "               movw           %C[product],    r0                    \n"

        /*
         */
        "               mul            %A[multiplier], %B[multiplicand]      \n"
        "               add            %B[product],    r0                    \n"
        "               adc            %C[product],    r1                    \n"
        "               clr            __zero_reg__                          \n"
        "               adc            %D[product],    __zero_reg__          \n"

        /*
         */
        "               mul            %B[multiplier], %A[multiplicand]      \n"
        "               add            %B[product],    r0                    \n"
        "               adc            %C[product],    r1                    \n"
        "               clr            __zero_reg__                          \n"
        "               adc            %D[product],    __zero_reg__          \n"
        : [product] "=&r" (product)
        : [multiplier] "r" (multiplier),
          [multiplicand] "r" (multiplicand)
        : "r0", "r1", "cc"
    );

    return product;
}


/*
 * unsigned multiply-divide with 32-bit intermediate result
 */
static inline uint16_t _ummd(uint16_t multiplier, uint16_t multiplicand, uint16_t divisor)
{
    return _divu(_mulu(multiplier, multiplicand), divisor);
}


/*
 * unsigned multiply-divide with 32-bit intermediate result, result rounded
 */
static inline uint16_t _ummdr(uint16_t multiplier, uint16_t multiplicand, uint16_t divisor)
{
    return _divu(_mulu(multiplier, multiplicand) + (divisor / 2), divisor);
}


uint16_t ummd(uint16_t multiplier, uint16_t multiplicand, uint16_t divisor);
uint16_t ummdr(uint16_t multiplier, uint16_t multiplicand, uint16_t divisor);

#endif /* _MATHOPS_H_ */
