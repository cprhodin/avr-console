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
#ifndef _PROJECT_H_
#define _PROJECT_H_

#ifndef BAUD
#define BAUD (250000L)
#endif

//
// timebase timer, 0, 1 or 2 (only 1 is extensively tested)
//
#define TBTIMER 1
#define TBTIMER_PRESCALER 64UL

//
// timebase counter size in bits, either 16 or 32
//
#define TBSIZE 32

#endif // _PROJECT_H_
