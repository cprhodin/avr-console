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

#include "ilitek_fastio.h"

#include "bits.h"
#include "pinmap.h"

#include "timers.h"

#include "itty.h"

#include <avr/io.h>
#include <avr/pgmspace.h>

#define SPI_DISABLED             (_BV(MSTR) | _BV(CPOL) | _BV(CPHA))
#define SPI_ENABLED   (_BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA))
#define spi_enable()  do {SPCR = SPI_ENABLED;} while (0)
#define spi_disable() do {SPCR = SPI_DISABLED;} while (0)

/*
 * Load ILITEK command, data sequence from FLASH.
 */
void Xilitek_load_sequence_P(uint8_t const * sequence) __attribute__((__naked__));
void Xilitek_load_sequence_P(uint8_t const * sequence)
{
    /*
     * Set T flag to indicate sequence is in FLASH.
     */
    __asm__ __volatile__ (
        "               set                                          \n"
        :::
    );

    /*
     * Jump to the shared load sequence code.
     */
    __asm__ __volatile__ (
        "               # Move sequence pointer to Z                 \n"
        "               movw            r30, r24                     \n"
        "               rjmp            __ilitek_load_sequence       \n"
        :::
    );
}

/*
 * Load ILITEK command, data sequence from RAM.
 */
void Xilitek_load_sequence(uint8_t const * sequence) __attribute__((__naked__));
void Xilitek_load_sequence(uint8_t const * sequence)
{
    /*
     * Clear T flag to indicate sequence is in RAM.
     */
    __asm__ __volatile__ (
        "               clt                                          \n"
        "               movw            r30, r24                     \n"
        :::
    );

    /*
     * Fall through to the shared load sequence code.
     */
}

/*
 * Load ILITEK command, data sequence shared code.
 *
 *  Params: T flag, r31:r30
 *  Uses:   r0, r1 (always returns 0)
 */
void __ilitek_load_sequence(uint8_t const * sequence) __attribute__((__naked__, __used__));
void __ilitek_load_sequence(uint8_t const * sequence)
{
    __asm__ __volatile__ (
        "       .macro  ldt             dst, src                     \n"
        "               #                                            \n"
        "               # T flag determines the memory space.        \n"
        "               #                                            \n"
        "               brts            . + 4                        \n"
        "               ld              \\dst, \\src                 \n"
        "               rjmp            . + 2                        \n"
        "               lpm             \\dst, \\src                 \n"
        "       .endm                                                \n"
        "                                                            \n"
        "               # Wait for SPI idle                          \n"
        "1:             in              __tmp_reg__, %[spsr]         \n"
        "               sbrs            __tmp_reg__, %[spif]         \n"
        "               rjmp            1b                           \n"
        "                                                            \n"
        "               # Read command length, exit if zero          \n" //      ok  ok  ok  ok
        "2:             ldt             __zero_reg__, Z+             \n" // 5    9   .   9   .
        "               tst             __zero_reg__                 \n" // 1    10  .   10  .
        "               breq            4f                           \n" // 1/2  11  .   11  .
        "                                                            \n"
        "               # Read and send command                      \n"
        "               ldt             __tmp_reg__, Z+              \n" // 5    16  .   16  .
        "               cbi             %[portd], %[ilitek_a0]       \n" // 2    18  .   18  .
        "               out             %[spdr], __tmp_reg__         \n" // 1   *1  *1   .   .
        "               dec             __zero_reg__                 \n" // 1    2   2   .   .
        "               breq            2b                           \n" // 1/2  4   3   .   .
        "                                                            \n"
        "               clr             __tmp_reg__                  \n" // 1    .   10  .   .
        "               inc             __tmp_reg__                  \n" // 1    .   .   .   .
        "3:             dec             __tmp_reg__                  \n" // 1    .   .   .   9
        "               brpl            3b                           \n" // 1    .   .   .   10
        "               nop                                          \n" // 1    .   11  .   11
        "                                                            \n"
        "               # Read and send command parameter            \n"
        "               ldt             __tmp_reg__, Z+              \n" // 5    .   16  .   16
        "               sbi             %[portd], %[ilitek_a0]       \n" // 2    .   18  .   18
        "               out             %[spdr], __tmp_reg__         \n" // 1    .   .  *1  *1
        "               dec             __zero_reg__                 \n" // 1    .   .   2   2
        "               breq            2b                           \n" // 1/2  .   .   4   3
        "               nop                                          \n" // 1    .   .   .   4
        "               nop                                          \n" // 1    .   .   .   5
        "               clr             __tmp_reg__                  \n" // 1    .   .   .   6
        "               rjmp            3b                           \n" // 2    .   .   .   8
        "                                                            \n"
        "4:             ret                                          \n"
        :
        : /* i/o registers */
          [portd] "I" _SFR_IO_ADDR(PORTD),
          [spsr]  "I" _SFR_IO_ADDR(SPSR),
          [spdr]  "I" _SFR_IO_ADDR(SPDR),
          /* i/o register bits */
          [ilitek_a0] "I" (PORTD7),
          [spif]   "I" (SPIF)
        : "r1", "r30", "r31", "cc"
    );
}

