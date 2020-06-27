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
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include <stdint.h>

#include "bits.h"
#include "pinmap.h"
#include "timer.h"
#include "lcd.h"
#include "mathops.h"

#include "font_profont15r.h"

#include "itty.h"
#include "ilitek_fastio.h"
#include "ssd.h"

extern int8_t encoder_accumulator;
uint16_t ummd(uint16_t multiplier, uint16_t multiplicand, uint16_t divisor) __attribute__((__noinline__));
uint16_t ummd(uint16_t multiplier, uint16_t multiplicand, uint16_t divisor)
{
    return _ummd(multiplier, multiplicand, divisor);
}


/*
 * Draw a horizontal bar graph.  THIS SHOULD USE AN OBJECT
 *  x, x_size, y, y_size
 *  color0, color1
 *  position, scale
 */

typedef struct lbar {
    uint8_t state;
    irect_s rect;

    int16_t lo_limit;
    int16_t position;
    int16_t hi_limit;

    icolor_t color0;
    icolor_t color1;
} lbar_s;

typedef union lobj {
    uint8_t state;
    lbar_s bargraph;
} lobj_u;

#define LOBJ_REDRAW (0x80)
#define LOBJ_TYPE_MASK (0x0f)
#define LOBJ_TYPE_HBAR (0x00)
#define LOBJ_TYPE_VBAR (0x01)

void lcd_redraw_hbar(lobj_u * lo)
{
    lbar_s * lb = &lo->bargraph;
    ldim_t position;

    /* scale position to pixels */
    position = ummd(lb->rect.x_size,
                    lb->position - lb->lo_limit, lb->hi_limit - lb->lo_limit);

    /* draw the left portion */
    ilitek_draw_rect(lb->rect.x, position,
                       lb->rect.y, lb->rect.y_size,
                       lb->color0);

    /* draw the right portion */
    ilitek_draw_rect(lb->rect.x + position, lb->rect.x_size - position,
                       lb->rect.y, lb->rect.y_size,
                       lb->color1);
}

void lcd_redraw_vbar(lobj_u * lo)
{
    lbar_s * lb = &lo->bargraph;
    ldim_t position;

    /* scale position to pixels */
    position = ummd(lb->rect.y_size,
                    lb->position - lb->lo_limit, lb->hi_limit - lb->lo_limit);

    /* draw the top portion */
    ilitek_draw_rect(lb->rect.x, lb->rect.x_size,
                       lb->rect.y, position,
                       lb->color0);

    /* draw the bottom portion */
    ilitek_draw_rect(lb->rect.x, lb->rect.x_size,
                       lb->rect.y + position, lb->rect.y_size - position,
                       lb->color1);
}

void lcd_redraw(lobj_u * lo)
{
    if (LOBJ_REDRAW & lo->state) {
        switch (LOBJ_TYPE_MASK & lo->state) {
        case 0:
            lcd_redraw_hbar(lo);
            break;
        case 1:
            lcd_redraw_vbar(lo);
            break;
        }
        lo->state &= ~LOBJ_REDRAW;
    }
}

int16_t lcd_update_bar(lobj_u * lo, int16_t parameter)
{
    lbar_s * lb = &lo->bargraph;

    parameter = lb->position + parameter;

    parameter = limit_range(lb->lo_limit, parameter, lb->hi_limit);

    if (lb->position != parameter) {
        lb->position = parameter;
        lb->state |= LOBJ_REDRAW;
    }

    return parameter;
}

int16_t lcd_update(lobj_u * lo, int16_t parameter)
{
    switch (LOBJ_TYPE_MASK & lo->state) {
    case 0:
    case 1:
        return lcd_update_bar(lo, parameter);
        break;
    }

    return parameter;
}

