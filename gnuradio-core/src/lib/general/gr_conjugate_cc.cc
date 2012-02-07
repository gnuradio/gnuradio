/* -*- c++ -*- */
/*
 * Copyright 2004,2010 Free Software Foundation, Inc.
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

// WARNING: this file is machine generated.  Edits will be over written

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_conjugate_cc.h>
#include <gr_io_signature.h>
#include <volk/volk.h>

gr_conjugate_cc_sptr
gr_make_conjugate_cc ()
{
  return gnuradio::get_initial_sptr(new gr_conjugate_cc ());
}

gr_conjugate_cc::gr_conjugate_cc ()
  : gr_sync_block ("conjugate_cc",
		   gr_make_io_signature (1, 1, sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, sizeof (gr_complex)))
{
  const int alignment_multiple = 
    volk_get_alignment() / sizeof(gr_complex);
  set_alignment(alignment_multiple);
}

int
gr_conjugate_cc::work (int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  gr_complex *iptr = (gr_complex *) input_items[0];
  gr_complex *optr = (gr_complex *) output_items[0];

  if(is_unaligned()) {
    volk_32fc_conjugate_32fc_u(optr, iptr, noutput_items);
  }
  else {
    volk_32fc_conjugate_32fc_a(optr, iptr, noutput_items);
  }

  return noutput_items;
}