/*
 * Write out a block of pixels color
 */
void ilitek_fill_color(icount_t count, icolor_t color) __attribute__((__naked__));
void ilitek_fill_color(icount_t count, icolor_t color)
{
    __asm__ __volatile__ (
        "               sbiw            r24, 1                       \n" // 2    2
        "               brcs            6f                           \n" // 1/2  3
        "                                                            \n"
        "1:             in              r26, %[spsr]                 \n" // 1    4
        "               sbrs            r26, %[spif]                 \n" // 1/2  6
        "               rjmp            1b                           \n" // 2
        "               sbi             %[portd], %[ilitek_a0]       \n" // 2    8
        "                                                            \n"
        "2:             out             %[spdr], r23                 \n" // 1   *1
        "               ldi             r26, 3                       \n" // 1    2
        "3:             dec             r26                          \n" // 1    3 +6 +9 +12
        "               brpl            3b                           \n" // 1    5 +8 +11+13
        "               nop                                          \n" // 1    14
        "               sbiw            r24, 1                       \n" // 2    16
        "               brcs            5f                           \n" // 1/2  17/18
        "               nop                                          \n" // 1    18
        "                                                            \n"
        "               out             %[spdr], r22                 \n" // 1   *1
        "               ldi             r26, 4                       \n" // 1    2
        "4:             dec             r26                          \n" // 1    3 +6 +9 +12+15
        "               brpl            4b                           \n" // 1    5 +8 +11+14+16
        "               rjmp            2b                           \n" // 2    18
        "                                                            \n"
        "5:             out             %[spdr], r22                 \n" // 1
        "6:             ret                                          \n" // 4
        :
        : /* i/o registers */
          [portd] "I" _SFR_IO_ADDR(PORTD),
          [spsr]  "I" _SFR_IO_ADDR(SPSR),
          [spdr]  "I" _SFR_IO_ADDR(SPDR),
          /* i/o register bits */
          [ilitek_a0] "I" (PORTD7),
          [spif]   "I" (SPIF)
        :
    );
}

/*
 * Write out a single pixels color.
 */
void ilitek_set_pixel_color(icolor_t color) __attribute__((__naked__));
void ilitek_set_pixel_color(icolor_t color)
{
    __asm__ __volatile__ (
        "               movw            r22, r24                     \n" // 1
        "               ldi             r24, 0x00                    \n" // 1
        "               ldi             r25, 0x00                    \n" // 1
        "               rjmp            ilitek_fill_color + 4        \n" // 2
        :::
    );
}

void ilitek_fill_bitmap_P(icount_t count, uint8_t const * bitmap, icolor_t color0, icolor_t color1) __attribute__((__naked__));
void ilitek_fill_bitmap_P(icount_t count, uint8_t const * bitmap, icolor_t color0, icolor_t color1)
{
    /*
     * Set T flag to indicate bitmap is in FLASH.
     */
    __asm__ __volatile__ (
        "               set                                          \n"
        :::
    );

    /*
     * Jump to the shared blit bitmap code.
     */
    __asm__ __volatile__ (
        "               rjmp            __ilitek_fill_bitmap         \n"
        :::
    );
}

