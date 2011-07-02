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

#include <gr_max_ss.h>
#include <gr_io_signature.h>

gr_max_ss_sptr
gr_make_max_ss ( size_t vlen )
{
	return gr_max_ss_sptr ( new gr_max_ss(vlen));
}

gr_max_ss::gr_max_ss( size_t vlen)
	: gr_sync_block ( "max_ss",
                   gr_make_io_signature (1, -1, vlen*sizeof (short)),
                   gr_make_io_signature (1, 1, sizeof (short))),
		   d_vlen(vlen)
{
}

int
gr_max_ss::work( int noutput_items,
	gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items)
{
	short *optr = (short *) output_items[0];

	int ninputs = input_items.size ();
	
	for (int i=0; i<noutput_items; i++) {

	        short max = ((short *) input_items[0])[i*d_vlen];
		
		for (int j=0; j < (int) d_vlen; j++ ) {
			for (int k=0; k<ninputs; k++) {
				if ( ((short *) input_items[k])[i*d_vlen + j] > max) {
					max = ((short*) input_items[k])[i*d_vlen + j];
				}
			}
		}

		*optr++ = (short) max;
	}
	return noutput_items;
}

