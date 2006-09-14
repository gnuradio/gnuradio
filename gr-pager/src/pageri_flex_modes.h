/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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
    bool         phase_a;       // PHASEA is transmitted
    bool         phase_b;       // PHASEB is transmitted
    bool         phase_c;       // PHASEC is transmitted
    bool         phase_d;       // PHASED is transmitted
    int          phases;        // number of phases transmitted
}
flex_mode_t;

extern const flex_mode_t flex_modes[];
extern const int num_flex_modes;

#endif // INCLUDED_PAGERI_FLEX_MODES_H