void lcd_effect(icolor_t color)
{
    uint8_t const * glyph;
    int glyph_size;
    char c;
    ltypeB_s lw;

    lw.x_size = 8;
    lw.y_size = 16;
    glyph_size = ((lw.x_size * lw.y_size) + 7) / 8;

    c = ' ';
    for (lw.y = 0; lw.y <= Y_SIZE - lw.y_size; lw.y += lw.y_size) {

        for (lw.x = 0; lw.x <= X_SIZE - lw.x_size; lw.x += lw.x_size) {
            glyph = u8g_font_profont15r + ((c - ' ') * glyph_size);
            ilitek_draw_bitmap_P(lw.x, lw.x_size, lw.y, lw.y_size, glyph, 0x0000, color);

            if (c == 126) break;
            c++;
        }

        if (c == 126) break;
    }

    lw.x = 0;
    lw.y = 0;

    lw.x_size = 15;
    lw.y_size = 27;

#if 0
    ilitek_draw_rect(lw.x, lw.x_size + 4, lw.y, lw.y_size + 4, 0x0000);
    digit7_draw_digit(lw.x + 1, lw.y + 1, 0, color);


    ilitek_draw_rect(lw.x + 17, 3, lw.y + 26, 3, color);

    lw.x += 20;
    ilitek_draw_rect(lw.x, lw.x_size + 4, lw.y, lw.y_size + 4, 0x0000);
    digit7_draw_digit(lw.x + 1, lw.y + 1, 1, color);

    lw.x += 20;
    ilitek_draw_rect(lw.x, lw.x_size + 4, lw.y, lw.y_size + 4, 0x0000);
    digit7_draw_digit(lw.x + 1, lw.y + 1, 2, color);

    lw.x += 20;
    ilitek_draw_rect(lw.x, lw.x_size + 4, lw.y, lw.y_size + 4, 0x0000);
    digit7_draw_digit(lw.x + 1, lw.y + 1, 3, color);

    lw.x += 20;
    ilitek_draw_rect(lw.x, lw.x_size + 4, lw.y, lw.y_size + 4, 0x0000);
    digit7_draw_digit(lw.x + 1, lw.y + 1, 4, color);

    lw.x = 0;
    lw.y += 29;

    digit7_draw_digit(lw.x + 1, lw.y + 1, 5, color);

    lw.x += 20;
    digit7_draw_digit(lw.x + 1, lw.y + 1, 6, color);

    lw.x += 20;
    digit7_draw_digit(lw.x + 1, lw.y + 1, 7, color);

    lw.x += 20;
    digit7_draw_digit(lw.x + 1, lw.y + 1, 8, color);

    lw.x += 20;
    digit7_draw_digit(lw.x + 1, lw.y + 1, 9, color);

    lw.x = 11;
    lw.y = 71;

    digit7_draw_digit(lw.x + 1, lw.y + 1, 8, color);

    lw.x += 20;
    digit7_draw_digit(lw.x + 1, lw.y + 1, 0, color);

    lw.x += 22;
    digit7_draw_digit(lw.x + 1, lw.y + 1, 5, color);

    lw.x += 20;
    digit7_draw_digit(lw.x + 1, lw.y + 1, 1, color);
#endif
}

#define GREY(x) (                                                      \
    (((x) << 8) & 0xf800) |                                            \
    (((x) << 3) & 0x07e0) |                                            \
    (((x) >> 3) & 0x001f)                                              \
)

lobj_u enc1 = {
    .bargraph = {
        .state = LOBJ_TYPE_HBAR | LOBJ_REDRAW,
        .rect = { .x = 10, .x_size = 108, .y = 110, .y_size = 10 },
        .lo_limit = 0,
        .position = 0,
        .hi_limit = 500,
        .color0 = GREY(192),
        .color1 = GREY(64),
    },
};


lobj_u enc2 = {
    .bargraph = {
        .state = LOBJ_TYPE_VBAR | LOBJ_REDRAW,
        .rect = { .x = 10, .x_size = 32, .y = 10, .y_size = 64 },
        .lo_limit = 0,
        .position = 0,
        .hi_limit = 500,
        .color0 = GREY(255),
        .color1 = GREY(64),
    },
};

