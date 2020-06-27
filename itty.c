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

#include <avr/pgmspace.h>
#include <stdint.h>

#include "itty.h"
#include "ilitek_fastio.h"

void itty_draw_canvas(idim_t x, idim_t y, itty_canvas_s const * ic, itty_canvas_data_s * icd)
{
    /* fill the background */
    ilitek_draw_rect(x, pgm_read_byte(&ic->x_size),
                     y, pgm_read_byte(&ic->y_size),
                     icd->bg_color);
}

/*
 * Draw a border around the outside of the specified canvas.
 */
void itty_draw_border(idim_t x, idim_t y, itty_canvas_s const * ic, itty_canvas_data_s * icd)
{
    icolor_t * border_color;
    irect_s border_rect;
    idim_t border_size;

    border_size = pgm_read_byte(&ic->border_size);

    border_rect.x = x;
    border_rect.x_size = pgm_read_byte(&ic->x_size);
    border_rect.y = y;
    border_rect.y_size = pgm_read_byte(&ic->y_size);

    border_color = icd->border_color;

    while (border_size--) {
        border_rect.x--;
        border_rect.y--;
        border_rect.x_size += 2;
        border_rect.y_size += 2;
        ilitek_draw_box(border_rect, *border_color++);
    }
}

void itty_draw_window(itty_window_s * iw)
{
    itty_draw_context_s draw_context;

    /* only draw if required */
    if (iw->dirty) {
        itty_draw_border(iw->x, iw->y, iw->ic, iw->icd);
        itty_draw_canvas(iw->x, iw->y, iw->ic, iw->icd);
        iw->dirty = 0;
    }

    draw_context.x = iw->x;
    draw_context.y = iw->y;
    draw_context.flash = &iw->ic->flash[0];
    draw_context.sram = &iw->icd->sram[0];

    for (;;) {
        itty_draw_f draw_f;

        draw_f = (itty_draw_f) pgm_read_word(draw_context.flash++);

        if (draw_f) draw_f(&draw_context);
        else        break;
    }
}

/*
 * Draw a one pixel wide line.
 */
void itty_draw_line(ipoint_s p0, ipoint_s p1, icolor_t color)
{
    uint8_t x_len;
    uint8_t y_len;
    uint8_t pixels;
    uint16_t x_acc;
    uint16_t y_acc;
    int16_t x_inc;
    int16_t y_inc;

    if (p0.x <= p1.x) x_len = p1.x - p0.x;
    else              x_len = p0.x - p1.x;

    if (p0.y <= p1.y) y_len = p1.y - p0.y;
    else              y_len = p0.y - p1.y;

    x_acc = p0.x << 8;
    y_acc = p0.y << 8;

    if (x_len >= y_len) {
        pixels = x_len;
        y_inc = ((uint16_t) y_len << 8) / (uint16_t) x_len;
        y_acc += y_inc >> 1;
        x_inc = 0x0100;
    }
    else {
        pixels = y_len;
        x_inc = ((uint16_t) x_len << 8) / (uint16_t) y_len;
        x_acc += x_inc >> 1;
        y_inc = 0x0100;
    }

    if (p0.x > p1.x) x_inc = -x_inc;
    if (p0.y > p1.y) y_inc = -y_inc;

    do {
        ilitek_draw_pixel(x_acc >> 8, y_acc >> 8, color);
        x_acc += x_inc;
        y_acc += y_inc;
    } while (pixels--);
}
