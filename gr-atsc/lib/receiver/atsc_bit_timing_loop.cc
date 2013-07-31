/* -*- c++ -*- */
/*
 * Copyright 2006,2010 Free Software Foundation, Inc.
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

#include <gnuradio/atsc/bit_timing_loop.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/atsc/consts.h>
#include <string.h>
#include <boost/math/special_functions/sign.hpp>
#include <iomanip>

// Input rate changed from 20MHz to 19.2 to support usrp at 3 * 6.4MHz
float input_rate = 19.2e6;
double ratio_of_rx_clock_to_symbol_freq = input_rate / ATSC_SYMBOL_RATE;

atsc_bit_timing_loop_sptr
atsc_make_bit_timing_loop()
{
	return gnuradio::get_initial_sptr(new atsc_bit_timing_loop());
}

atsc_bit_timing_loop::atsc_bit_timing_loop()
	: gr::block("atsc_bit_timing_loop",
		gr::io_signature::make(1, 1, sizeof(float)),
		gr::io_signature::make3(2, 3, sizeof(float), sizeof(float), sizeof(float))),
	d_next_input(0), d_rx_clock_to_symbol_freq (ratio_of_rx_clock_to_symbol_freq),
	d_si(0)
{
	d_loop.set_taps( LOOP_FILTER_TAP );
	reset();
}

void
atsc_bit_timing_loop::reset() 
{ 
	d_w = d_rx_clock_to_symbol_freq;
	d_mu = 0.5;

	for (int i = 0; i < ATSC_DATA_SEGMENT_LENGTH; i++)
		sample_mem[i] = 0;

	d_timing_adjust = 0;
	d_counter = 0;
	d_symbol_index = 0;
	d_seg_locked = false;

	d_sr = 0;

	for (int i = 0; i < ATSC_DATA_SEGMENT_LENGTH; i++)
		d_integrator[i] = SSI_MIN;
}

void
atsc_bit_timing_loop::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
	unsigned ninputs = ninput_items_required.size();
	for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = static_cast<int>(noutput_items * d_rx_clock_to_symbol_freq) + 1500 - 1;
}

int
atsc_bit_timing_loop::general_work (int noutput_items,
                                 gr_vector_int &ninput_items,
                                 gr_vector_const_void_star &input_items,
                                 gr_vector_void_star &output_items)
{
	int r = work( noutput_items, input_items, output_items );
	if (r > 0)
		consume_each( d_si );
	return r;
}

int
atsc_bit_timing_loop::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
	const float *in = (const float *) input_items[0];
	float *out_sample = (float *) output_items[0];
	atsc::syminfo *out_tag = (atsc::syminfo *) output_items[1];
	float *out_timing_error = (float *) output_items[2];

	// We are tasked with producing output.size output samples.
	// We will consume approximately 2 * output.size input samples.

	int  k; // output index

	float            interp_sample;
	atsc::syminfo    tag;
	// ammount requested in forecast
	unsigned long input_size = noutput_items * d_rx_clock_to_symbol_freq + 1500 -1;

	memset (&tag, 0, sizeof (tag));

	// ammount actually consumed
	d_si = 0;

	for (k = 0; k < noutput_items; k++)
	{
		if (d_si + (unsigned long)d_interp.ntaps() > input_size)
		{
    			fprintf (stderr, "atsc_bit_timing_loop: ran short on data...\n");
			break;
		}

		// FIXME Confirm that this is right.  I think it is.  It was (1-d_mu)
		interp_sample = d_interp.interpolate ( &in[d_si], d_mu );

		double filter_out = 0;

		filter_out = d_loop.filter (d_timing_adjust);
		d_mu = d_mu + ADJUSTMENT_GAIN * 5e3 * filter_out;

		double s = d_mu + d_w;
		double float_incr = floor (s);
		d_mu = s - float_incr;
		d_incr = (int) float_incr;

		assert (d_incr >= 1 && d_incr <= 3);
		d_si += d_incr;

		sample_mem[d_counter] = interp_sample;

		// Is the sample positive or negative?
		int bit = (interp_sample < 0 ? 0 : 1);

		d_sr = ((bit & 1) << 3) | (d_sr >> 1);
		int corr_out = (d_sr == 0x9);	// 1001

		int weight = corr_out ? +2 : -1;

		int t = d_integrator[d_counter] + weight;
		t = std::max (t, SSI_MIN);
		t = std::min (t, SSI_MAX);
		d_integrator[d_counter] = t;

		int best_correlation_index = -1;

		incr_symbol_index ();
		if (incr_counter ())	// counter just wrapped...
		{
    			int best_correlation_value;

			int best_value = d_integrator[0];
			int best_index = 0;

			for(int i = 1; i < ATSC_DATA_SEGMENT_LENGTH; i++)
    				if (d_integrator[i] > best_value)
				{
					best_value = d_integrator[i];
					best_index = i;
				}

			best_correlation_value = best_value;
			best_correlation_index = best_index;

    			d_seg_locked = best_correlation_value >= MIN_SEG_LOCK_CORRELATION_VALUE;
    			//std::cout << "best = " << best_correlation_value << " min is " << MIN_SEG_LOCK_CORRELATION_VALUE << std::endl;

			// the coefficients are -1,-1,+1,+1
			d_timing_adjust = sample_mem[best_correlation_index - 3] + 
			                   sample_mem[best_correlation_index - 2] - 
			                   sample_mem[best_correlation_index - 1] - 
			                   sample_mem[best_correlation_index];


    			d_symbol_index = SYMBOL_INDEX_OFFSET - 1 - best_correlation_index;
    			if (d_symbol_index < 0)
      				d_symbol_index += ATSC_DATA_SEGMENT_LENGTH;
  		}

		if (output_items.size() == 3)
		{
			out_timing_error[k] = d_timing_adjust;
		}
		out_sample[k] = interp_sample;
		tag.valid = d_seg_locked;
		tag.symbol_num = d_symbol_index;
		out_tag[k] = tag;
	}

	return k;
}