void ilitek_fill_bitmap(icount_t count, uint8_t const * bitmap, icolor_t color0, icolor_t color1) __attribute__((__naked__));
void ilitek_fill_bitmap(icount_t count, uint8_t const * bitmap, icolor_t color0, icolor_t color1)
{
    /*
     * Clear T flag to indicate bitmap is in RAM.
     */
    __asm__ __volatile__ (
        "               clt                                          \n"
        :::
    );

    /*
     * Fall through to the shared blit bitmap code.
     */
}

static void __ilitek_fill_bitmap(icount_t count, uint8_t const * bitmap, icolor_t color0, icolor_t color1) __attribute__((__naked__, __used__));
static void __ilitek_fill_bitmap(icount_t count, uint8_t const * bitmap, icolor_t color0, icolor_t color1)
{
    __asm__ __volatile__ (
        "               # Move bitmap pointer to Z                   \n"
        "               movw            r30, r22                     \n"
        "                                                            \n"
        "               # Wait for SPI idle                          \n"
        "1:             in              r27, %[spsr]                 \n"
        "               sbrs            r27, %[spif]                 \n"
        "               rjmp            1b                           \n"
        "                                                            \n"
        "               # Set A0 to indicate data                    \n"
        "               sbi             %[portd], %[ilitek_a0]       \n"
        "                                                            \n"
        "               # Jump to loop entry point                   \n"
        "               rjmp            4f                           \n"
        "                                                            \n"
        "               # Start next full byte                       \n"
        "2:             ldi             r26, 8                       \n" // 1       7
        "               nop                                          \n" // 1       8
        "               rjmp            5f                           \n" // 2       10
        "                                                            \n"
        "3:             out             %[spdr], r22                 \n" // 1   *   1  1
        "               nop                                          \n" // 1       2  2
        "                                                            \n"
        "               # Loop start                                 \n" //
        "4:             sbiw            r24, 8                       \n" // 2       4  4
        "               brsh            2b                           \n" // 1/2     6  5
        "               adiw            r24, 8                       \n" // 2          7
        "               breq            9f                           \n" // 1/2        8
        "               mov             r26, r24                     \n" // 1          9
        "               clr             r24                          \n" // 1          10
        "                                                            \n"
        "               # Load next byte                             \n"
        "5:             ldt             __tmp_reg__, Z+              \n" // 5       15 15
        "                                                            \n"
        "               # Bit value determines color                 \n"
        "6:             movw            r22, r18                     \n" // 1    16 16 16
        "               sbrs            __tmp_reg__, 7               \n" // 1(2) 17 17 17
        "               movw            r22, r20                     \n" // 1(0) 18 18 18
        "                                                            \n"
        "               out             %[spdr], r23                 \n" // 1   *1  1  1
        "               ldi             r27, 3                       \n" // 1    2  2  2
        "7:             dec             r27                          \n" // 1    3 +6 +9 +12
        "               brpl            7b                           \n" // 1/2  5 +8 +11+13
        "               nop                                          \n" // 1    14 14 14
        "               nop                                          \n" // 1    15 15 15
        "               dec             r26                          \n" // 1    16 16 16
        "               breq            3b                           \n" // 1/2  17 18 18
        "               lsl             __tmp_reg__                  \n" // 1    18
        "               out             %[spdr], r22                 \n" // 1   *1
        "               ldi             r27, 3                       \n" // 1    2
        "8:             dec             r27                          \n" // 1    3 +6 +9 +12
        "               brpl            8b                           \n" // 1/2  5 +8 +11+13
        "               rjmp            6b                           \n" // 2    15
        "                                                            \n"
        "9:             ret                                          \n"
        :
        : /* i/o registers */
          [portd] "I" _SFR_IO_ADDR(PORTD),
          [spsr]  "I" _SFR_IO_ADDR(SPSR),
          [spdr]  "I" _SFR_IO_ADDR(SPDR),
          /* i/o register bits */
//          [ilitek_a0] "I" (PORTD7),
          [ilitek_a0] "I" (PORTD7),
          [spif]   "I" (SPIF)
        :
    );
}

