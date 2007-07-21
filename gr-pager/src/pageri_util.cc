/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pageri_util.h>

unsigned char pageri_reverse_bits8(unsigned char val)
{
    // This method was attributed to Rich Schroeppel in the Programming 
    // Hacks section of Beeler, M., Gosper, R. W., and Schroeppel, R. 
    // HAKMEM. MIT AI Memo 239, Feb. 29, 1972.
    //
    // Reverses 8 bits in 5 machine operations with 64 bit arch
    return (val * 0x0202020202ULL & 0x010884422010ULL) % 1023;
}

gr_int32 pageri_reverse_bits32(gr_int32 val)
{
    gr_int32 out = 0x00000000;
    out |= (pageri_reverse_bits8((val >> 24) & 0x000000FF)      );
    out |= (pageri_reverse_bits8((val >> 16) & 0x000000FF) <<  8);
    out |= (pageri_reverse_bits8((val >>  8) & 0x000000FF) << 16);
    out |= (pageri_reverse_bits8((val      ) & 0x000000FF) << 24);
    return out;
}
