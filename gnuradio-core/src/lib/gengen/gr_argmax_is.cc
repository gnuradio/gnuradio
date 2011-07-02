/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#include <gr_argmax_is.h>
#include <gr_io_signature.h>

gr_argmax_is_sptr
gr_make_argmax_is ( size_t vlen )
{
	return gr_argmax_is_sptr ( new gr_argmax_is(vlen));
}

gr_argmax_is::gr_argmax_is( size_t vlen)
	: gr_sync_block ( "argmax_is",
                   gr_make_io_signature (1, -1, vlen*sizeof (int)),
                   gr_make_io_signature (2, 2, sizeof (short))),
	d_vlen(vlen)
{
}


int
gr_argmax_is::work( int noutput_items,
	gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items)
{

	int ninputs = input_items.size ();

	short *x_optr = (short *) output_items[0];
	short *y_optr = (short *) output_items[1];

	for (int i=0; i<noutput_items; i++) {

		int max = ((int *) input_items[0])[i*d_vlen];
		int x = 0;
		int y = 0;

		for (int j=0; j < (int) d_vlen; j++ ) {
			for (int k=0; k<ninputs; k++) {
				if ( ((int *) input_items[k])[i*d_vlen + j] > max) {
					max = ((int *) input_items[k])[i*d_vlen + j];
					x = j;
					y = k;
				}
			}
		}

		*x_optr++ = (short) x;
		*y_optr++ = (short) y;
	}
	return noutput_items;
}

