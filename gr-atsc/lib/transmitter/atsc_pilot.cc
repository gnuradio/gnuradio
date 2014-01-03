/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#include <gnuradio/atsc/pilot.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/atsc/consts.h>

atsc_pilot_sptr
atsc_make_pilot()
{
	return gnuradio::get_initial_sptr(new atsc_pilot());
}

atsc_pilot::atsc_pilot()
  : gr::block("atsc_seg_sync_mux",
                   gr::io_signature::make(1, 1, sizeof(atsc_soft_data_segment)),
                   gr::io_signature::make(1, 1, sizeof(float)))
{
	set_output_multiple(ATSC_DATA_SEGMENT_LENGTH); // data + seg sync
	reset();
}

int
atsc_pilot::general_work (int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
	const atsc_soft_data_segment *in = (const atsc_soft_data_segment *) input_items[0];
	float *out = (float *) output_items[0];

	assert (noutput_items % (ATSC_DATA_SEGMENT_LENGTH) == 0);
	int ni = noutput_items / (ATSC_DATA_SEGMENT_LENGTH);

	for (int i = 0; i < ni; i++)
		for( int j = 0; j < ATSC_DATA_SEGMENT_LENGTH; j++ ) // Add the pilot
			out[(i * (ATSC_DATA_SEGMENT_LENGTH)) + j] = in[i].data[j] + 1.25;

	consume_each(ni);

	return noutput_items;
}
