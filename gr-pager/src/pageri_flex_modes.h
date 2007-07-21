/*
 * Copyright 2006,2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_PAGERI_FLEX_MODES_H
#define INCLUDED_PAGERI_FLEX_MODES_H

#include <gr_types.h>

#define FLEX_SYNC_MARKER 0xA6C6AAAA

typedef struct flex_mode
{
    gr_int32     sync;          // Outer synchronization code
    unsigned int baud;          // Baudrate of SYNC2 and DATA
    unsigned int levels;        // FSK encoding of SYNC2 and DATA
}
flex_mode_t;

extern const flex_mode_t flex_modes[];
extern const char *flex_page_desc[];
extern const int num_flex_modes;
int find_flex_mode(gr_int32 sync_code);
extern unsigned char flex_bcd[];

typedef enum {
    FLEX_SECURE,
    FLEX_UNKNOWN,
    FLEX_TONE,
    FLEX_STANDARD_NUMERIC,
    FLEX_SPECIAL_NUMERIC,
    FLEX_ALPHANUMERIC,
    FLEX_BINARY,
    FLEX_NUMBERED_NUMERIC,
    NUM_FLEX_PAGE_TYPES
}
page_type_t;

inline bool is_alphanumeric_page(page_type_t type)
{
    return (type == FLEX_ALPHANUMERIC ||
	    type == FLEX_SECURE);
}

inline bool is_numeric_page(page_type_t type)
{
    return (type == FLEX_STANDARD_NUMERIC ||
            type == FLEX_SPECIAL_NUMERIC  ||
            type == FLEX_NUMBERED_NUMERIC);
}

inline bool is_tone_page(page_type_t type)
{
    return (type == FLEX_TONE);
}

#endif // INCLUDED_PAGERI_FLEX_MODES_H
