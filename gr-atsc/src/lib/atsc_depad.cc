/* -*- c++ -*- */
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <atsc_depad.h>
#include <gr_io_signature.h>
#include <atsc_types.h>

static const int INTR = ATSC_MPEG_PKT_LENGTH;

atsc_depad_sptr
atsc_make_depad()
{
  return atsc_depad_sptr(new atsc_depad());
}

atsc_depad::atsc_depad()
  : gr_sync_interpolator("atsc_depad",
		  gr_make_io_signature(1, 1, sizeof(atsc_mpeg_packet)),
		  gr_make_io_signature(1, 1, sizeof(unsigned char)),
		  INTR)
{
  reset();
}

void
atsc_depad::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size();
  for (unsigned i = 0; i < ninputs; i++) 
    ninput_items_required[i] = noutput_items / ATSC_MPEG_PKT_LENGTH;
}


int
atsc_depad::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  const atsc_mpeg_packet *in = (const atsc_mpeg_packet *) input_items[0];
  unsigned char *out = (unsigned char *) output_items[0];

  // size with padding (256)
  unsigned int ATSC_MPEG_PKT = sizeof(atsc_mpeg_packet);
  unsigned int i;

  for (i = 0; i < noutput_items/ATSC_MPEG_PKT + 1; i++){
    for (int j = 0; j < ATSC_MPEG_PKT_LENGTH; j++)
	out[i * ATSC_MPEG_PKT_LENGTH + j] = in[i * ATSC_MPEG_PKT].data[j];

  }

  return i * ATSC_MPEG_PKT_LENGTH;
}




