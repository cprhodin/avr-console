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
#ifndef _LCD_H_
#define _LCD_H_

typedef uint16_t icolor_t;
typedef uint32_t icount_t;
typedef uint8_t lcoor_t;
typedef uint8_t ldim_t;

//typedef struct lpoint {
//    lcoor_t x;
//    lcoor_t y;
//} lpoint_s;

typedef struct lrect {
    lcoor_t x;
    ldim_t x_size;
    lcoor_t y;
    ldim_t y_size;
} lrect_s;

typedef struct lregion {
    icolor_t color;
    lcoor_t x;
    lcoor_t y;
    ldim_t x_size;
    ldim_t y_size;
} lregion_s;

typedef struct ltypeA {
    lcoor_t x0;
    lcoor_t y0;
    lcoor_t x1;
    lcoor_t y1;
} ltypeA_s;

typedef struct ltypeB {
    lcoor_t x;
    lcoor_t y;
    ldim_t x_size;
    ldim_t y_size;
} ltypeB_s;

typedef struct ltypeC {
    lcoor_t x;
    lcoor_t y;
    ldim_t x_size;
    ldim_t y_size;
    icolor_t color;
} ltypeC_s;

typedef struct ltypeD {
    lcoor_t x;
    lcoor_t y;
    ldim_t x_size;
    ldim_t y_size;
    icolor_t color0;
    icolor_t color1;
} ltypeD_s;

typedef uint8_t lsize_t;
typedef uint8_t llen_t;

#define lcd_window_s irect_s

//typedef struct lcd_window {
//    uint8_t x;
//    uint8_t x_size;
//    uint8_t y;
//    uint8_t y_size;
//} lcd_window_s;

extern void lcd_init(void);
extern void lcd_reset(void);
extern void lcd_command(uint8_t command);
extern void lcd_data(uint8_t data);

#endif /* _LCD_H_ */
