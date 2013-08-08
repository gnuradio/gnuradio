/* -*- c++ -*- */
/*
 * Copyright 2006,2008,2010 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <gnuradio/atsc/depad.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/atsc/types.h>

atsc_depad_sptr
atsc_make_depad()
{
  return gnuradio::get_initial_sptr(new atsc_depad());
}

atsc_depad::atsc_depad()
  : gr::sync_interpolator("atsc_depad",
                          gr::io_signature::make(1, 1, sizeof(atsc_mpeg_packet)),
                          gr::io_signature::make(1, 1, sizeof(unsigned char)),
                          ATSC_MPEG_PKT_LENGTH)
{
  reset();
}

int
atsc_depad::work (int noutput_items,
		  gr_vector_const_void_star &input_items,
		  gr_vector_void_star &output_items)
{
  const atsc_mpeg_packet *in = (const atsc_mpeg_packet *) input_items[0];
  unsigned char *out = (unsigned char *) output_items[0];

  int i;

  for (i = 0; i < noutput_items/ATSC_MPEG_PKT_LENGTH; i++){
    memcpy(&out[i * ATSC_MPEG_PKT_LENGTH], in[i].data, ATSC_MPEG_PKT_LENGTH);
  }

  return i * ATSC_MPEG_PKT_LENGTH;
}
