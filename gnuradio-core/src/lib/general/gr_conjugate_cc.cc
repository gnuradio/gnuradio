/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

gr_conjugate_cc_sptr
gr_make_conjugate_cc ()
{
  return gr_conjugate_cc_sptr (new gr_conjugate_cc ());
}

gr_conjugate_cc::gr_conjugate_cc ()
  : gr_sync_block ("conjugate_cc",
		   gr_make_io_signature (1, 1, sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, sizeof (gr_complex)))
{
}

int
gr_conjugate_cc::work (int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  gr_complex *iptr = (gr_complex *) input_items[0];
  gr_complex *optr = (gr_complex *) output_items[0];

  int	size = noutput_items;

  while (size >= 8){
    optr[0] = conj(iptr[0]);
    optr[1] = conj(iptr[1]);
    optr[2] = conj(iptr[2]);
    optr[3] = conj(iptr[3]);
    optr[4] = conj(iptr[4]);
    optr[5] = conj(iptr[5]);
    optr[6] = conj(iptr[6]);
    optr[7] = conj(iptr[7]);
    size -= 8;
    optr += 8;
    iptr += 8;
  }

  while (size-- > 0) {
    *optr = conj(*iptr);
    iptr++;
    optr++;
  }

  return noutput_items;
}