#if 0

digit7_display_s my_display = {
    .rect = {
        .x = 10,
        .y = 10,
        .x_size = 100,
        .y_size = 60,
    },
    .places = 5,
    .digits = {
        [0] = { 0, 0, 17 },
        [1] = { 20, 0, 9 },
        [2] = { 40, 0, 8 },
        [3] = { 60, 0, 7 },
        [4] = { 80, 0, 6 },
    },
};

digit7_display_s my_clock = {
    .rect = {
        .x = 5,
        .y = 5,
        .x_size = 100,
        .y_size = 37,
    },
    .places = 5,
    .digits = {
        [0] = { 0, 0, 2 },
        [1] = { 20, 0, 3 },
        [2] = { 37, 0, 19 },
        [3] = { 42, 0, 0 },
        [4] = { 62, 0, 0 },
    },
};
#endif

icolor_t bg_bcolors[] = { 0x4208, 0x630c, 0x0000 };
icolor_t bcolors[] = { 0x8410, 0xc618, 0x0000 };

static const itty_canvas_s ic __ATTR_PROGMEM__ = {
    .x_size = 79,
    .y_size = 31,
    .border_size = ARRAY_SIZE(bcolors),
    .flash = {
        0, 0,
    },
};

itty_canvas_data_s id1 = {
    .bg_color = 0xffff,
    .border_color = bg_bcolors,
};

itty_canvas_data_s id2 = {
    .bg_color = 0x8410,
    .border_color = bg_bcolors,
};

itty_canvas_data_s id3 = {
    .bg_color = 0xffff,
    .border_color = bg_bcolors,
};

itty_canvas_data_s id4 = {
    .bg_color = 0x0007,
    .border_color = bcolors,
};


#define WORD_VALUE(a) ((uint16_t) (a) & 0xff), ((uint16_t) (a) >> 8)


static const itty_draw_f draw_function __ATTR_PROGMEM__ = ssd_draw_display;


const ssd_display_s my_display0 __ATTR_PROGMEM__ = {
    .places = 4,
    .digit = {
        { .x = 2,  .y = 2, .type = SSD_NUMERIC },
        { .x = 22, .y = 2, .type = SSD_NUMERIC },
        { .x = 42, .y = 2, .type = SSD_NUMERIC },
        { .x = 62, .y = 2, .type = SSD_NUMERIC },
    },
};

const ssd_display_s my_display1 __ATTR_PROGMEM__ = {
    .places = 4,
    .digit = {
        { .x = 2,  .y = 33, .type = SSD_NUMERIC },
        { .x = 22, .y = 33, .type = SSD_NUMERIC },
        { .x = 42, .y = 33, .type = SSD_NUMERIC },
        { .x = 62, .y = 33, .type = SSD_NUMERIC },
    },
};


ssd_display_data_s my_display_data0 = {
    .dirty = 1,
    .on_color = 0x001f,
    .off_color = 0x0000,
    .digit_id = {
        SSD_DIGIT_0,
        SSD_DIGIT_1,
        SSD_DIGIT_2,
        SSD_DIGIT_3,
    },
};

ssd_display_data_s my_display_data1 = {
    .dirty = 1,
    .on_color = 0x07e0,
    .off_color = 0x0000,
    .digit_id = {
        SSD_DIGIT_4,
        SSD_DIGIT_5,
        SSD_DIGIT_6,
        SSD_DIGIT_7,
    },
};

static const itty_canvas_s my_canvas __ATTR_PROGMEM__ = {
    .x_size = 79,
    .y_size = 62,
    .border_size = ARRAY_SIZE(bcolors),
    .flash = {
        ssd_draw_display,
        &my_display0,
        ssd_draw_display,
        &my_display1,
        0
    },
};

static itty_canvas_data_s my_canvas_data = {
    .bg_color = 0x0000,
    .border_color = bcolors,
    .sram = {
        &my_display_data0,
        &my_display_data1,
    },
};

