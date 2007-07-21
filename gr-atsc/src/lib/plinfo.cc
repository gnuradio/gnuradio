/* -*- c++ -*- */
/*
 * Copyright 2002,2006 Free Software Foundation, Inc.
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

#include <atsc_types.h>
#include <assert.h>

void
plinfo::delay (plinfo &out, const plinfo &in, int nsegs_of_delay)
{
  assert (in.regular_seg_p ());
  assert (nsegs_of_delay >= 0);
  
  int	s = in.segno ();
  if (in.in_field2_p ())
    s += ATSC_DSEGS_PER_FIELD;

  s -= nsegs_of_delay;
  if (s < 0)
    s += 2 * ATSC_DSEGS_PER_FIELD;

  assert (0 <= s && s < 2 * ATSC_DSEGS_PER_FIELD);

  if (s < ATSC_DSEGS_PER_FIELD)
    out.set_regular_seg (false, s);				// field 1
  else
    out.set_regular_seg (true, s - ATSC_DSEGS_PER_FIELD);	// field 2
}

void
plinfo::sanity_check (const plinfo &x)
{
  // basic sanity checks...
  assert (x.segno () >= 0);
  assert (x.segno () < (unsigned) ATSC_DSEGS_PER_FIELD);
  assert ((x.flags () & ~0x3f) == 0);

  assert (x.regular_seg_p () ^ x.field_sync_p ());
  assert ((x.segno () != 0) ^ x.first_regular_seg_p ());
}

