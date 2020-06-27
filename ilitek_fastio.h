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
#ifndef _ILITEK_FASTIO_H_
#define _ILITEK_FASTIO_H_

#include <stdint.h>

typedef uint16_t icolor_t;
typedef uint16_t idim_t;
typedef uint32_t icount_t;

typedef struct ipoint {
    idim_t x;
    idim_t y;
} ipoint_s;

typedef struct irect {
    idim_t x;
    idim_t x_size;
    idim_t y;
    idim_t y_size;
} irect_s;

/*
 * ILITEK controller commands.
 */
#define NOP                     0x00
#define SOFT_RESET              0x01
#define GET_RED_CHANNEL         0x06
#define GET_GREEN_CHANNEL       0x07
#define GET_BLUE_CHANNEL        0x08
#define GET_PIXEL_FORMAT        0x0C
#define GET_POWER_MODE          0x0A
#define GET_ADDRESS_MODE        0x0B
#define GET_DISPLAY_MODE        0x0D
#define GET_SIGNAL_MODE         0x0E
#define GET_DIAGNOSTIC_RESULT   0x0F
#define ENTER_SLEEP_MODE        0x10
#define EXIT_SLEEP_MODE         0x11
#define ENTER_PARTIAL_MODE      0x12
#define ENTER_NORMAL_MODE       0x13
#define EXIT_INVERT_MODE        0x20
#define ENTER_INVERT_MODE       0x21
#define SET_GAMMA_CURVE         0x26
#define SET_DISPLAY_OFF         0x28
#define SET_DISPLAY_ON          0x29
#define SET_COLUMN_ADDRESS      0x2A
#define SET_PAGE_ADDRESS        0x2B
#define WRITE_MEMORY_START      0x2C
#define WRITE_LUT               0x2D
#define READ_MEMORY_START       0x2E
#define SET_PARTIAL_AREA        0x30
#define SET_SCROLL_AREA         0x33
#define SET_TEAR_OFF            0x34
#define SET_TEAR_ON             0x35
#define SET_ADDRESS_MODE        0x36
#define SET_SCROLL_START        0X37
#define EXIT_IDLE_MODE          0x38
#define ENTER_IDLE_MODE         0x39
#define SET_PIXEL_FORMAT        0x3A
#define WRITE_MEMORY_CONTINUE   0x3C
#define READ_MEMORY_CONTINUE    0x3E
#define SET_TEAR_SCANLINE       0x44
#define GET_SCANLINE            0x45
#define READ_ID1                0xDA
#define READ_ID2                0xDB
#define READ_ID3                0xDC
#define FRAME_RATE_CONTROL1     0xB1
#define FRAME_RATE_CONTROL2     0xB2
#define FRAME_RATE_CONTROL3     0xB3
#define DISPLAY_INVERSION       0xB4
#define SOURCE_DRIVER_DIRECTION 0xB7
#define GATE_DRIVER_DIRECTION   0xB8
#define POWER_CONTROL1          0xC0
#define POWER_CONTROL2          0xC1
#define POWER_CONTROL3          0xC2
#define POWER_CONTROL4          0xC3
#define POWER_CONTROL5          0xC4
#define VCOM_CONTROL1           0xC5
#define VCOM_CONTROL2           0xC6
#define VCOM_OFFSET_CONTROL     0xC7
#define WRITE_ID4_VALUE         0xD3
#define NV_MEMORY_FUNCTION1     0xD7
#define NV_MEMORY_FUNCTION2     0xDE
#define POSITIVE_GAMMA_CORRECT  0xE0
#define NEGATIVE_GAMMA_CORRECT  0xE1
#define GAM_R_SEL               0xF2

/*
 * Macros to create ILITEK command, data sequences.
 */
#define GR_COMMAND(c,p) (p) + 1, (c)
#define GR_WORD(w) ((uint8_t) ((w) >> 8)), ((uint8_t) (w))
#define GR_STOP 0

/*
 * Load ILITEK command, data sequence from FLASH.
 */
static inline void ilitek_load_sequence_P(uint8_t const * sequence)
{
    /*
     * Set T flag to indicate sequence is in ROM.
     */
    __asm__ __volatile__ (
        "               set                                          \n"
        "               call            __ilitek_load_sequence       \n"
        : [sequence] "=&z" (sequence)
        : "0" (sequence)
        : "r1", "cc"
    );
}

/*
 * Load ILITEK command, data sequence from RAM.
 */
static inline void ilitek_load_sequence(uint8_t const * sequence)
{
    /*
     * Clear T flag to indicate sequence is in RAM.
     */
    __asm__ __volatile__ (
        "               clt                                          \n"
        "               call            __ilitek_load_sequence       \n"
        : [sequence] "=&z" (sequence)
        : "0" (sequence)
        : "r1", "cc"
    );
}

extern void ilitek_fill_color(icount_t count, icolor_t color);
extern void ilitek_set_pixel_color(icolor_t color);
extern void ilitek_fill_bitmap_P(icount_t count, uint8_t const * bitmap, icolor_t color0, icolor_t color1);
extern void ilitek_fill_bitmap(icount_t count, uint8_t const * bitmap, icolor_t color0, icolor_t color1);
extern void ilitek_set_access_window(idim_t x, idim_t x_size, idim_t y, idim_t y_size);
extern void ilitek_load_default_lut(void);
extern void ilitek_fill_root_window(icolor_t color);
extern void ilitek_draw_pixel(idim_t x, idim_t y, icolor_t color);
extern void ilitek_draw_rect(idim_t x, idim_t x_size, idim_t y, idim_t y_size, icolor_t color);
extern void ilitek_send_command(uint8_t command);
extern void ilitek_send_data(uint8_t data);
extern void ilitek_draw_box(irect_s rect, icolor_t color);
extern void ilitek_draw_bitmap_P(idim_t x, idim_t x_size, idim_t y, idim_t y_size, uint8_t const * bitmap, icolor_t color0, icolor_t color1);
extern void ilitek_draw_bitmap(idim_t x, idim_t x_size, idim_t y, idim_t y_size, uint8_t const * bitmap, icolor_t color0, icolor_t color1);
extern void ilitek_draw_mask_P(idim_t x, idim_t x_size, idim_t y, idim_t y_size, uint8_t const * mask, icolor_t color);
extern void ilitek_draw_mask(idim_t x, idim_t x_size, idim_t y, idim_t y_size, uint8_t const * mask, icolor_t color);
extern void ilitek_init(void);

#endif /* _ILITEK_FASTIO_H_ */