/*
 * Set the display data access window.  Leaves the ILITEK ready to accept data.
 */
void ilitek_set_access_window(idim_t x, idim_t x_size, idim_t y, idim_t y_size)
{
    static uint8_t set_access_window_seq[] = {
        GR_COMMAND(SET_COLUMN_ADDRESS, 4),
            0x00, 0x00, 0x00, 0x00,
        GR_COMMAND(SET_PAGE_ADDRESS, 4),
            0x00, 0x00, 0x00, 0x00,
        GR_COMMAND(WRITE_MEMORY_START, 0),
        GR_STOP
    };

    /* set the coordinates */
    x += X_ORIGIN;
    set_access_window_seq[2] = x >> 8;
    set_access_window_seq[3] = x;
    x += x_size - 1;
    set_access_window_seq[4] = x >> 8;
    set_access_window_seq[5] = x;

    y += Y_ORIGIN;
    set_access_window_seq[8] = y >> 8;
    set_access_window_seq[9] = y;
    y += y_size - 1;
    set_access_window_seq[10] = y >> 8;
    set_access_window_seq[11] = y;

    /* send it to the ILITEK */
    ilitek_load_sequence(set_access_window_seq);
}

/*
 * Fill the ILITEK color lookup table with a flat set of entries.
 */
void ilitek_load_default_lut(void)
{
    static const uint8_t load_default_lut_seq[] __ATTR_PROGMEM__ = {
        GR_COMMAND(WRITE_LUT, 128),
             0,  2,  4,  6,  8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30,
            33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63,
             0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
            16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
            32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
            48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
             0,  2,  4,  6,  8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30,
            33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63,
        GR_STOP
    };

    ilitek_load_sequence_P(load_default_lut_seq);
}

/*
 * Fill the whole display with a single color.  Leaves the ILITEK ready to accept data.
 */
void ilitek_fill_root_window(icolor_t color)
{
    static const uint8_t root_fill_seq[] __ATTR_PROGMEM__ = {
        GR_COMMAND(SET_COLUMN_ADDRESS, 4),
            GR_WORD(X_ORIGIN), GR_WORD(X_ORIGIN + X_SIZE - 1),
        GR_COMMAND(SET_PAGE_ADDRESS, 4),
            GR_WORD(Y_ORIGIN), GR_WORD(Y_ORIGIN + Y_SIZE - 1),
        GR_COMMAND(WRITE_MEMORY_START, 0),
        GR_STOP
    };

    ilitek_load_sequence_P(root_fill_seq);
    ilitek_fill_color((icount_t) X_SIZE * Y_SIZE, color);
}

/*
 * Set the color of the specified pixel.
 */
void ilitek_draw_pixel(idim_t x, idim_t y, icolor_t color)
{
    static uint8_t draw_pixel_seq[] = {
        GR_COMMAND(SET_COLUMN_ADDRESS, 4),
            0x00, 0x00, GR_WORD(X_ORIGIN + X_SIZE - 1),
        GR_COMMAND(SET_PAGE_ADDRESS, 4),
            0x00, 0x00, GR_WORD(Y_ORIGIN + Y_SIZE - 1),
        GR_COMMAND(WRITE_MEMORY_START, 2),
            0x00, 0x00,
        GR_STOP
    };

    /* set the coordinates */
    x += X_ORIGIN;
    draw_pixel_seq[2] = x >> 8;
    draw_pixel_seq[3] = x;

    y += Y_ORIGIN;
    draw_pixel_seq[8] = y >> 8;
    draw_pixel_seq[9] = y;

    /* set the color */
    draw_pixel_seq[14] = color >> 8;
    draw_pixel_seq[15] = color;

    /* send it to the ILITEK */
    ilitek_load_sequence(draw_pixel_seq);
}

/*
 * Fill the specified rectangle with a single color.
 */
