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
#ifndef _ITTY_H_
#define _ITTY_H_

/*
 * Located in FLASH.
 */
typedef struct itty_canvas {
    idim_t x_size;
    idim_t y_size;
    idim_t border_size;
    void const * flash[];
} itty_canvas_s;

/*
 * Located in RAM.
 */
typedef struct itty_canvas_data {
    icolor_t bg_color;
    icolor_t * border_color;
    void * sram[];
} itty_canvas_data_s;

typedef struct itty_window {
    uint8_t dirty;
    idim_t x;
    idim_t y;
    itty_canvas_s const * ic;
    itty_canvas_data_s * icd;
} itty_window_s;

typedef struct itty_draw_context {
    idim_t x;
    idim_t y;
    void const * const * flash;
    void ** sram;
} itty_draw_context_s;

typedef void (* itty_draw_f)(itty_draw_context_s * draw_context);

extern void itty_draw_canvas(idim_t x, idim_t y, itty_canvas_s const * ic, itty_canvas_data_s * id);
extern void itty_draw_border(idim_t x, idim_t y, itty_canvas_s const * ic, itty_canvas_data_s * id);
extern void itty_draw_window(itty_window_s * iw);
extern void itty_draw_line(ipoint_s p0, ipoint_s p1, icolor_t color);

#endif /* _ITTY_H_ */
