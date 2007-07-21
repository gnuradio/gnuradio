/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
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

#include <sdr_1000.h>
#include <ppio.h>

sdr_1000_base::sdr_1000_base (int which_pp)
{
  d_ppio = make_ppio (which_pp);
  d_shadow[0] = 0;
  d_shadow[1] = 0;
  d_shadow[2] = 0;
  d_shadow[3] = 0;
  reset ();
}

sdr_1000_base::~sdr_1000_base ()
{
}

void
sdr_1000_base::reset ()
{
  d_ppio->lock ();
  d_ppio->write_control (0x0F);
  d_ppio->unlock ();
  write_latch (L_EXT,  0x00, 0xff);
  write_latch (L_BAND, 0x00, 0xff);
  write_latch (L_DDS0, 0x80, 0xff);	// hold DDS in reset
  write_latch (L_DDS1, 0x00, 0xff);
}

  
void
sdr_1000_base::write_latch (int which, int value, int mask)
{
  if (!(0 <= which && which <= 3))
    return;
  
  d_ppio->lock ();
  d_shadow[which] = (d_shadow[which] & ~mask) | (value & mask);
  d_ppio->write_data (d_shadow[which]);
  d_ppio->write_control (0x0F ^ (1 << which));
  d_ppio->write_control (0x0F);
  d_ppio->unlock ();
}