void ilitek_draw_rect(idim_t x, idim_t x_size, idim_t y, idim_t y_size, icolor_t color)
{
    ilitek_set_access_window(x, x_size, y, y_size);
    ilitek_fill_color(x_size * y_size, color);
}

void ilitek_send_command(uint8_t command)
{
    while (!bit_test(&SPSR, SPIF));
    pinmap_clear(ILITEK_A0);
    SPDR = command;
}

void ilitek_send_data(uint8_t data)
{
    while (!bit_test(&SPSR, SPIF));
    pinmap_set(ILITEK_A0);
    SPDR = data;
}

/*
 * Draw a one pixel wide box.
 */
void ilitek_draw_box(irect_s rect, icolor_t color)
{
    ilitek_draw_rect(rect.x, rect.x_size, rect.y,                   1, color);
    ilitek_draw_rect(rect.x, rect.x_size, rect.y + rect.y_size - 1, 1, color);
    ilitek_draw_rect(rect.x,                   1, rect.y + 1, rect.y_size - 2, color);
    ilitek_draw_rect(rect.x + rect.x_size - 1, 1, rect.y + 1, rect.y_size - 2, color);
}

void ilitek_draw_bitmap_P(idim_t x, idim_t x_size, idim_t y, idim_t y_size,
                          uint8_t const * bitmap, icolor_t color0, icolor_t color1)
{
    ilitek_set_access_window(x, x_size, y, y_size);
    ilitek_fill_bitmap_P(x_size * y_size, bitmap, color0, color1);
}

void ilitek_draw_bitmap(idim_t x, idim_t x_size, idim_t y, idim_t y_size,
                        uint8_t const * bitmap, icolor_t color0, icolor_t color1)
{
    ilitek_set_access_window(x, x_size, y, y_size);
    ilitek_fill_bitmap(x_size * y_size, bitmap, color0, color1);
}

static uint8_t draw_mask_seq[] = {
    GR_COMMAND(SET_COLUMN_ADDRESS, 4),
        0x00, 0x00, 0x00, 0x00,
    GR_COMMAND(SET_PAGE_ADDRESS, 4),
        0x00, 0x00, 0x00, 0x00,
    GR_COMMAND(WRITE_MEMORY_START, 2),
        0x00, 0x00,
    GR_STOP
};

void ilitek_draw_mask_P(idim_t x, idim_t x_size, idim_t y, idim_t y_size,
                        uint8_t const * mask, icolor_t color)
{
    uint16_t xs;
    uint16_t xe;
    uint16_t ys;
    uint16_t ye;
    uint8_t byte;
    uint8_t bit;

    /* set the color */
    draw_mask_seq[14] = color >> 8;
    draw_mask_seq[15] = color;

    /* translate to physical coordinates */
    xs = X_ORIGIN + x;
    ys = Y_ORIGIN + y;
    xe = xs + x_size - 1;
    ye = ys + y_size - 1;

    /* the bottom right coordinates are fixed */
    draw_mask_seq[4] = xe >> 8;
    draw_mask_seq[5] = xe;
    draw_mask_seq[10] = ye >> 8;
    draw_mask_seq[11] = ye;

    bit = 0;
    for (y = ys; y <= ye; y++) {
        for (x = xs; x <= xe; x++) {
            if (bit == 0) {
                byte = pgm_read_byte(mask++);
                bit = 8;
            }

            if (byte & 0x80) {
                draw_mask_seq[2] = x >> 8;
                draw_mask_seq[3] = x;
                draw_mask_seq[8] = y >> 8;
                draw_mask_seq[9] = y;
                Xilitek_load_sequence(draw_mask_seq);
            }
            byte <<= 1;
            bit--;
        }
    }
}

