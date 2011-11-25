/***************************************************************************
 *   Copyright (C) 2010 by Pierre Marchand   *
 *   pierre@oep-h.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef POTRACE_UTIL_H
#define POTRACE_UTIL_H

#include "potracelib.h"

#define SET_BIT(port, bit)   ((port) |= (1 << (bit)))
#define CLEAR_BIT(port, bit) ((port) &= ~(1 << (bit)))
#define IS_BIT_SET(port, bit) (((port) & (1 << (bit))) ? 1 : 0)
#define IS_BIT_CLEAR(port, bit) (((port) & (1 << (bit))) == 0 ? 1 : 0)

#define bitcount(data) (sizeof(data)*8)
#define potrace_N (bitcount(potrace_word))

#define POTRACE_MAP_PIXEL(bitmap, i, j) \
IS_BIT_SET((bitmap->map + j * bitmap->dy)[i / potrace_N], \
	   (potrace_N - 1 - i % potrace_N))

#define SET_POTRACE_MAP_PIXEL(bitmap, i, j)	    \
		SET_BIT((bitmap->map + j * bitmap->dy)[i / potrace_N], \
			(potrace_N - 1 - i % potrace_N))

#define CLEAR_POTRACE_MAP_PIXEL(bitmap, i, j)	    \
		CLEAR_BIT((bitmap->map + j * bitmap->dy)[i / potrace_N],    \
			  (potrace_N - 1 - i % potrace_N))


void potrace_set_pixel(potrace_bitmap_t *bitmap,
				       unsigned short i, unsigned short j) {
	SET_POTRACE_MAP_PIXEL(bitmap, i, j);
}

void potrace_clear_pixel(potrace_bitmap_t *bitmap,
			 unsigned short i, unsigned short j) {
	CLEAR_POTRACE_MAP_PIXEL(bitmap, i, j);
}

unsigned char potrace_get_pixel(potrace_bitmap_t *bitmap,
				unsigned short i, unsigned short j) {
	return POTRACE_MAP_PIXEL(bitmap, i, j) ? 1 : 0;
}


#endif // POTRACE_UTIL_H