itty_window_s my_window = {
    .dirty = 1,
    .x = 20,
    .y = 44,
    .ic = &my_canvas,
    .icd = &my_canvas_data,
};


#if 0
itty_digit_s const meter_digits[] __ATTR_PROGMEM__ = {
    { .x = 2,  .y = 2, .mask = D7_NUMBER_AND_DP },
    { .x = 22, .y = 2, .mask = D7_NUMBER_AND_DP },
    { .x = 42, .y = 2, .mask = D7_NUMBER_AND_DP },
    { .x = 62, .y = 2, .mask = D7_NUMBER_AND_DP },
};

itty_digit_s const clock_digits[] __ATTR_PROGMEM__ = {
    { .x = 2,  .y = 2, .mask = D7_NUMBER },
    { .x = 22, .y = 2, .mask = D7_NUMBER },
    { .x = 39, .y = 2, .mask = D7_COLON },
    { .x = 44, .y = 2, .mask = D7_NUMBER },
    { .x = 64, .y = 2, .mask = D7_NUMBER },
};

itty_digit_s idisplay = {
    .places = 3,
    .on_color = 0xffff;
    .off_color = 0;
    .digits = {
        [0] = { .x = 2,  .y = 2 },
        [1] = { .x = 22, .y = 2 },
        [2] = { .x = 42, .y = 2 },
        [3] = { .x = 62, .y = 2 },
    },
};

itty_digit_data_s idd = {
    .on_color = 0xffff;
    .off_color = 0;
    .digits = {
        [0] = 0,
        [1] = 0,
        [2] = 0,
        [3] = 0,
    },
};
#endif


itty_window_s iw1 = {
    .x = 20,
    .y = 20,
    .ic = &ic,
    .icd = &id1,
};

itty_window_s iw2 = {
    .x = 10,
    .y = 40,
    .ic = &ic,
    .icd = &id2,
};

itty_window_s iw3 = {
    .x = 30,
    .y = 60,
    .ic = &ic,
    .icd = &id3,
};

itty_window_s iw4 = {
    .x = 40,
    .y = 30,
    .ic = &ic,
    .icd = &id4,
};