void ilitek_draw_mask(idim_t x, idim_t x_size, idim_t y, idim_t y_size,
                      uint8_t const * mask, icolor_t color)
{
    uint16_t xs;
    uint16_t xe;
    uint16_t ys;
    uint16_t ye;
    uint8_t byte;
    uint8_t bit;

    /* set the color */
    draw_mask_seq[14] = color >> 8;
    draw_mask_seq[15] = color;

    /* translate to physical coordinates */
    xs = X_ORIGIN + x;
    ys = Y_ORIGIN + y;
    xe = xs + x_size - 1;
    ye = ys + y_size - 1;

    /* the bottom right coordinates are fixed */
    draw_mask_seq[4] = xe >> 8;
    draw_mask_seq[5] = xe;
    draw_mask_seq[10] = ye >> 8;
    draw_mask_seq[11] = ye;

    bit = 0;
    for (y = ys; y <= ye; y++) {
        for (x = xs; x <= xe; x++) {
            if (bit == 0) {
                byte = *mask++;
                bit = 8;
            }

            if (byte & 0x80) {
                draw_mask_seq[2] = x >> 8;
                draw_mask_seq[3] = x;
                draw_mask_seq[8] = y >> 8;
                draw_mask_seq[9] = y;
                Xilitek_load_sequence(draw_mask_seq);
            }
            byte <<= 1;
            bit--;
        }
    }
}

static const uint8_t ilitek_init_seq[] __ATTR_PROGMEM__ = {
    GR_COMMAND(SET_PIXEL_FORMAT, 1),
        0x05,
    GR_COMMAND(SET_GAMMA_CURVE, 1),
        0x04,
    GR_COMMAND(GAM_R_SEL, 1),
        0x01,
    GR_COMMAND(POSITIVE_GAMMA_CORRECT, 15),
        0x3f, 0x25, 0x1c, 0x1e, 0x20, 0x12, 0x2a, 0x90,
        0x24, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00,
    GR_COMMAND(NEGATIVE_GAMMA_CORRECT, 15),
        0x20, 0x20, 0x20, 0x20, 0x05, 0x00, 0x15, 0xa7,
        0x3d, 0x18, 0x25, 0x2a, 0x2b, 0x2b, 0x3a,
    GR_COMMAND(FRAME_RATE_CONTROL1, 2),
        0x08, 0x08,
    GR_COMMAND(DISPLAY_INVERSION, 1),
        0x01,
    GR_COMMAND(POWER_CONTROL1, 2),
        0x0a, 0x02,
    GR_COMMAND(POWER_CONTROL2, 1),
        0x02,
    GR_COMMAND(VCOM_CONTROL1, 2),
        0x50, 0x5b,
    GR_COMMAND(VCOM_OFFSET_CONTROL, 1),
        0x40,
    GR_COMMAND(SET_ADDRESS_MODE, 1),
        0x00,
    GR_STOP
};

/*
 * Hard reset LCD and initialize interface.
 */
void ilitek_init(void)
{
    /* initialize the SPI and LCD pins */
    pinmap_set(ILITEK_SCK | ILITEK_SI | ILITEK_CS | ILITEK_RES | ILITEK_A0);
    pinmap_dir(0, ILITEK_SCK | ILITEK_SI | ILITEK_CS | ILITEK_RES | ILITEK_A0);

    /* pulse ILITEK_RES low */
    pinmap_clear(ILITEK_RES);
    timebase_delay(TBTICKS_FROM_US(20));
    pinmap_set(ILITEK_RES);

    /* initialize the SPI port */
    spi_disable();
    SPSR = _BV(SPI2X);
    spi_enable();

    /* 120 ms internal reset processing */
    timer_delay(TBTICKS_FROM_MS(120));

    /* select the LCD and send a NOP to prime SPIF */
    pinmap_clear(ILITEK_CS);
    pinmap_clear(ILITEK_A0);
    SPDR = NOP;

#if 0
    /* bring the LCD out of sleep mode */
    ilitek_send_command(EXIT_SLEEP_MODE);
    while (!bit_test(&SPSR, SPIF));

    /* 120 ms internal reset processing */
    timer_delay(TBTICKS_FROM_MS(120));

    ilitek_load_sequence_P(ilitek_init_seq);

    ilitek_load_default_lut();

    ilitek_fill_root_window(0x0000);

    ilitek_send_command(SET_DISPLAY_ON);
#endif
}