void lcd_init(void)
{
    ilitek_init();

    for (;;) {
//        irect_s lr1 = { 32, 65, 32, 65 };
        irect_s lr1 = { 10, 80, 70, 31 };
        icolor_t colors[] = { GREY(128), GREY(192), GREY(128) };
        ipoint_s p0 = { 64, 64 };
        ipoint_s p1 = { 96, 96 };

       //lcd_draw_border(lr1, colors, ARRAY_SIZE(colors));

        timer_delay(TBTICKS_FROM_MS(500));
        lcd_effect(0xf800);
        timer_delay(TBTICKS_FROM_MS(500));
        lcd_effect(0x07e0);
        timer_delay(TBTICKS_FROM_MS(500));
        lcd_effect(0x001f);

    ilitek_fill_root_window(0x0000);
    itty_draw_window(&iw1);
    itty_draw_window(&iw2);
    itty_draw_window(&iw3);
    itty_draw_window(&iw4);

    itty_draw_window(&my_window);

    return;

#if 0

    ssd_draw_display(iw4.x, iw4.y, &my_display, &my_display_data);
//    digit7_draw_digit_P(iw4.x, iw4.y, 0xf800, 7);
    itty_draw_segment_P(iw4.x, iw4.y, 0x001f, 0);
    itty_draw_segment_P(iw4.x, iw4.y, 0x001f, 1);
    itty_draw_segment_P(iw4.x, iw4.y, 0x001f, 2);
    itty_draw_segment_P(iw4.x, iw4.y, 0x001f, 3);
    itty_draw_segment_P(iw4.x, iw4.y, 0x001f, 4);
    itty_draw_segment_P(iw4.x, iw4.y, 0x001f, 5);
    itty_draw_segment_P(iw4.x, iw4.y, 0x001f, 6);

    itty_draw_canvas(20, 20, &ic);
    itty_draw_border(20, 20, &ic);

    itty_draw_canvas(10, 40, &ic);
    itty_draw_border(10, 40, &ic);

    itty_draw_canvas(30, 60, &ic);
    itty_draw_border(30, 60, &ic);

    itty_draw_canvas(60, 30, &ic);
    itty_draw_border(60, 30, &ic);

    digit7_draw_display_P(0, 0, &my_display, 1);
    digit7_draw_display_P(0, 30, &my_display, 1);

    lcd_draw_border(my_display.rect, colors, ARRAY_SIZE(colors));
    digit7_draw_display_P(20, 60, &my_clock, 1);
    digit7_draw_digit_P(10, 10, 9, 0x0000);
    digit7_draw_digit_P(30, 10, 8, 0x0000);
    digit7_draw_digit_P(50, 10, 7, 0x0000);
    digit7_draw_digit_P(70, 10, 6, 0x0000);
    digit7_draw_digit_P(90, 10, 5, 0x0000);
    digit7_draw_digit_P(110, 10, 4, 0x0000);
    digit7_draw_digit(0, 0, 10, 0x0000);
    digit7_draw_digit(20, 0, 1, 0x0000);
    digit7_draw_digit(40, 0, 2, 0x0000);
    digit7_draw_digit(60, 0, 3, 0x0000);
    digit7_draw_digit(80, 0, 4, 0x0000);
    digit7_draw_digit(0, 0, 0, 0x0000);
    digit7_draw_digit(0, 0, 10, 0x0000);
    digit7_draw_digit(20, 0, 1, 0x0000);
    digit7_draw_digit(40, 0, 2, 0x0000);
    digit7_draw_digit(60, 0, 3, 0x0000);
    digit7_draw_digit(80, 0, 4, 0x0000);

    digit7_draw_digit(0, 30, 11, 0x0000);
    digit7_draw_digit(20, 30, 5, 0x0000);
    digit7_draw_digit(40, 30, 6, 0x0000);
    digit7_draw_digit(60, 30, 7, 0x0000);
    digit7_draw_digit(80, 30, 8, 0x0000);
    digit7_draw_digit(100, 30, 9, 0x0000);

    digit7_draw_digit(0, 60, 12, 0x0000);
    digit7_draw_digit(40, 60, 2, 0x0000);
    digit7_draw_digit(60, 60, 3, 0x0000);
    digit7_draw_digit(60, 60, 10, 0x0000);
    digit7_draw_digit(80, 60, 4, 0x0000);

    digit7_draw_digit(20, 90, 0, 0x0000);
    digit7_draw_digit(40, 90, 6, 0x0000);
    digit7_draw_digit(57, 90, 13, 0x0000);
    digit7_draw_digit(62, 90, 3, 0x0000);
    digit7_draw_digit(82, 90, 7, 0x0000);
#endif

        for (;;);

        //lcd_draw_border(lr1, colors, ARRAY_SIZE(colors));

        for (; p1.x > 32; p1.x -=8) itty_draw_line(p0, p1, 0xffff);
        for (; p1.y > 32; p1.y -=8) itty_draw_line(p0, p1, 0xffff);
        for (; p1.x < 96; p1.x +=8) itty_draw_line(p0, p1, 0xffff);
        for (; p1.y < 96; p1.y +=8) itty_draw_line(p0, p1, 0xffff);

        //lcd_draw_border(enc1.bargraph.rect, colors, ARRAY_SIZE(colors));
        //lcd_draw_border(enc2.bargraph.rect, colors, ARRAY_SIZE(colors));

        for (;;) {
            int8_t delta;

            delta = encoder_accumulator;
            encoder_accumulator = 0;

            lcd_redraw(&enc1);
            lcd_redraw(&enc2);

            lcd_update_bar(&enc1, delta);
            lcd_update_bar(&enc2, delta);
        }
    }

    for (;;) {
    lcd_effect(0xffff);

    }
}
